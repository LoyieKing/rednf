int importCharacLinkSystemScript(const char* path, stCharacLinkSystem_t& out)
{
    bool ok = false;
    std::string token;                  // ORIG -0x14
    std::string unused;                 // ORIG -0x18（仅构造/析构，零引用）

    if (!loadRDARScriptFile("", path))
    {
        return 0x19ab;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[link level]")
        {
            out.m_field0 = ScanInt(&ok);
            if (!ok) { return 0x19b7; }
        }
        else if (token == "[low link character exp bonus]" ||
                 token == "[low link character gold bonus]" ||
                 token == "[high link character exp bonus]" ||
                 token == "[high link character gold bonus]")
        {
            std::vector<stCharacLinkSystemData>* target = 0;
            if (token == "[low link character exp bonus]")
            {
                target = &out.m_vec4;               // +0x04
            }
            else if (token == "[low link character gold bonus]")
            {
                target = &out.m_vec10;              // +0x10
            }
            else if (token == "[high link character exp bonus]")
            {
                target = &out.m_vec1c;              // +0x1c
            }
            else if (token == "[high link character gold bonus]")
            {
                target = &out.m_vec28;              // +0x28
            }

            for (;;)
            {
                stCharacLinkSystemData data;        // ORIG ctor 0x891b762 四连清零
                data.m_field0 = ScanInt(&ok);
                if (!ok) { break; }                 // ORIG 此路径跳过 dtor，无观测差异（trivial dtor）
                data.m_field4 = ScanInt(&ok);
                if (!ok) { return 0x19d5; }
                data.m_field8 = ScanInt(&ok);
                if (!ok) { return 0x19da; }
                data.m_fieldc = ScanInt(&ok);
                if (!ok) { return 0x19df; }
                target->push_back(data);
            }
        }
    }
    (void)unused;
    return 0;
