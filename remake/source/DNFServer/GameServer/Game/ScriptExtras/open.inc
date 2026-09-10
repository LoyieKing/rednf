int Arad_ScriptLoader::open(const char* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x10;

    int size = g_dnfLex.getInputStreamSize();
    char* buffer = new char[size + 1];
    std::memset(buffer, 0, size + 1);
    static_cast<std::istream*>(g_dnfLex.getInputStream())->read(buffer, size);
    buffer[size] = static_cast<char>(0xff);

    int ok = parsing(buffer);
    if (ok)
    {
        delete[] buffer;
        return ok;
    }
    delete[] buffer;
    return 0;
