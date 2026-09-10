void CHackAnalyzer::_addHackInfo(ENUM_REPORT_4_HACK_FLAG flag,
                                 ENUM_HACKTYPE type, int value)
{
    // ORIG 0x080f7fd2（_addHackInfo.md）：取 acc_id / curCharacNo 作记录前两域，
    // {uid, hackNo, type(u16), value(int)} 落栈后 push_back 进 m_hackInfoList，
    // 随后 m_flag04 |= flag。
    stHackInfo_t info;
    info.m_uid = ((CUser*)m_baseHackType)->get_acc_id();
    info.m_characNo = (unsigned int)((CUser*)m_baseHackType)->getCurCharacNo();
    info.m_hackType = (unsigned short)(unsigned int)type;
    info.m_value = value;
    m_hackInfoList.push_back(info);

    m_flag04 = (unsigned char)(m_flag04 | flag);
