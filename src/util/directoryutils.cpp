#include "directoryutils.h"

QString DirectoryUtils::getConfigDir()
{
    QString path = getenv(ENV_VAR);
    path += CONFIG_PATH;
    return path;
}

QString DirectoryUtils::getDictionaryDir()
{
    return getConfigDir() + SLASH + DICTIONARY_DIR;
}

QString DirectoryUtils::getCacheDir()
{
    return getConfigDir() + SLASH + CACHE_DIR;
}