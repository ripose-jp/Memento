# Apple Build Options
option(MEMENTO_BUNDLE "Toggle when building macOS Bundles" OFF)
set(MEMENTO_CODESIGN_IDENTITY "" CACHE STRING "macOS code signing identity for the Bundle")

# Release Options
option(MEMENTO_RELEASE_BUILD "Toggle to build with release settings" OFF)

# Debugging
option(MEMENTO_WERROR "Use -Werror when compiling" OFF)
option(MEMENTO_ASAN "Enable the address sanitizer" OFF)

# Use Local System Libraries
option(MEMENTO_SYSTEM_MOCR "Use the local installation of libmocr instead of FetchContent" OFF)
option(MEMENTO_SYSTEM_QCORO "Use the local installation of QCoro instead of FetchContent" OFF)

# Features
option(MEMENTO_OCR_SUPPORT "Support for OCR through MangaOCR" OFF)
option(MEMENTO_MECAB_SUPPORT "Support for deconjugation with MeCab" OFF)

# Modifications
option(MEMENTO_QAPPLICATION "Builds Memento with a QApplication instead of QGuiApplication" OFF)
option(MEMENTO_WIN32_EXECUTABLE "Disable console output on Windows" ON)
