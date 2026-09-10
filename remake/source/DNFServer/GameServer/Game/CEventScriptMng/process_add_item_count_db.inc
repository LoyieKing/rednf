void EventClassify::CEventScriptMng::process_add_item_count_db(CUser* user,
                                                               int itemId,
                                                               int count)
{
    if (user == 0)
        return;
    CAddItemCountDB* entity =
        static_cast<CAddItemCountDB*>(get_event_entity(0x193));
    if (entity != 0)
    {
        if (entity->is_event_item(itemId))
            user->IncCurCharacEventAddItemCount((unsigned short)count);
    }
