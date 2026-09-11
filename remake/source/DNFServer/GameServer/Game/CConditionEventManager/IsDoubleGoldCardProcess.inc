// ORIG T 0x08335a3a
// _ZN22CConditionEventManager23IsDoubleGoldCardProcessERSt6vectorI26stConditionEventRewardInfoSaIS1_EE
// 任一 info 的 m_flag9（+0x09 字节）非零 → true。
bool CConditionEventManager::IsDoubleGoldCardProcess(
    std::vector<stConditionEventRewardInfo>& infos)
{
    for (unsigned int i = 0; i < infos.size(); ++i)
    {
        if (infos[i].m_flag9)
            return true;
    }
    return false;
}