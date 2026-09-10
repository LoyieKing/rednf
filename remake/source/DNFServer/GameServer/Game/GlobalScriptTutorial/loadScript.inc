bool GlobalScriptTutorial::loadScript()
{
    if (!loadRDARScriptFile("Etc/GlobalTutorialInfo/", "CharacterList.lst"))
        return false;

    std::vector<std::string> files;
    while (true)
    {
        std::string token;
        if (!ScanType(token, true))
            break;
        if (token == "[list]")
        {
            while (true)
            {
                std::string s;
                if (!ScanStr(&s))
                    break;
                files.push_back(s);
            }
        }
    }
    unloadRDARScriptFile();

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        if (!loadScript("Etc/GlobalTutorialInfo/", files[i].c_str()))
            return false;
    }
    return true;
