// STExpertJobScript::GetCompoundRate（ORIG 0x0849f366 W）。
// 遍历 m_compoundRateVec（+0x1f4）：返回首个 stCompoundRate.m_level >= level 的元素；
// 全表都 < level 时返回末元素；空表返回 0。调用方（GetSuccessRate）读 +4/+8/+0xc。
// [b28d-solo-CDataManagerScripts] AE：①P13 缓存 end（`iterator e = m_compoundRateVec.end();` 具名初始化）⇒ begin/end 槽 -0xc/-0x10 对齐；②P167 —— ORIG 三处调 `operator*`(deEv)，`it->m_level` 改 `(*it).m_level` 消 ptEv 差集；fresh clean 56=56；ORIG 0x0849f366。
stCompoundRate* STExpertJobScript::GetCompoundRate(int level)
{
    if (m_compoundRateVec.empty())
        return 0;
    std::vector<stCompoundRate>::iterator it = m_compoundRateVec.begin();
    std::vector<stCompoundRate>::iterator e = m_compoundRateVec.end();
    for (; it != e; ++it)
    {
        if ((*it).m_level >= level)
            return &(*it);
    }
    --it;  // 全部 < level：取末元素
    return &(*it);
}