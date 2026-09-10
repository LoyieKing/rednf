void SetPriorityBaseDirectory(const char* dir)
{
    std::string path(dir);
    if (path.length() == 0)
    {
    }
    else
        g_szScriptFilePriorityBaseDirectory.push_back(path);
