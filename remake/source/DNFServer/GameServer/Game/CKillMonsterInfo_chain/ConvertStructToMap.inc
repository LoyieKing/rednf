bool CKillMonsterInfo::ConvertStructToMap(
    stMonsterInfo_t* pInfo, unsigned int count,
    std::map<unsigned int, unsigned char>& outMap, bool bAPC)
{
    unsigned int mobIdx;
    unsigned char diff;
    unsigned int i;
    void* pMon;
    void* pAI;
    for (i = 0; i < count; ++i)
    {
        mobIdx = pInfo[i].mobIdx;
        if (mobIdx == 0)
            break;
        if (!bAPC)
        {
            pMon = G_CDataManager()->find_monster(mobIdx);
            if (pMon == 0)
            {
                LogManager::logFormat(
                    1, "KillMonsterInfo.cpp", __PRETTY_FUNCTION__, 0x25,
                    "ConvertStrToMap ERROR : mobIdx is not valid(%u)", mobIdx);
                continue;
            }
        }
        if (bAPC)
        {
            pAI = G_CDataManager()->m_aiCharacterList->get(mobIdx);
            if (pAI == 0)
            {
                LogManager::logFormat(
                    1, "KillMonsterInfo.cpp", __PRETTY_FUNCTION__, 0x2e,
                    "ConvertStrToMap ERROR : mobIdx is not valid(%u)", mobIdx);
                continue;
            }
        }
        diff = pInfo[i].diff;
        outMap.insert(std::make_pair(mobIdx, diff));
    }
    return 1;
