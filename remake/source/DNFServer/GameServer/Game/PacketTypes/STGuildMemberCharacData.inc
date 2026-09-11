// ORIG 0x822b802：+0=0xff、+1=0xff、+2=0、memset(this+3,0,0x1e)
STGuildMemberCharacData::STGuildMemberCharacData()
{
    m_0 = 0xff;
    m_1 = 0xff;
    m_2 = 0;
    memset(m_pad3, 0, 0x1e);
}