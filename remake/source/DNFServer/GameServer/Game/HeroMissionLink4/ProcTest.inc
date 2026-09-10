void WongWork::CCompoundAvatar::ProcTest(ENUM_CHARACTERJOB job,
                                         ENUM_EQUIPMENTTYPE equipType,
                                         int addRate, int bonusFlag,
                                         std::map<int, int>& result)
{
    tagCompoundAvatar* info = m_tables[job].getCompoundAvatarInfo(1);

    for (int loop = 0; loop < 10000; ++loop)
    {
        int itemIdx = 0;
        // 命中权重 = m_a[equipType] + addRate * m_e[equipType]
        int rate = info->m_a[equipType] + addRate * info->m_e[equipType];
        if (bonusFlag == 1)
        {
            rate += *(int*)((char*)G_CDataManager() +
                            0x4a54);   // ORIG 直访（字段未建模）
        }

        unsigned long range = 10000;
        if ((unsigned)m_rand->randInt(range) < (unsigned)rate)
        {
            itemIdx = getItemIndex(info->m_optsA[equipType],
                                   get_rand_int(info->m_b[equipType]));
        }
        if (itemIdx == 0)
        {
            itemIdx = getItemIndex(info->m_optsB[equipType],
                                   get_rand_int(info->m_c[equipType]));
        }

        std::map<int, int>::iterator rit = result.find(itemIdx);
        if (rit == result.end())
        {
            result.insert(std::make_pair(itemIdx, 1));
        }
        else
        {
            ++rit->second;
        }
    }
