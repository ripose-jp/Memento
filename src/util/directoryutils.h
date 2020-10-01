#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QString>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define ENV_VAR "APPDATA"
    #define CONFIG_PATH "\\memento"
    #define SLASH "\\"
#elif __linux__
    #define ENV_VAR "HOME"
    #define CONFIG_PATH "/.config/memento"
    #define SLASH "/"
#elif __APPLE__
    #if TARGET_OS_MAC
        #define ENV_VAR "HOME"
        #define CONFIG_PATH "/.config/memento"
        #define SLASH "/"
    #else
        #error "Apple OS type no supported"
    #endif
#else
    #error "OS not supported"
#endif

#define DICTIONARY_DIR "dict"
#define CACHE_DIR "cache"

class DirectoryUtils
{
public:
    static QString getConfigDir();
    static QString getDictionaryDir();
    static QString getCacheDir();
    
private:
    DirectoryUtils() {}
};

#endif // PARAMETERS_H