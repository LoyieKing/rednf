bool COnlinePreliminaryMember::loadData(CUser* /*pUser*/, char* pData)
{
    // 签名保持 char*（ORIG mangled …EP5CUserPc）；pData 即 MEMBER 缓冲布局。
    SIG_ONLINE_PRELIMINARY_MEMBER* d =
        reinterpret_cast<SIG_ONLINE_PRELIMINARY_MEMBER*>(pData);
    m_rating = d->m_rating;   // ORIG movl 4B 直拷（0x08585fad..0x08585fd0）
    m_winCount = d->m_winCount;
    m_loseCount = d->m_loseCount;
    return true;
