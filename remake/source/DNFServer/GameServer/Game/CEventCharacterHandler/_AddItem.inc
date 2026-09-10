bool CEventCharacterHandler::_AddItem(CUser* user, unsigned long itemIdx, int count,
                                      int reason)
{
    // [b28d-solo-CEventCharacterHandler] ORIG 0x0848e8e2 逐条对应：
    // ① start/end 无槽往返（P0）⇒ 内联实参，end 值经 %ebx 跨第二个 getCurSec
    //    调用保活（P52/P226 形）；② stack 无具名局部（P183）⇒ 每次 cast 重读；
    // ③ slot<0 走共享尾 mov$0 岛（js 落尾）⇒ P222 正向包裹；
    // ④ name/accStr 无槽（%esi/%ebx 跨 ctor 保活）⇒ P227 全表达式临时链。
    if (itemIdx == 0xffffffff)
    {
        WongWork::CHandlePremium::handleSetUserPCRoom(
            user,
            GlobalData::s_systemTime_.getCurSec(),
            GlobalData::s_systemTime_.getCurSec() + 0x15180);
    }

    const CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item != 0)
    {
        if (item->is_stackable())
        {
            if (((const CStackableItem*)item)->getStackableLimit() < count)
            {
                count = ((const CStackableItem*)item)->getStackableLimit();
            }
        }
    }

    ENUM_ITEMSPACE itemSpace;
    int slot = user->AddItem((int)itemIdx, count, (eItemAddReason)6, itemSpace, reason);
    if (slot >= 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xda, 0)(
            "%s:'%s' GM Mode Add Item(%d).",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacName(), slot);
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)itemSpace, slot);
        return true;
    }
    return false;
