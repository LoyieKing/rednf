// ============================================================================
// loadPvpMapBasicDataScript —— ORIG 0x0890a427
// [pvp map order]：idx 入 PVPMAP_DATA::m_mapIdx（m_name ctor 后 clear）push_back；
// [pvp map basic data]：首列弃读 + 6 列入全局表（3 int 表 + 3 bool 化 char 表），
// 计满 0x14 条打印超限日志并仍返回成功；EOF 返回成功；打开失败返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890a427：clean 160/160（ORIG 164 insns）；
// p167 CLEAR。已对齐：首列弃读、6 列全局表写、entryIdx>0x13 日志 + break、
// [pvp map order] while(out!=0) 形、PVPMAP_DATA ctor + clear + push_back。
bool loadPvpMapBasicDataScript(const char* path, std::vector<PVPMAP_DATA>* out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
            0x1c00,
            "error : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe"
            "\xec\x9d\x84 \xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88"
            "\xeb\x8b\xa4.",
            path);
        return false;
    }

    if (out != 0)
        out->clear();

    std::string token;
    int orderIdx = 0;
    bool ok = false;
    int entryIdx = 0;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                                 // EOF → 共享尾块 return true

        if (token == "[pvp map basic data]")
        {
            const int firstCol = ScanInt((bool*)0); // ORIG 首列读入 -0xc 后弃用
            (void)firstCol;
            PVPMAP_BIG_IMAGE[entryIdx] = ScanInt((bool*)0);
            PVPMAP_SMALL_IMAGE[entryIdx] = ScanInt((bool*)0);
            PVPMAP_DEATHMATCH_MODE[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            PVPMAP_ONLY_TOURNAMENT_EXPOSURE[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            PVPMAP_ONLY_NORMAL_EXPOSURE[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            PVPMAP_ONLY_TOURNAMENT_RANDOM[entryIdx] =
                static_cast<char>(ScanInt((bool*)0) != 0);
            ++entryIdx;
            if (entryIdx > 0x13)                   // ORIG cmpl $0x13,jle 继续
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x1c2c,
                    "ERROR : %d \xea\xb0\x9c \xec\x9d\xb4\xec\x83\x81\xec\x9d"
                    "\x98 pvp map \xec\x9d\x84 \xec\xb6\x94\xea\xb0\x80\xed"
                    "\x95\xa0\xec\x88\x98\xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88"
                    "\xeb\x8b\xa4!!!",
                    0x14);
                break;
            }
        }
        else if (token == "[pvp map order]")
        {
            // ORIG do-while 布局：先 jmp 条件，`out != 0` 走 setne 物化。
            while (out != 0)
            {
                orderIdx = ScanInt(&ok);           // ORIG 复用 -0x14 槽
                if (!ok)
                    break;                         // → 回外层标签扫描
                PVPMAP_DATA data;                  // ctor：仅构造 m_name
                data.m_mapIdx = orderIdx;
                data.m_name.clear();               // ORIG push 前 string::clear
                out->push_back(data);
            }
        }
    }
    return true;
}