// ORIG 0x810c2d4：透传商城消费奖励邮件结果。
bool CEventScriptMng::send_purchase_bonus_mail(
    CUser* user, unsigned int goodsId)
{
    if (!is_eventing(0x19a))
        return false;

    CPurchaseCashItemBonus* entity =
        static_cast<CPurchaseCashItemBonus*>(get_event_entity(0x19a));
    if (entity == 0)
        return false;

    return entity->sendPurchaseBonusReward(user, (int)goodsId);
}