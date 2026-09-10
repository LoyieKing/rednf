bool stSeriaBlessingScript::FindOneByLv::operator()(
    const std::pair<const int, Value>& p) const
{
    if (p.second.m_minLv == p.second.m_maxLv)
    {
        if (p.second.m_minLv == m_lv)
            return true;
    }
    // b25c：ORIG 两 if 共享后置 mov$0 块（jg/jge 同落 +0x4e）；4.4 对独立
    // if-return 内联 mov$0。!(A||B) 形（探针 /tmp/probe_fob.cpp f2）复现共享
    // 块布局，残差仅块4 cmp 操作数序反转（N22 族）。
    return !(p.second.m_minLv > m_lv || m_lv >= p.second.m_maxLv);
