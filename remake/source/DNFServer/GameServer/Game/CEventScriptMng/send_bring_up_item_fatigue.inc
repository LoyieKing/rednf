// ORIG 0x810c33a：养成事件开启时发送疲劳奖励。
bool CEventScriptMng::send_bring_up_item_fatigue(
    CUser* user, int fatigue)
{
    if (!is_eventing(0x19b))
        return false;

    CObjectBringUp* entity =
        static_cast<CObjectBringUp*>(get_event_entity(0x19b));
    if (entity == 0)
        return false;

    entity->sendEventItemFromFatigue(user, (unsigned int)fatigue);
    return true;
}