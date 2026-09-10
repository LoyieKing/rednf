void CEventScriptMng::process_update_ingame_history(CUser* user)
{
    if (user == 0)
        return;

    std::map<int, bool>::const_iterator it =
        static_cast<const std::map<int, bool>*>(
            user->get_ingame_event_history_update())->begin();

    while (it != static_cast<const std::map<int, bool>*>(
                      user->get_ingame_event_history_update())->end())
    {

        if (it->second)
        {
            std::map<int, std::pair<int, int> >::const_iterator hit =
                static_cast<const std::map<int, std::pair<int, int> >*>(
                    user->get_ingame_event_history())
                    ->find(it->first);
            if (hit !=
                static_cast<const std::map<int, std::pair<int, int> >*>(
                    user->get_ingame_event_history())->end())
            {
                DB_UpdateInGameEventHistory::makeRequest(  // UNLINKED_CALLEE
                    user->get_acc_id(), it->first,
                    hit->second.first, hit->second.second);
            }
            ++it;
        }
    }
