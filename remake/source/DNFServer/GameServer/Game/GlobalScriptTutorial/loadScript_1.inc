bool GlobalScriptTutorial::loadScript(const char* directory, const char* path)
{
    if (!loadRDARScriptFile(directory, path))
        return false;

    ENUM_CHARACTERJOB job = static_cast<ENUM_CHARACTERJOB>(11);
    while (true)
    {
        std::string token;
        if (!ScanType(token, true))
            break;
        if (token == "[dialog]")
            LoadDialogTag(job);
        else if (token == "[avatar]")
            LoadAvatarTag(job);
        else if (token == "[equipment]")
            LoadEquipmentTag(job);
        else if (token == "[dungeonindex]")
            LoadDungeonIndex(job);
        else if (token == "[skill]")
            LoadSkillTag(job);
        else if (token == "[passive skill]")
            LoadPassiveSkilltag(job);
        else if (token == "[tutorial notice]")
            LoadTutorialNotice(job);
        else if (token == "[item]")
            LoadItem(job);
        else
            job = static_cast<ENUM_CHARACTERJOB>(getCharacterJob(token.c_str()));
    }
    unloadRDARScriptFile();
    return true;
