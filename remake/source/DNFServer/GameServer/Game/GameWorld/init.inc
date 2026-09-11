// [b28d-solo-GameWorld] _ZN9GameWorld4initEv verdict=AE 275/275（b28d-solo 本轮）
// 修法：P0/P183 —— `CEnvironment* env = G_CEnvironment();` 具名指针缓存删除，改
// 表达式直传 `G_CEnvironment()->get_gc_no_hardcode(...)`（ORIG 0x086c42aa 结果直作
// receiver、无 env 槽）；删后 gcNo 槽由 -0x20 回到 ORIG -0x1c，多出的 1 call+1 store
// 归零。p167 CLEAR 40=40。
bool GameWorld::init()
{
    // ORIG +0x00c：GetInstanceSyncScript 结果直转实参，无命名局部
    if (sync_script::GetInstanceSyncScript()->truncate_town_info_table() != 1)
    {
        puts("Fail truncate_town_info_tablef");
        return false;
    }

    m_uniqueId = 0;
    initTownScript((char*)"Town/", (char*)"Town.lst");

    TownScript tempTown;
    const char* fileName = NULL;

    int townCount = (int)g_townScriptFileList.size();
    MAX_VILLAGE_NUM = townCount + 1;
    // ORIG +0x09a~+0x13d：m_pVill/m_iVillCount 两个 store 都在 Village 构造循环之后
    m_pVill = new Village[MAX_VILLAGE_NUM];
    m_iVillCount = townCount;

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int index = 0;
        int gcNo = GlobalData::s_statistic_proxy_mgr->GetStartIndex();
        while (index < GlobalData::s_statistic_proxy_mgr->GetEndIndex())
        {
            // ORIG 0x086c42aa：G_CEnvironment() 结果直传 receiver（无具名 env 槽，
            // P0/P183）；具名指针缓存多 1 槽 + 1 条 store/load 并使 gcNo 槽浅移 4。
            InsertChannel(G_CEnvironment()->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
                          (ENUM_SERVER_GROUP)gcNo);
            gcNo = GlobalData::s_statistic_proxy_mgr->GetNextIndex(index);
        }
    }
    else
    {
        // ORIG +0x1cc~+0x1d7：mov 0x198(%eax) 字段直读 m_serverEnvir.m_gcNo（非 get_gc_no 调用）
        InsertChannel(G_CEnvironment()->m_serverEnvir.m_gcNo, E_SERVER_GROUP_NONE);
    }

    std::map<int, std::string>::iterator it;
    for (it = g_townScriptFileList.begin(); it != g_townScriptFileList.end(); it++)
    {
        fileName = it->second.c_str();
        if (fileName == NULL)
        {
            break;
        }
        if (!importTownScript(&tempTown, fileName))
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x3d4,
                                  "importTownScript() fail - filename(%s)", fileName);
            return false;
        }
        if (!sync_script::GetInstanceSyncScript()->insert_town_info_to_db(tempTown))
        {
            printf("insert_town_category Error: %s\n", fileName);
            return false;
        }
        if (tempTown.m_field0 > m_iVillCount)   // ORIG +0x2d6 cmp vill,town; jle ok
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x3e3,
                                  "tempTown.townIndex_(%d) > m_iVillCount(%d) - file(%s)",
                                  tempTown.m_field0, m_iVillCount, fileName);
            return false;
        }
        if (!m_pVill[tempTown.m_field0].set_village(tempTown))
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x3e9,
                                  "m_pVill[tempTown.townIndex_].set_village() fail - file(%s)",
                                  fileName);
            return false;
        }
    }

    G_CDataManager()->GetPvPChannelGrade();
    return true;
}