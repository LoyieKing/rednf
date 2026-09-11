// ORIG 0x810c1ea：礼物疲劳事件验证通过后发送奖励。
void CEventScriptMng::send_gift_fatigue_item(
    CUser* user, unsigned int fatigue)
{
    // ORIG 形：三段独立守卫（P133；ORIG 每段 jne/je 直跳统一尾部出口，
    // 无 bool 汇聚）；verifyApplyUser bool 直测（test %al）；尾部无返回值。
    if (!is_eventing(0x65))
        return;
    CUsedFatigueGiveItem* entity =
        static_cast<CUsedFatigueGiveItem*>(get_event_entity(0x65));
    if (entity == 0)
        return;
    if (entity->verifyApplyUser(user))
    {
        user->IncCurCharacUsedGiftFatigueQuantity(fatigue);
        entity->sendEventItemFromFatigue(user, fatigue);
    }
}