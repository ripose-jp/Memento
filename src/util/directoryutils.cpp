#include "directoryutils.h"

QString DirectoryUtils::getConfigDir()
{
    QString path = getenv(ENV_VAR);
    path += CONFIG_PATH;
    path += SLASH;
    return path;
}

QString DirectoryUtils::getDictionaryDir()
{
    return getConfigDir() + DICTIONARY_DIR + SLASH;
}

QString DirectoryUtils::getCacheDir()
{
    return getConfigDir() + CACHE_DIR + SLASH;
}