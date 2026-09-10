bool UserQuest::hasEpicQuest()
{
    // b28d-40: N4 封顶——`lea 0x4(%edx)` vs 4.4 `add $0x4,%eax`（add-vs-lea），
    // 语义闭合。ORIG 0x086ad266。
    if (m_user == 0)
    {
        return false;
    }

    std::list<int> questList;
    stSelectQuestParam selectParam(m_user);

    WongWork::CQuestClear cleared(m_user->getCurCharacQuestR()->m_cleared);

    G_CDataManager()->m_questList->allowable_questlist_as_npc(
        questList,
        *G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_NONE),
        selectParam,
        cleared, false);

    if (!questList.empty())
    {
        char deadLocal = 0;   // ORIG 0x086ad334 死写（P171 照抄）
        (void)deadLocal;
        // ORIG 0x086ad3f4 增量段调 `_ZNSt14_List_iteratorIiEppEv`（前缀，无 sret）⇒ 写 ++it。
        for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
        {
            // P167：ORIG 0x086ad399 调 const 版 `_ZNKSt6vectorI25ENUM_QUEST_EXCEPTION_TYPESaIS0_EEixEj`
            // ⇒ 原码 quest 为 const Quest*。
            const Quest* quest = G_CDataManager()->find_quest(*it);
            if (quest == 0)
            {
                goto L_quest0;   // P223 多岛：L1 岛（nop; jmp）
            }
            {
                char bException = 0;
                for (unsigned int j = 0; j < quest->m_exceptionList.size(); ++j)
                {
                    if (quest->m_exceptionList[j] == ENUM_QUEST_EXCEPTION_TYPE_0)
                    {
                        bException = 1;
                        break;
                    }
                }
                if (bException != 0)
                {
                    goto L_bexc;   // P223 多岛：L2 岛（nop）
                }
                if (selectParam.m_level < quest->m_levelMin + 0xc)
                {
                    return true;
                }
            }
        L_quest0: ;
        L_bexc: ;
        }
    }
    return false;
