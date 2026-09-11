// ============================================================================
// importCharacLinkSystemScript —— ORIG T 0x08909529
//   int (const char*, stCharacLinkSystem_t&)，0=成功。
// [link level] 写 +0x00；四个 bonus 节各自选定目标 vector 后 do-while 连续装
// 4×ScanInt 记录；未匹配节名静默跳过（ORIG al=0 → 回外层）。
// ============================================================================
// b28d-solo-InitScriptProducers_6: ORIG 0x08909529 — 第二个 string 局部
// unused(-0x18，仅 ctor/dtor)；[link level] ScanInt 直存无局部；四 bonus 节
// 合并 `||` 短路链后**四路** else-if 选 target（第 4 路仍比较，无裸 else）。
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
}