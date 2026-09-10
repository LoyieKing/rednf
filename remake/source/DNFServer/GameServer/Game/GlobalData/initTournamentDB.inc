bool GlobalData::initTournamentDB(const char* fileName)
{
    typedef std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > GroupNameMap;
    if (G_GameWorld()->GetChannelType() != 7 &&
        !G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return true;
    }

    CDBEnviroment dbEnv;
    dbEnv.set_file_name(fileName);
    if (!dbEnv.load())
    {
        return false;
    }
    if (!s_db_connector)
    {
        return false;
    }

    GroupNameMap::iterator it =
        G_CEnvironment()->m_serverGroupMap.begin();
    while (it != G_CEnvironment()->m_serverGroupMap.end())
    {
        ENUM_SERVER_GROUP group = it->first;
        DBGroupMap groupMap;
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)2, group));
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)3, group));
        DBInfoMap& info = G_CEnvironment()->m_dbConnInfoByGroup[group];
        if (!s_db_connector->GetConnInfo(groupMap, info))
        {
            return false;
        }
        if (!s_db_connector->ConnectWithServerGroup(DBInfoMap(info)))
        {
            return false;
        }
        ++it;
    }
    // ORIG 0x829e040：cMyTrace 第一参即 __PRETTY_FUNCTION__
    // （rodata _ZZN10GlobalData16initTournamentDBEPKcE19__PRETTY_FUNCTION__ @08c1b100）。
    // [G7-sub] 结论（N93/N82 封顶）：ORIG 220 / 我方 220、帧 0x480==0x480
    //   （ORIG 0x0829dd87 `sub $0x480,%esp`）；p167 仅 C1↔C2 / D1↔D2 别名
    //   （map/pair/cMyTrace）。残差是**块级具名槽分配方向相反**（P112/N93）：
    //   dbEnv 副本 ORIG -0x474 / 我方 -0x478；groupMap 的 make_pair 临时
    //   ORIG 0x0829de7a 段 -0x54/-0x38 vs 我方 -0x44/-0x28；ORIG 另有
    //   -0x1c(×2) 我方无。声明序/作用域旋钮已在 parse 族实测回滚，不再试。
    cMyTrace t(__PRETTY_FUNCTION__, 0x20aa, 0);
    t("This is Tournament PvP channel");
    s_db_mgr->activateGlobalDB();
    return true;
