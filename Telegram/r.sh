#export LDFLAGS="-L/opt/homebrew/opt/bzip2/lib"
#export CPPFLAGS="-I/opt/homebrew/opt/bzip2/include"

./configure.sh -D TDESKTOP_API_ID=56234 -D TDESKTOP_API_HASH=a797f14a68bf44dd6a5ff65d2e1af0c2

#./configure.sh -D CMAKE_OSX_ARCHITECTURES=arm64 -D DESKTOP_APP_MAC_ARCH=arm64 -D TDESKTOP_API_ID=56234 -D TDESKTOP_API_HASH=a797f14a68bf44dd6a5ff65d2e1af0c2 -D DESKTOP_APP_USE_PACKAGED=OFF -D CMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk

#/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk
#/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk

#./configure.sh -D CMAKE_OSX_ARCHITECTURES=arm64 -D CMAKE_OSX_ARCHITECTURES=arm64 -D DESKTOP_APP_MAC_ARCH=arm64 -D TDESKTOP_API_ID=56234 -D TDESKTOP_API_HASH=a797f14a68bf44dd6a5ff65d2e1af0c2 -D DESKTOP_APP_USE_PACKAGED=OFF -D CMAKE_OSX_SYSROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk
#/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
#/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk

