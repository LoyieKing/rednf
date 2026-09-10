bool loadString(CharString* error)
{
    return app::stringResource()->loadFromFile(STRING_FILE, error);
