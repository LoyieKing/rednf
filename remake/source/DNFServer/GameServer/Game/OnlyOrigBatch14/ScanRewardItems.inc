bool ScanRewardItems(std::vector<RewardItemInfo>& out, std::string token)
{
    // ok2 无初始化式（ScanTypeOrInt 写入后即读）；ORIG 预清仅两条
    // （`movb $0,-0xe; movb $0,-0xf`）。声明序三形实测见函数头判词。
    // [L13-b14 判据⑩ 槽序反转律试验] ORIG 槽深→浅 = ok1(-0xf)/unusedFlag(-0xe)/
    // ok2(-0xd)/v(-0xc)；按「深=先声明」把 ok1 提到声明序首位（形 A：123=123，
    // info/s 槽 -0x20/-0x24 与 ORIG 一致）。形 B（v 提前）实测 info/s 整体下移 4
    // （-0x24/-0x28）⇒ 回滚。残余 = 4.4 对 int(v)/bool 分组方向与 4.1 相反
    // （v 恒最深 -0x10 vs ORIG -0xc）⇒ N82/N126 版本槽基差封顶。
    // [P1T1 本波复核] 现状 = **NEAR**（124=124，助记符逐条相同，仅 -0x9/-0xa/-0xb/
    // -0x10 与 ORIG -0xf/-0xe/-0xd/-0xc 的槽号差；fdiff 16 块全为槽号替换）。
    // 4.4 对 bool 组与 int(v) 组按**相反**方向分组（我方 bools 浅/v 深），
    // 形 A/B 已实测（形 B 令 info/s 下移 4 更劣）⇒ 无新源形，维持 NEAR 不回退。
    bool ok1 = false;
    bool unusedFlag = false;
    bool ok2;
    int v;
    RewardItemInfo info;
    std::string s;
    for (;;)
    {
        v = ScanTypeOrInt(ok1, s, ok2, 1);
        if (ok1)
        {
            // [b14 本波 P325/P223 试验] ORIG 形 `test %al,%al; jne <真岛>` +
            // fall-through `mov $0x0,%ebx`（false 紧随；真岛在函数后部 +0x164）。
            // if-return / `!=` 两形实测均把 true 块前置（N93附 跨版本块序反差）
            // ⇒ 改 goto 尾标签形，让 4.4 走 `je <跳过>; jmp <尾标签>` 并由
            // jump-threading 折成 ORIG 的 `jne <真岛>`。
            if (s == token)
                goto scan_reward_true;
            return false;
        }
        if (!ok2)
            return false;
        info.m_itemIdx = v;
        info.m_job = 0xb;
        v = ScanStrOrInt(ok2, s, ok1, 1);
        if (!ok2)
            return false;
        // [b14 报告核对·真语义缺口] ORIG 0x08a65e6a..0x8a65eec 控制流（报告
        // Ghidra C 0x8a65e6a 段 + objdump 逐条核对）：
        //   ok1==0 → je 0x8a65eec（共享 m_count 赋值尾，直接用 ScanStrOrInt 返回值）；
        //   ok1==1 → s=="[job]" 时读 m_job/m_growType，随后**无条件**再 ScanInt；
        //            非 "[job]" 时也执行该 ScanInt（je 0x12e 落在同一段）。
        // 原实现把最后那次 ScanInt 写成 else 分支 ⇒ [job] 情形少读一个 int（真 bug）。
        // 同时 ORIG 全程只用一个 -0xc 变量（v 兼作 ScanInt 中转槽，Ghidra local_10
        // 贯穿），故删除 v2。
        if (ok1)
        {
            if (s == "[job]")
            {
                if (!ScanInt(&v))
                    return false;
                info.m_job = v;
                if (!ScanInt(&v))
                    return false;
                info.m_growType = v;
            }
            if (!ScanInt(&v))
                return false;
        }
        info.m_count = v;
        out.push_back(info);
    }
scan_reward_true:
    return true;
}
