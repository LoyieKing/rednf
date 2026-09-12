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
            "error : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb \xbc\xf6 "
            "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
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
                    "ERROR : %d \xb0\xb3 \xc0\xcc\xbb\xf3\xc0\xc7 pvp map "
                    "\xc0\xbb \xc3\xdf\xb0\xa1\xc7\xd2\xbc\xf6\xbe\xf8\xbd"
                    "\xc0\xb4\xcf\xb4\xd9!!!",
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