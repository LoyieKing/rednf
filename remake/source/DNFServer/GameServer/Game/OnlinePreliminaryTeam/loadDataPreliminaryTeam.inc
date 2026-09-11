// ORIG T 0x085882b8：data[0]→teamId(+0x4)；strncpy(name,data+4,0xf)；
// data[0x14]→rating(+0x8)、data[0x18]→win、data[0x1c]→lose；返回 1。
int COnlinePreliminaryTeam::loadDataPreliminaryTeam(char* data)
{
    // 签名保持 char*（ORIG mangled …EPc）；data 即 TEAM 序列化缓冲布局。
    SIG_ONLINE_PRELIMINARY_TEAM* d =
        reinterpret_cast<SIG_ONLINE_PRELIMINARY_TEAM*>(data);
    m_field04 = d->m_teamId;
    strncpy(m_field14, d->m_teamName, 0xf);
    m_rating08 = d->m_rating;
    m_winCount = d->m_winCount;
    m_loseCount = d->m_loseCount;
    return 1;
}