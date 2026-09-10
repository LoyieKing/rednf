int ImportItemMakingSkill(const char* path, stItemMakingSkill& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x279e;

    std::string token;
    bool ok = false;

    // P164：4.4 把 while 旋转成尾部条件头；纯 goto 循环复位到 ORIG 头部条件块
    // （0x0890f5ab），ScanType 失败落尾部岛（0x0890f8b2 nop; mov $0,%ebx）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[itemmaking exp limit]")
        {
            while (true)
            {
                const int lv = ScanInt(&ok);
                if (!ok)
                    break;
                const int exp = ScanInt(&ok);
                if (!ok)
                    return 0x27b2;
                out.m_vec18.push_back(std::make_pair(lv, exp));
            }
            goto scan_top;
        }

        if (token == "[itemmaking exp]")
        {
            out.m_exp = ScanInt(&ok);
            if (!ok)
                return 0x27bb;
            out.m_bonusExp = ScanInt(&ok);
            if (!ok)
                return 0x27bf;
            goto scan_top;
        }

        if (token == "[itemmaking level limit]")
        {
            out.m_levelLimit = ScanInt(&ok);
            if (!ok)
                return 0x27c5;
            goto scan_top;
        }

        if (token == "[upgrade with compound]")
        {
            while (true)
            {
                stUpgradeWithCompound data;              // ctor 清零（ORIG clear）
                const int key = ScanInt(&ok);
                if (!ok)
                    break;

                data.m_field00 = ScanInt(&ok);
                if (!ok)
                    return 0x27d3;
                data.m_field04 = ScanInt(&ok);
                if (!ok)
                    return 0x27d7;
                data.m_field08 = ScanInt(&ok);
                if (!ok)
                    return 0x27db;
                data.m_rangeExp0C = (data.m_field04 - data.m_field00) * 1000 + 999;

                data.m_field10 = ScanInt(&ok);
                if (!ok)
                    return 0x27e1;
                data.m_field14 = ScanInt(&ok);
                if (!ok)
                    return 0x27e5;
                data.m_field18 = ScanInt(&ok);
                if (!ok)
                    return 0x27e9;
                data.m_rangeExp1C = (data.m_field14 - data.m_field10) * 1000 + 999;

                out.m_map0.insert(std::make_pair(key, data));
            }
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return 0;
