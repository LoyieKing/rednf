bool importUltimateDungeonList(const char* path, std::set<int>& out)  // [P330] 与 DataManagerInitDepends.h 同步
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
            0x3209,
            "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe"
            "\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88"
            "\xeb\x8b\xa4.",
            path);
        return 0;
    }

    std::string token;
    bool ok;
    int value;

    out.clear();                                   // ORIG set::clear
    for (;;)
    {
        if (!ScanType(token, true))
            break;                                 // EOF → 共享尾块 return 1
        ok = false;                                // ORIG local_19=false 每轮重置
        if (token == "[apply ultimate]")
        {
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out.insert(value);
            }
            continue;
        }
        if (token == "[/apply ultimate]")
            break;                                 // 结束标记 → 直接收尾
        // 其他 token：ORIG do-while(!=)[/apply ultimate]) 继续吞并
    }
    return 1;
