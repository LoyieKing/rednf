// b28d-20: ORIG 0x08ab8ffe。修法：token 移入循环体 + while(true)/`if(!ScanType)break;`
// （P0 无槽往返=表达式右值；ORIG 每轮构造/析构 token 并复用 ebx 作循环/返回汇聚标志）。
// 结果 IDENTICAL_AE（155=156，尾 nop 剥离）。
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
}