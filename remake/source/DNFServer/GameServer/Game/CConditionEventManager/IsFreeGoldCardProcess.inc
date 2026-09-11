// ORIG T 0x0833586c：任一 info.m_flag8（+0x08 字节）非零即免费。
bool CConditionEventManager::IsFreeGoldCardProcess(
    std::vector<stConditionEventRewardInfo>& infos)
{
    for (unsigned int i = 0; i < infos.size(); ++i)
    {
        if (infos[i].m_flag8)
            return true;
    }
    return false;
}