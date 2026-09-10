bool importConditionEventScript(const char* path, stConditionEventInfo* out)
{
    // ORIG T 0x0890e868（323 条；orig=ours=323，助记符序列仅 4 处 4.1 块放置
    // 顺序差：entry jmp/nop landing 前置与 EOF dtor 块位——跨版本 CFG 布局，
    // 无计数/调用/操作数缺口）。四 token 段 reward/quick 为**各自独立展开**
    // （ORIG 槽组 -0xbc/-0x28..-0x1c vs -0xc8/-0x18..-0xc 两套即证）；map/
    // multimap 提交为 make_pair(lvalue)+pair 转换 ctor+insert 三段 call 形
    // （P61/P123，value_type 直构会少一段）；[id] 节 ScanInt 失败 → break
    // 主循环 → return true；主循环外还有一个仅构造/析构的 dead string
    // （ORIG -0xa4）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                 // ORIG -0xa0
    std::string unused;                // ORIG -0xa4（ctor/dtor 各一次，零引用）
    bool ok = false;                   // ORIG -0xa5

    for (;;)
    {
        if (!ScanType(token, true))
        {
            goto loop_exit;                        // 共享 true 出口岛（ORIG +0x4b2）
        }

        if (token == "[id]")
        {
            int v = ScanInt(&ok);
            if (ok != true)
            {
                goto loop_exit;
            }
            // [L16 本轮] 唯一残差点（NEAR ndiff=2）：ORIG `mov v,%edx; mov out,%eax;
            // mov %edx,(%eax)` = RHS-first ⇒ RHS 异型化（P211 档③）。
            out->m_field0U = v;                    // ORIG mov %edx,(%eax) → +0x00
        }
        else if (token == "[condition match]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // ORIG -0xac
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x30
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x2c
                if (ok != true) { break; }
                std::pair<int, int> value;         // 默认构造后 first/second 赋值（ORIG +0x14a 序）
                value.first = v1;
                value.second = v2;
                out->m_map8.insert(std::make_pair(key, value));
            }
        }
        else if (token == "[condition reward]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // -0xb8
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x28
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x24
                if (ok != true) { break; }
                int v3 = ScanInt(&ok);             // -0x20
                if (ok != true) { break; }
                int v4 = ScanInt(&ok);             // -0x1c
                if (ok != true) { break; }
                stConditionEventRewardInfo info;   // 默认构造（ORIG 0891b682 每轮）
                info.m_field0 = v1;
                info.m_field4 = v2;
                info.m_flag8 = (v3 != 0);          // cmpl$0;setne 直存
                info.m_flag9 = (v4 != 0);
                out->m_multiMap20.insert(std::make_pair(key, info));   // +0x20
            }
        }
        else if (token == "[condition quick party add reward]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // -0xc8
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x18
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x14
                if (ok != true) { break; }
                int v3 = ScanInt(&ok);             // -0x10
                if (ok != true) { break; }
                int v4 = ScanInt(&ok);             // -0xc
                if (ok != true) { break; }
                stConditionEventRewardInfo info;
                info.m_field0 = v1;
                info.m_field4 = v2;
                info.m_flag8 = (v3 != 0);
                info.m_flag9 = (v4 != 0);
                out->m_multiMap38.insert(std::make_pair(key, info));   // +0x38
            }
        }
    }
loop_exit:
    return true;
