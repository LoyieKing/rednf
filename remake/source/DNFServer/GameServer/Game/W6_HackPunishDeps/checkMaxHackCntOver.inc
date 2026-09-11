// ----------------------------------------------------------------------------
// checkMaxHackCntOver —— ORIG W 0x08695360。
// 遍历 m_hackTypeMap 中 key == type 的 [lower_bound, upper_bound) 区间；
// 规则条目按两个对称条件判断"累计计数超限"（条件成立即置 flag）：
//   A: m_type1==type && m_value1<=param && getServerHackAccumulatedCnt(m_type2)>=m_value2
//   B: m_type2==type && m_value2<=param && getServerHackAccumulatedCnt(m_type1)>=m_value1
// flag 置位后交由 _checkApplyFlag 处置；返回 true 表示命中处置（调用方终止）。
// ----------------------------------------------------------------------------
bool CAutoPunishRuleHackTypeMgr::checkMaxHackCntOver(
    ENUM_HACKTYPE type, unsigned int param, CHackAnalyzer* analyzer,
    HackTypeInfo_t& outInfo, std::vector<stLookOuts>& lookOutsA,
    std::vector<tyIPCounters>& ipCounters, std::vector<stLookOuts>& lookOutsB,
    char* ip)
{
    // b28d-solo-checkMaxHackCntOver: **AE 131/131**（check_tu_routed fresh）。
    // 历次修复：去掉 (unsigned short)type cast（复现 32 位 cmp）、m_value2 左置
    // 复现 call 前 %ebx 缓存、A/B 条件改 else-if 链。本轮两处翻正：
    //  ① P225 推广：`if (flag && _checkApplyFlag(...))` 改**嵌套 if** —— 合并形
    //     在 4.4 物化 bool（mov $0/mov $1+test），多 1 个 je+1 个 jmp，jcc 序列
    //     与 ORIG 失配（jcc 逐位核对：修前 MISMATCH、修后 MATCH）。
    //  ② P326 变体：去掉具名 `iterator lower/upper` 中间量、直接以 const_iterator
    //     接收 lower_bound/upper_bound 的返回值 —— 复现 ORIG 的 sret 临时槽阶梯
    //     lower(-0x14)/cit(-0x18)、upper(-0x10)/citEnd(-0x1c)（旧形 lower 组反而
    //     落在浅槽，帧 0x54 两侧一致但槽号互换）。
    // ORIG 0x08695360。
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::const_iterator cit =
        m_hackTypeMap.lower_bound(type);
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::const_iterator citEnd =
        m_hackTypeMap.upper_bound(type);

    for (; cit != citEnd; ++cit)
    {
        HackTypeInfo_t info = cit->second;
        bool flag = false;
        if (info.m_type1 == type &&
            info.m_value1 <= param &&
            info.m_value2 <= analyzer->getServerHackAccumulatedCnt(
                (ENUM_HACKTYPE)info.m_type2))
        {
            flag = true;
        }
        // ORIG: A 成立即置 flag 并跳过 B（else-if 链）。
        else if (info.m_type2 == type &&
                 info.m_value2 <= param &&
                 info.m_value1 <= analyzer->getServerHackAccumulatedCnt(
                     (ENUM_HACKTYPE)info.m_type1))
        {
            flag = true;
        }
        // P225 推广：ORIG 为 `cmpb flag; je 尾; call; test %al; je 尾; mov $1`
        // 的**嵌套 if 直跳**形；合并 `flag && f()` 在 4.4 会物化 bool
        // （mov $0/mov $1 + test）多出 1 个 je + 1 个 jmp（jcc 序列失配）。
        if (flag)
        {
            if (_checkApplyFlag(info, analyzer, outInfo, lookOutsA,
                                ipCounters, lookOutsB, ip))
            {
                return true;
            }
        }
    }
    return false;
}