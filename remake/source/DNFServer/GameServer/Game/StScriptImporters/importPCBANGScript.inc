int importPCBANGScript(const char* path, std::vector<stNeoplePCBANG>& out)
{
    // [b28d-solo-StScriptImporters] AE：指令数 393=393、p167 69/69 CLEAR。
    // [SweepStScriptImporters 2026-09-10 复核] jmp+jcc 46/46 目标下标逐位全等（回边 21=21）。
    // 修复：①外层改 for(;;){ if(!ScanType) break; } 破 4.4 循环旋转（ORIG 入口 jmp cond +
    // 各 case 尾 jmp cond）；②补 ORIG -0x10/-0xc 两个死 int 局部（P232 槽序，令
    // token/data/statusPair 槽位 -0x14/-0x2c/-0x34 与 ORIG 精确对齐）。
    if (!loadRDARScriptFile("", path))
        return 0x1417;
    std::string token;
    int dead0 = 0;    // [b28d-solo-StScriptImporters] ORIG -0x10 死局部（P232 槽序）
    int dead1 = 0;    // [b28d-solo-StScriptImporters] ORIG -0xc 死局部
    out.clear();
    stNeoplePCBANG data;
    std::pair<ENUM_CHANGE_STATUS_TYPE, int> statusPair;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[level]")
        {
            data.m_level = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[people count]")
        {
            data.m_peopleCount = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[physical attack]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[physical defense]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(3);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[magical attack]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(1);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[magical defense]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(2);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[fire resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x14);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[water resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x15);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[dark resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x16);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[light resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x17);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[HP regen speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xe);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[MP regen speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x1d);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[move speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xb);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[attack speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(10);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[cast speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xc);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[hit recovery]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x22);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[jump power]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(8);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[HP MAX]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xd);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[MP MAX]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x30);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[/level]")
        {
            out.push_back(data);
            data.clear();
        }
    }
    return 1;
