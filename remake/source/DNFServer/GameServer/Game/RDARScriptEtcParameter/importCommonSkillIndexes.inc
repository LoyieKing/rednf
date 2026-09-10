int importCommonSkillIndexes(
    const char* path,
    std::vector<int>& commonSkill,
    std::map<ENUM_CHARACTERJOB, std::set<int> >& exceptSkill)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x329a, "%s", path);
        return 0;
    }
    // [b28d-solo] ORIG 声明序 token/ok 先于两 clear，job 先于 skills（P309 槽序）。
    std::string token;
    bool ok = false;                        // ORIG -0x55 函数级一次初始化
    commonSkill.clear();
    exceptSkill.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;                              // ORIG 共享出口（P63）
        if (token == "[common skill]")
        {
            for (;;)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                commonSkill.push_back(v);
            }
        }
        else if (token == "[except skill info]")
        {
            ENUM_CHARACTERJOB job = ENUM_CHARACTERJOB(0xb);   // ORIG -0x60 外层一次
            std::set<int> skills;                       // ORIG -0x78 外层块构造一次
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (token == "[/except skill info]")
                    break;
                job = ENUM_CHARACTERJOB(0xb);           // ORIG 每轮 movl $0xb,-0x60
                skills.clear();
                std::string jobName;                    // ORIG -0x7c 每轮构造
                if (!ScanStr(&jobName))
                    return 0;
                job = ENUM_CHARACTERJOB(getCharacterJob(jobName.c_str()));
                bool okJob;                             // ORIG -0x7d 无初始化
                for (;;)
                {
                    int v = ScanInt(&okJob);
                    if (okJob)
                        skills.insert(v);
                    else
                        break;
                }
                exceptSkill.insert(std::make_pair(job, skills));
            }
        }
        else if (token == "[/common skill]")
        {
        }
    }
    return 1;
