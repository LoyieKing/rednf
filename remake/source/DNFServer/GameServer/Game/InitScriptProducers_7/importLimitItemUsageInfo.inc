// ============================================================================
// importLimitItemUsageInfo — ORIG 0x0890d871（init.md +0x562b，
// "Etc/Chn_Server_LimititemUsageinfo.etc" → m_limitItemUsage）。
// 三段分别填充 Ex 结构的三个子表；日期串经 ConvertStringToTime 转秒。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0890d871
// 按 ORIG 逐段重排：
//  ① [Limit Item Usage Info] 首个 ScanInt 存 char 槽(-0x1d) ⇒ 源码首参是 char kind，
//    第二/三参是 int a(-0x1c)/b(-0x18)（原重建把 kind/a 类型写反）；
//  ② 循环头 ScanType 检查（P164）+ ok 失败 break/return 双出口；
//  ③ itemIdx(-0x14)/itemId(-0x10)/count(-0xc) 循环外声明并预清（ORIG movl $0）；
//  ④ ORIG -0x24 循环外 dead int 预清（P171/P232）补齐帧。
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
}