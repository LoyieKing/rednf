int ImportBingoScript(const char* path, stBingoScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool ok = false;

    // P164：纯 goto 循环复位头部条件块（ORIG 0x08918800）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[board]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;                          // ORIG 0x08918c96 尾部岛
            // ++i 必须留在 for 增量位：ORIG 0x089189d3 `addl $1,-0x24` 在
            // values 析构（0x089189ce D1）之后；写进循环体会被 4.4 提前到析构前。
            for (int i = 0; i < count; ++i)
            {
                // 非 const 左值 ⇒ ORIG make_pair<int&,vector<int>&>（p167）。
                int key = ScanInt(&ok);
                if (!ok)
                    break;                               // 直接回主循环，不插入

                std::vector<int> values;                 // 每行新建（ORIG ctor+clear）
                values.clear();                          // ORIG 0x089188ad 显式 clear
                int limit = 25;                          // ORIG 上限存槽 -0x20（setl 形）
                for (int j = 0; j < limit; )
                {
                    const int v = ScanInt(&ok);
                    if (!ok)
                        break;
                    values.push_back(v);
                    ++j;
                }
                out.m_map18.insert(std::make_pair(key, values));
            }
            goto scan_top;
        }

        if (token == "[quiz value]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;
            for (int i = 0; i < count; )
            {
                int k = ScanInt(&ok);
                if (!ok)
                    goto scan_done;
                int v = ScanInt(&ok);
                if (!ok)
                    goto scan_done;
                out.m_map0.insert(std::make_pair(k, v));
                ++i;
            }
            goto scan_top;
        }

        if (token == "[reward]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;
            for (int i = 0; i < count; ++i)
            {
                std::pair<int, int> reward;              // 默认构造（ORIG pair()）
                int a = ScanInt(&ok);
                if (!ok)
                    break;
                reward.first = ScanInt(&ok);
                if (!ok)
                    break;
                reward.second = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_map30.insert(std::make_pair(a, reward));
            }
            goto scan_top;
        }

        if (token == "[use item]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_vec48.push_back(v);
            }
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return 0;
