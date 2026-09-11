// ==== Dispatcher_PurifyItem::process (ORIG 0x08218cb6 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08218cb6 orig/ours=488/492 ndiff-blocks=61
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN21Dispatcher_PurifyItem7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x10(%ebp),%eax | mov    %eax,-0x14
int Dispatcher_PurifyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;  // ORIG 不读 param（local_18 仅赋值未使用）

    if (user->get_state() != 3)
    {
        return LineFunc(0xcbcb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    if (G_GameWorld()->GetChannelType() == 7)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0x11);
        return 0;
    }

    // [DispLaneB 2026-09-11] P0：ORIG 直到 0x08218d3d 才发 `mov 0x10(%ebp),%eax;
    //   mov %eax,-0x14(%ebp)`（msg 引用缓存），即声明点在 0x08218d01 的
    //   GetChannelType 判定**之后** ⇒ 声明点下移，槽序/求值序与 ORIG 对齐。
    MSG_PURIFY_ITEM& m = reinterpret_cast<MSG_PURIFY_ITEM&>(msg);

    if (user->CheckItemLock(1, m.m_0d))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0xd5);
        return 0;
    }

    // [G4-318 2026-09-11 · P16/P59 语义轴] ORIG 两条 Inven_Item 构造（0x08218d93/0x08218da1）
    //   在 try 起点之前；try 区间 [0x08218def,0x082191c4]，0x082191c5 `jmp 0x08219222`
    //   直接跳过 handler 落到 update_item（其后各段无 catch 目标）⇒ 原码 try 只到 apply_Item。
    //   旧形把 update_item/PacketGuard 发送段也包进 try ⇒ 该段异常会先走 catch 类型匹配（真语义差）。
    Inven_Item targetItem;
    Inven_Item materialItem;

    try
    {
        const int purifyType =
            GlobalData::s_itemAmplifier_->getPurifyMaterialType(m.m_15);
        if (purifyType == -1)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        if (GlobalData::s_itemAmplifier_->getPurifyMaterialItemIdx(
                purifyType) != (int)m.m_15)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        targetItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_0d);
        if (targetItem.m_addInfo != (int)m.m_0f)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        // 已鉴定（bit7==0）的强化选项不允许净化
        if (targetItem.m_amp.isIdentified())
        {
            throw static_cast<ENUM_ERROR>(0x17);
        }

        materialItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);
        // [G1-Dispatchers] ORIG 两处独立调用 getPurifyMaterialItemCount：
        //   0x08219082（数量校验）与 0x082190d6（delete_item 实参位，结果入 %esi）
        // ⇒ 不设 needCount 具名局部（P48/P0；p167 ORIG-only ×1）。
        if (materialItem.m_addInfo != (int)m.m_15 ||
            materialItem.m_addInfo2 <
                GlobalData::s_itemAmplifier_->getPurifyMaterialItemCount(purifyType))
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        // [G1-Dispatchers] P48：ORIG delete_item 与 update_item 各自重取
        // `getCurCharacInvenW()`（p167 ORIG-only ×1，我方原用 invenW 缓存）⇒ 不设具名 invenW。
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, m.m_13,
                GlobalData::s_itemAmplifier_->getPurifyMaterialItemCount(purifyType),
                (eItemDelReason)3, true))
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        // [G1-Dispatchers] 还原 ORIG 0x082194b6 的**独立成员调用**
        // `_ZN21Dispatcher_PurifyItem17processPurifyTypeEiR10Inven_Item`（T）：
        // Dispatchers.h:388 已声明该成员、Game/Dispatchers/processPurifyType.inc 已有真实体
        // ⇒ 此处不得再内联展开（p167：ORIG-only processPurifyType x1，我方 EXTRA
        //   stAmplifyOption_t::isIdentified/reset 各 x1 即内联残留）。
        if (!processPurifyType(purifyType, targetItem))
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        user->m_historyLog.apply_Item(materialItem.m_addInfo,
                                      targetItem.m_addInfo,
                                      (eApplyItemReason)2);
    }
    catch (ENUM_ERROR& e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, (unsigned char)e);
        return 0;
    }

    // ORIG 0x08219222 起：不在 try 内（异常直接外传；PacketGuard LP @0x082194a5 直接 resume）
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)1, m.m_0d, targetItem);

    // 刷新材料槽（删除后剩余数量入包）
    materialItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);

    PacketGuard guard;
    guard.put_header(1, 0xcf);
    guard.put_byte(1);
    guard.put_short(m.m_13);
    guard.put_int(materialItem.m_addInfo2);
    guard.put_short(m.m_0d);
    guard.put_byte((unsigned char)targetItem.m_amp.getAbilityType());
    guard.put_short(targetItem.m_amp.getAbilityValue());
    guard.finalize(true);
    user->Send(guard);
    return 0;
}