// ORIG _ZNK12CDataManager16GetResetItemInfoEv @ 0x08693f24 (W)：return this+0x4d04
// = &m_limitItemUsage.m_resetC.m_list（stLimitItemUsageInfoEx@0x4cf8 + stResetItemInfo@+0x0c）。
std::vector<stResetItemInfo::ResetItem>* CDataManager::GetResetItemInfo() const
{
    return const_cast<std::vector<stResetItemInfo::ResetItem>*>(
        &m_limitItemUsage.m_resetC.m_list);
}