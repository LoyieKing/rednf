void CEventScriptMng::process_add_quest_count_db(CUser* user, int questId)
{
    if (user == 0)
        return;
    CAddItemCountDB* entity =
        static_cast<CAddItemCountDB*>(get_event_entity(0x193));
    if (entity != 0)
    {
        if (entity->is_event_quest(questId))
            user->IncCurCharacEventAddItemCount(1);
    }
