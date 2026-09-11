// ---------------------------------------------------------------------------
// loadString —— ORIG T 0x08ac35cc：stringResource()->loadFromFile(path,err)。
// ---------------------------------------------------------------------------
bool loadString(CharString* error)
{
    return app::stringResource()->loadFromFile(STRING_FILE, error);
}