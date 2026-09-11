// ORIG W 0x0863bd5c：*(int*)(this+0x848) + *(int*)(this+0x84c)
int CUserPremium::GetTotalPremiumCount() const
{
    return m_normalCount
         + m_restrictedCount;
}