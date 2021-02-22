/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "editor/photo_editor.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "editor/color_picker.h"
#include "editor/photo_editor_content.h"
#include "editor/photo_editor_controls.h"
#include "editor/undo_controller.h"
#include "styles/style_editor.h"

namespace Editor {
namespace {

constexpr auto kPrecision = 100000;

[[nodiscard]] QByteArray Serialize(const Brush &brush) {
	auto result = QByteArray();
	auto stream = QDataStream(&result, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_5_3);
	stream << qint32(brush.sizeRatio * kPrecision) << brush.color;
	stream.device()->close();

	return result;
}

[[nodiscard]] Brush Deserialize(const QByteArray &data) {
	auto stream = QDataStream(data);
	auto result = Brush();
	auto size = qint32(0);
	stream >> size >> result.color;
	result.sizeRatio = size / float(kPrecision);
	return (stream.status() != QDataStream::Ok)
		? Brush()
		: result;
}

} // namespace

PhotoEditor::PhotoEditor(
	not_null<Ui::RpWidget*> parent,
	std::shared_ptr<Image> photo,
	PhotoModifications modifications,
	EditorData data)
: RpWidget(parent)
, _modifications(std::move(modifications))
, _undoController(std::make_shared<UndoController>())
, _content(base::make_unique_q<PhotoEditorContent>(
	this,
	photo,
	_modifications,
	_undoController,
	std::move(data)))
, _controls(base::make_unique_q<PhotoEditorControls>(
	this,
	_undoController,
	_modifications))
, _colorPicker(std::make_unique<ColorPicker>(
	this,
	Deserialize(Core::App().settings().photoEditorBrush()))) {

	sizeValue(
	) | rpl::start_with_next([=](const QSize &size) {
		if (size.isEmpty()) {
			return;
		}
		const auto geometry = QRect(QPoint(), size);
		const auto contentRect = geometry
			- style::margins(0, 0, 0, st::photoEditorControlsHeight);
		_content->setGeometry(contentRect);
		const auto controlsRect = geometry
			- style::margins(0, contentRect.height(), 0, 0);
		_controls->setGeometry(controlsRect);

		_colorPicker->moveLine(QPoint(
			controlsRect.x() + controlsRect.width() / 2,
			controlsRect.y() + st::photoEditorColorPickerTopSkip));
	}, lifetime());

	_mode.value(
	) | rpl::start_with_next([=](const PhotoEditorMode &mode) {
		_content->applyMode(mode);
		_controls->applyMode(mode);
		_colorPicker->setVisible(mode.mode == PhotoEditorMode::Mode::Paint);
	}, lifetime());

	_controls->rotateRequests(
	) | rpl::start_with_next([=](int angle) {
		_modifications.angle += 90;
		if (_modifications.angle >= 360) {
			_modifications.angle -= 360;
		}
		_content->applyModifications(_modifications);
	}, lifetime());

	_controls->flipRequests(
	) | rpl::start_with_next([=] {
		_modifications.flipped = !_modifications.flipped;
		_content->applyModifications(_modifications);
	}, lifetime());

	_controls->paintModeRequests(
	) | rpl::start_with_next([=] {
		_mode = PhotoEditorMode{
			.mode = PhotoEditorMode::Mode::Paint,
			.action = PhotoEditorMode::Action::None,
		};
	}, lifetime());

	_controls->doneRequests(
	) | rpl::start_with_next([=] {
		const auto mode = _mode.current().mode;
		if (mode == PhotoEditorMode::Mode::Paint) {
			_mode = PhotoEditorMode{
				.mode = PhotoEditorMode::Mode::Transform,
				.action = PhotoEditorMode::Action::Save,
			};
		} else if (mode == PhotoEditorMode::Mode::Transform) {
			save();
		}
	}, lifetime());

	_controls->cancelRequests(
	) | rpl::start_with_next([=] {
		const auto mode = _mode.current().mode;
		if (mode == PhotoEditorMode::Mode::Paint) {
			_mode = PhotoEditorMode{
				.mode = PhotoEditorMode::Mode::Transform,
				.action = PhotoEditorMode::Action::Discard,
			};
		} else if (mode == PhotoEditorMode::Mode::Transform) {
			_cancel.fire({});
		}
	}, lifetime());

	_colorPicker->saveBrushRequests(
	) | rpl::start_with_next([=](const Brush &brush) {
		_content->applyBrush(brush);

		const auto serialized = Serialize(brush);
		if (Core::App().settings().photoEditorBrush() != serialized) {
			Core::App().settings().setPhotoEditorBrush(serialized);
			Core::App().saveSettingsDelayed();
		}
	}, lifetime());
}

void PhotoEditor::save() {
	_content->save(_modifications);
	_done.fire_copy(_modifications);
}

rpl::producer<PhotoModifications> PhotoEditor::doneRequests() const {
	return _done.events();
}

rpl::producer<> PhotoEditor::cancelRequests() const {
	return _cancel.events();
}

} // namespace Editor
