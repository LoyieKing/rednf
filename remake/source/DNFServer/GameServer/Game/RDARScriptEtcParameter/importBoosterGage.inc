int importBoosterGage(char* path, BoosterGageData* out)
{
    if (!out)
        return 0;

    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    std::string unused;   // ORIG -0x10：构造/析构但从未使用的第二 string（照抄）
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[max gage]")
        {
            // ORIG：store 先于 ok 检查（ScanInt 返回值 %eax→%edx 截 byte 直写 +0xc）
            bool okMax = false;                    // ORIG -0x11 独立槽
            out->m_cMaxBoosterGage = (unsigned char)ScanInt(&okMax);
            if (!okMax)
                break;
        }
        else if (token == "[sub gage]")
        {
            bool okSub = false;                    // ORIG -0x12 独立槽
            out->m_cFieldD = (unsigned char)ScanInt(&okSub);
            if (!okSub)
                break;
        }
        else if (token == "[reward multiple]")
        {
            bool okRw = false;                     // ORIG -0x13 独立槽
            out->m_cMultiboxCount = (unsigned char)ScanInt(&okRw);
            if (!okRw)
                break;
        }
        else if (token == "[booster point]")
        {
            bool okBp = false;                     // ORIG 每轮 movb $0,-0x14
            BoosterPoint bp;                       // ORIG -0x19..-0x15 固定槽
            for (;;)
            {
                bp.m_dwItemIdx = (unsigned long)ScanInt(&okBp);   // →-0x19
                if (!okBp)
                    break;
                bp.m_cPoint = (unsigned char)ScanInt(&okBp);      // al 直写 -0x15
                if (!okBp)
                    break;
                out->m_BoosterPoint.push_back(bp);
            }
        }
    }
    return 1;
