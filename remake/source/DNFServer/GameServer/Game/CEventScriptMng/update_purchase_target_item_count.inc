void CEventScriptMng::update_purchase_target_item_count(
    CUser* user, unsigned int goodsId, stCeraShopIPGRequest_t* request)
{
    if (!is_eventing(0x19a))
        return;

    CPurchaseCashItemBonus* entity =
        static_cast<CPurchaseCashItemBonus*>(get_event_entity(0x19a));
    if (entity != 0)
        entity->updateTargetGoodsCount(user, goodsId, request);
    return;
