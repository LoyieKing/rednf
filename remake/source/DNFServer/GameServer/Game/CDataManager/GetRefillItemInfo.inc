// ORIG _ZNK12CDataManager17GetRefillItemInfoEv @ 0x08693f32 (W)：return this+0x4d10
// = &m_limitItemUsage.m_refill18.m_list（stLimitItemUsageInfoEx@0x4cf8 + stRefillItemInfo@+0x18）。
std::vector<stRefillItemInfo::RefillItem>* CDataManager::GetRefillItemInfo() const
{
    return const_cast<std::vector<stRefillItemInfo::RefillItem>*>(
        &m_limitItemUsage.m_refill18.m_list);
}