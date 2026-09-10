bool importLimitItemUsageInfo(char* path, stLimitItemUsageInfoEx* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                         // ORIG -0x30
    std::string dateText;                      // ORIG -0x34
    int unused24 = 0; (void)unused24;          // ORIG -0x24（循环外预清，全程无读）
    bool ok = false;                           // ORIG -0x35

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        if (token == "[Limit Item Usage Info]")
        {
            for (;;)
            {
                const char kind = (char)ScanInt(&ok);   // ORIG mov%al,-0x1d（char 槽）
                if (!ok)
                {
                    break;                                   // 回主循环
                }
                const int a = ScanInt(&ok);              // ORIG -0x1c
                if (!ok)
                {
                    return false;
                }
                const int b = ScanInt(&ok);              // ORIG -0x18
                if (!ok)
                {
                    return false;
                }
                out->m_info0.Add(kind, a, b);
            }
        }
        else if (token == "[reset item]")
        {
            int itemIdx = 0;                     // ORIG -0x14（块首预清）
            for (;;)
            {
                itemIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (!ScanStr(&dateText))
                {
                    return false;
                }
                long endTime;                    // ORIG -0x3c（无预清）
                if (!ConvertStringToTime(dateText, endTime))
                {
                    return false;
                }
                out->m_resetC.Add(itemIdx, endTime);
            }
        }
        else if (token == "[refill item]")
        {
            int itemId = 0;                      // ORIG -0x10（块首预清）
            int count = 0;                       // ORIG -0xc（块首预清）
            for (;;)
            {
                itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                count = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                if (!ScanStr(&dateText))
                {
                    return false;
                }
                long endTime;                    // ORIG -0x40（无预清）
                if (!ConvertStringToTime(dateText, endTime))
                {
                    return false;
                }
                out->m_refill18.Add(itemId, count, endTime);
            }
        }
    }
    return true;
