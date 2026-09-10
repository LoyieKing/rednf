void CUser::setLoadHackGold(bool flag)
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    m_mcap.m_loadingHackGold = flag;
