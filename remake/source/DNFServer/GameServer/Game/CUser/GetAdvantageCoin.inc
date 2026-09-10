int WongWork::CUserPremium::GetAdvantageCoin() const
{
    return m_outPremiumCount;   // +0x854（ORIG mov 0x854(%eax) 直读；m_pad 字段化后此越界读改具名成员）
