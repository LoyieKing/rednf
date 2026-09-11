int ServerParameterScript::GetLottePoint(int idx)
{
    // b28c-L8 [AE 6→0]：`it->first`/`it->second` → `(*it).first`/`(*it).second`。
    // p167 差集一击：ORIG 0x085bec76 三处解引用均为 __normal_iterator::operator*()
    // const（MISSING deEv ×3 / EXTRA ptEv ×3），非 operator->。同法连带
    // GetPriceAverage(0x85beaf6)/GetAdmissionIncrease(0x85bebb6)/
    // GetCleanChattingCount(0x86d17ec) 共 4 件归零。
    if (m_bloodReward794.m_vec18.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vec18.begin();
    std::vector<std::pair<int, int> >::iterator e =
        m_bloodReward794.m_vec18.end();
    for (; it != e; ++it)
    {
        if ((*it).first > idx)
        {
            return (*it).second;
        }
    }
    --it;
    return (*it).second;
}