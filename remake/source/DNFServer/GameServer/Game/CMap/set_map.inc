int CMap::set_map(STMapScript script)
{
    m_4 = script.m_index;
    m_8 = script.m_c;
    m_80 = script.m_championRate;
    m_entranceVec = script.m_recoveryObjects;
    m_pathObjectList = script.m_list4;
    m_bloodSystem = script.m_vecBloodSystem;
    m_bloodPhaseTime = script.m_vecBloodPhaseTime;
    m_b4 = script.m_maxMonsterDropException;
    std::memcpy(m_pathArea1, script.m_pathgatePos, sizeof(m_pathArea1));
    std::memcpy(m_pathArea2, script.m_pathgateRange, sizeof(m_pathArea2));

    STAssignMonster monster;
    std::vector<STAssignMonster>::iterator it;
    for (it = script.m_vecMonster.begin(); it != script.m_vecMonster.end(); it++)
    {
        monster = STAssignMonster(*it);
        if (m_8 == 2 && monster.m_24 == 3)
        {
            monster.m_24 = 1;
            monster.m_54 = 1;
        }
        if (monster.m_24 == 3)
            ++m_bossCount;
        insert_monster(monster, -1);
    }

    for (it = script.m_vecConditionMonster.begin(); it != script.m_vecConditionMonster.end(); it++)
    {
        monster = STAssignMonster(*it);
        insert_condition_monster(monster);
    }

    int area = 0;
    if (!script.m_listSpecialPassive.empty())
    {
        for (std::list<STAssignSpecialPassiveObj>::iterator obj = script.m_listSpecialPassive.begin();
             obj != script.m_listSpecialPassive.end(); ++area, obj++)
        {
            // ORIG 0834e5b7 只调一次 _List_iterator::operator*（const）并把引用存 -0x14，
            // 之后全部经该引用访问（0834e5cb `mov -0x14; lea 0x10(%eax)`）。
            STAssignSpecialPassiveObj& spObj = *obj;
            for (unsigned int i = 0; i < spObj.m_vec10.size(); ++i)
            {
                SPOBJ_ACTION_DATAS* action = &spObj.m_vec10[i];
                if (action->m_type == 0)
                {
                    if (action->m_10 != 0 &&
                        get_rand_int(100) <= (action->m_c * 100) / action->m_10 &&
                        action->m_8 < 0xc9 && action->m_8 > 0)
                    {
                        monster.Clear();
                        monster.m_0 = action->m_4;
                        monster.m_8 = action->m_8;
                        monster.m_4 = 0;
                        insert_monster(monster, area);
                    }
                }
                else if (action->m_type == 2)
                {
                    insert_item(action->m_4, action->m_8, action->m_c, spObj.m_1c);
                }
            }
        }
    }

    m_rivalData1 = script.m_vecMonsterSpawnPos;
    m_rivalData2 = script.m_vecEventMonsterPos;
    m_78 = script.m_apcRandomPoint;
    m_aiCharacters = script.m_vecAICharacter;
    m_assignedNpcCount = static_cast<int>(script.m_vecNpc.size());
    if (!script.m_listPassiveObject.empty())
    {
        for (std::list<STAssignPassiveObject>::iterator obj = script.m_listPassiveObject.begin();
             obj != script.m_listPassiveObject.end(); ++obj)
        {
            // ORIG 0834e880 用 _List_iterator::operator*（0838b542），非 operator->。
            m_passiveObjectList.push_back((*obj).m_0);
        }
    }
    m_ec = script.m_maxMonsterCountException;
    m_f0 = script.m_328;
    m_timeLine = script.m_timeLine;
    m_110 = script.m_178;
    // ORIG 0834e917..0834e931：G_CDataManager() 只调一次（eax→+0x62e8=&m_actionMap），
    // checkAndSetActionDefine 为 static 成员（P245/P281），参数即 m_timeLine/&m_actionMap；
    // 返回值 0834e936 `mov %al,-0x15(%ebp)` 单字节落槽后未使用（P0 具名局部）。
    char checkRet = advancealtar::StageTimeLineParameter::checkAndSetActionDefine(
        m_timeLine, G_CDataManager()->m_stageTimeLine.m_actionMap);
    (void)checkRet;
    m_tournamentMap = script.m_tournamentMonsters;
    m_12c = script.m_fixChampion;
    return 1;
