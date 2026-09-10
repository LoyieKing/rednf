int UserQuest::giveup_quest(int questIdx)
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            Quest* quest = G_CDataManager()->find_quest(questIdx);
            if (quest != 0)
            {
                if (quest->m_field120 == 1 || quest->m_field120 == 2)
                {
                    if (m_trigger[i] == 0)
                    {
                        return 0x12;
                    }
                    if (m_user->get_state() == 5)
                    {
                        void* party = m_user->GetParty();
                        if (party != 0)
                        {
                            void* maze = reinterpret_cast<CBattle_Field*>(static_cast<char*>(party) + 0xb24)->getMaze();
                            if (maze != 0 &&
                                reinterpret_cast<MazeQuestView*>(maze)->m_questIdx == questIdx)
                            {
                                return 0x14;
                            }
                        }
                    }
                }
                if (quest->m_type == 8)
                {
                    if (m_user->get_charac_level() > quest->m_levelMax)
                    {
                        m_authenData.m_urgentQuest = -1;
                    }
                    if (hasEpicQuest())
                    {
                        m_authenData.m_urgentQuest = -1;
                    }
                }
            }

            m_quest[i] = 0;
            m_trigger[i] = 0;
            return 0;
        }
    }
    return 0x13;
