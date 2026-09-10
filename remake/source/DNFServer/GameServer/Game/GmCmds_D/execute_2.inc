void CLinkSystem::execute()
{
    std::string msg("gm Link Charac : ");                    // local_14（构造后未使用，照抄）
    trim(m_parser->m_args);
    const char flag =
        static_cast<char>(atoi(m_parser->m_args.c_str()));   // movsbl 截断
    // b28d-17/48：ORIG 0x084abdd0 为 switch 决策树（movsbl;test;je;cmp $1;je）
    // 且不缓存 ServerParameterScript&（p167：G_CDataManager 调用 3 次，每分支重取）。
    switch (flag)
    {
    case 0:
        G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
        G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
        break;
    case 1:
        if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) != true)
        {
            G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, true);
            G_GameWorld()->send_all_dungeon_inout_message(0x2aff, true);
        }
        break;
    default:
        break;
    }
