// ============================================================================
// LoadRewardDataByGrowType —— ORIG T 0x08917f44
// 扫描到 endTag 结束；每条：deleteTagBracketString→getCharacterJobToEnumValue
// （-1 → 0x3693），grow=ScanInt 必需；循环 level=ScanInt（失败→回到外层标签循
// 环）、data 两 int（失败→0x36a3/0x36a7）插入 out.m_map0。返回 0=正常结束。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=IDENTICAL_AE（原 DIFF 117）：
//   ①key/data 提到**函数头**构造一次（ORIG 0x08917f5b/0x08917f66，非每轮重构造）；
//   ②P164 纯 goto 循环复位头部条件块（0x08917f71），m_grow 失败与 ScanType 失败
//   共享尾部 `mov $0,%ebx` 块。
int LoadRewardDataByGrowType(stLevelUpRewardItemScript& out, std::string endTag)
{
    std::string token;
    bool ok = false;
    // ORIG 0x08917f5b/0x08917f66：key(-0x36)/data(-0x40) 在**函数头**各构造一次
    // （循环外复用），非每轮重构造。
    LevelUpRewardItemTableKey key;
    LevelUpRewardItemData data;

    // P164：4.4 把 while 旋转成尾部条件头；纯 goto 循环复位到 ORIG 的头部条件块
    // （0x08917f71），ScanType 失败与 m_grow 失败共享尾部 `mov $0,%ebx` 块。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == endTag)
            return 0;

        deleteTagBracketString(token);
        const int job = getCharacterJobToEnumValue(token);
        if (job == -1)
            return 0x3693;

        key.m_job = (unsigned char)job;                  // +0x02
        key.m_grow = (unsigned char)ScanInt(&ok);        // +0x03
        if (!ok)
            goto scan_done;

        while (true)
        {
            key.m_lv = (short)ScanInt(&ok);              // +0x00（b28c-L20: signed short）
            if (!ok)
                break;                                   // 回到外层标签扫描

            data.m_field0 = ScanInt(&ok);
            if (!ok)
                return 0x36a3;
            data.m_field4 = ScanInt(&ok);
            if (!ok)
                return 0x36a7;

            out.m_map0.insert(
                std::make_pair(key, data));
        }
    }
    goto scan_top;
scan_done:
    return 0;
}