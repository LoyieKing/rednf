bool importConditionEventChkDungeonScript(const char* path,
                                          ConditionLevelChkDungeon* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x10
    std::string unused;                 // ORIG -0x14（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x15

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[apply level]")
        {
            int v = ScanInt(&ok);
            out->m_applyLevel = v;                  // +0x0c
        }
        else if (token == "[unable dungeon]")
        {
            out->m_unableDungeons.clear();
            for (;;)
            {
                int dungeonIdx = ScanInt(&ok);
                if (!ok) { break; }
                out->m_unableDungeons.push_back(dungeonIdx);
            }
        }
    }
    (void)unused;
    return true;
