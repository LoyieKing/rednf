// ORIG 0x810c270：商城奖励活动开启时更新目标商品计数。
// [B24D判词 N] 语义正确：is_eventing(0x19a)→get_event_entity(0x19a)→
//   updateTargetGoodsCount(user,goodsId,request)。diff=ORIG 句尾 `jmp;nop;jmp`（4.1
//   合并跳转+对齐）我方单 jmp，32/30 纯 lowering/对齐表示差异，语义等价。
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
}