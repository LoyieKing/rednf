int importInvalidName(char* path, InvalidCharData* out)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    // [b28d-solo-RDARScriptEtcParameter] ORIG 0x08908338 真形（Ghidra C 复核）：
    // 外层 while(ScanType) + 四个**独立 if**（非 else-if 链）+ 内层 while 条件循环
    // （逗号表达式内联 ScanInt）；ok 变量 6 个独立（okA..okF），lo/hi 跨块共享。
    // [b28d-solo] 4.4 头部声明「先声明浅」⇒ 按 ORIG 槽浅→深排列（okA=-0x21..okF=-0x26，P309）。
    std::string token;                      // ORIG -0x20
    bool okA;                               // ORIG -0x21
    bool okB;                               // ORIG -0x22
    bool okC;                               // ORIG -0x23
    bool okD;                               // ORIG -0x24
    bool okE;                               // ORIG -0x25
    bool okF;                               // ORIG -0x26
    int lo;                                 // ORIG -0x14
    int hi;                                 // ORIG -0x10
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[one byte range]")
        {
            for (;;)
            {
                lo = ScanInt(&okA);
                if (!okA)
                    break;
                hi = ScanInt(&okB);
                if (okA)
                {
                    if (okB)
                    {
                        out->m_charRange.push_back(
                            InvalidCharData::stRangeElement<unsigned char>(
                                (unsigned char)lo, (unsigned char)hi));
                    }
                    else
                        break;
                }
                else
                    break;
            }
        }
        if (token == "[one byte]")
        {
            for (;;)
            {
                lo = ScanInt(&okC);
                if (okC)
                    out->m_charList.push_back((unsigned char)lo);
                else
                    break;
            }
        }
        if (token == "[two byte range]")
        {
            for (;;)
            {
                lo = ScanInt(&okD);
                if (!okD)
                    break;
                hi = ScanInt(&okE);
                if (okD)
                {
                    if (okE)
                    {
                        out->m_shortRange.push_back(
                            InvalidCharData::stRangeElement<unsigned short>(
                                (unsigned short)lo, (unsigned short)hi));
                    }
                    else
                        break;
                }
                else
                    break;
            }
        }
        if (token == "[two byte]")
        {
            for (;;)
            {
                lo = ScanInt(&okF);
                if (okF)
                    out->m_shortList.push_back((unsigned short)lo);
                else
                    break;
            }
        }
    }
    return 1;
