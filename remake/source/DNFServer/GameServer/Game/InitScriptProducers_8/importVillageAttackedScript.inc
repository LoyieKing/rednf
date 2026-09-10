int importVillageAttackedScript(const char* path, stVillageAttacked_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
        return 0x197f;

    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[attacked map info]")
        {
            for (;;)
            {
                stAttackedMapInfo_t info;
                info.m_vill = ScanInt(&ok);
                if (!ok)
                    break;                         // 首列失败 → 回标签扫描
                info.m_area = ScanInt(&ok);
                if (!ok)
                    return 0x1990;
                info.m_field8 = ScanInt(&ok);
                if (!ok)
                    return 0x1994;
                info.m_fieldC = ScanInt(&ok);
                if (!ok)
                    return 0x1998;
                out.m_mapList.push_back(info);
            }
        }
    }
    return 0;
