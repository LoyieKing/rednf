bool CDungeonList::load_list()
{
    char* path = 0;
    STDungeonScript script;
    CDungeon* dungeon = 0;
    // [b28d-solo-CListTables] ORIG 0x0834d39e：迭代器先 C1Ev 默认构造（-0x3c）
    // 再 `it = begin()`（P50 复活）；尾部后置 it++（ppEi，sret -0x24，P28）。
    std::map<int, std::string>::iterator it;
    it = g_stDungeonScriptFileList.begin();
    while (it != g_stDungeonScriptFileList.end())
    {
        path = (char*)it->second.c_str();
        if (path == 0)
        {
            break;
        }
        if (!ImportDungeonScript(&script, path))
        {
            printf("Dungeon Script Error: %s\n", path);
            return 0;
        }
        if (script.m_dailySchedule.size() != 0)
        {
            // ORIG 0x0834d448：lea script+0x318 直传 AddDailSchedule
            // （vector<_DailySchedule> const&，ORIG 0x8125c3c），无逐项转换。
            GlobalData::s_DailyScheduleManager->AddDailSchedule(
                script.m_dailySchedule);
        }
        if (script.m_isTowerOfDespair)
        {
            dungeon = new CDungeon_TowerOfDespair();
        }
        else
        {
            dungeon = new CDungeon();
        }
        dungeon->set_dungeon(script);
        insert_dungeon(dungeon);
        G_CGameManager()->GetQuickPartySystemManager()
            ->init_quick_party_pool(dungeon);
        if (size() > 1000)
        {
            // [b28d-solo-CListTables P227] ORIG 0x0834d532：size() 结果先入 %ebx
            // 再跨 cMyTrace C1 保活（`mov %eax,%ebx` 在 ctor 前）⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x8ec, 5)(
                "Best Clear Time Save Dungeon Index Over Error: %d\n",
                size());
            return 0;
        }
        it++;
    }
    return 1;
