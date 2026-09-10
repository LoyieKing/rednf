bool CParty::checkUnLimitUsedCoinCondition(CDungeon const* dungeon)
{
    if (dungeon == 0)
    {
        int dungeonIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                             ->get_dungeon_index();
        dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        if (dungeon == 0)
        {
            return false;
        }
    }
    // ORIG 无 ev 判空（虚调用直发，+0x024..0x031）；尾条件为有符号 >0
    // （+0x03b.. test/jle），忠实转写。
    // ORIG O@0x76 `test%al; jle`：dim 按有符号 char 判 >0（值域≥0x80 分叉）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x5e)->IsEventing(0)
        && static_cast<signed char>(
               const_cast<CDungeon*>(dungeon)->get_dimension_possible()) > 0)
    {
        return true;
    }
    return false;
