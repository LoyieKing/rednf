// ORIG W 0x0864f8e0：mov 0x854(%eax),%eax 直读 +0x854（outPremiumCount）。
int WongWork::CUserPremium::GetAdvantageCoin() const
{
    return m_outPremiumCount;   // +0x854（ORIG mov 0x854(%eax) 直读；m_pad 字段化后此越界读改具名成员）
}