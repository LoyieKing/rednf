// ============================================================================
// CDataManager::GetAutoMarketContitionsControl —— ORIG W 0x084e92fc
// （objdump：mov 0x8cc4(%eax),%eax —— 即成员 m_autoMarket）
// ============================================================================
CAutoMarketConditionsControl* CDataManager::GetAutoMarketContitionsControl()
{
    return m_autoMarket;
}