void CDataManager::get_useJobFatigueMatchGiftMsg(int job, std::string& msg1,
                                                std::string& msg2)
{
    std::map<int, useCharacterJobFatigue::fatigueMatchGift>::iterator it =
        m_useJobFatigue.m_giftMap.find(job);
    if (it != m_useJobFatigue.m_giftMap.end())
    {
        msg1 = it->second.m_msg1;
        msg2 = it->second.m_msg2;
    }
