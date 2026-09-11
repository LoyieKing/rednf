// ORIG T 0x08692000，报告 GiveItemsReturnUser.md。
// b28d-solo-W7_UserOnlyC 判词：addInfo 具名槽（去 const 折叠）+ group/msgLen/
// characNo/sender 内联实参（P0/P52）+ P285 空 else 岛 ⇒ 81=81，TU 层 AE。
void CUser::GiveItemsReturnUser()
{
    // ORIG 读 G_CDataManager()+0xb548 = m_returnUserReward.m_field0。
    const int itemIdx = G_CDataManager()->m_returnUserReward.m_field0;
    // ORIG 0x0869201a `movl $0x1,-0x24(%ebp)`：addInfo 为具名局部（P0 槽往返）。
    // 非 const：4.4 对 const 局部会常量传播折成立即数（ORIG 从槽读取）。
    int addInfo = 1;
    Inven_Item item;                                 // ctor 清零
    CItem* base = G_CDataManager()->find_item(itemIdx);
    if (base != 0)
    {
        item.m_addInfo = itemIdx;                    // +0x02 item idx
        base->make_item(item);                       // vtable+8 虚调用
        item.set_add_info(addInfo);

        const char* msgBody =
            g_scriptStringManager_.findString(4, "return_user_msg_1", 0);
        // P0/P52：ORIG 的 group/msgLen/characNo/sender 均无具名槽（esi/edi/ebx/eax
        // 跨调用保活）⇒ 保持内联实参；仅 msgBody 具名（-0x1c 槽）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0),
            item, 100000u, static_cast<unsigned int>(getCurCharacNo()),
            msgBody, static_cast<int>(strlen(msgBody)), 7u, GetServerGroup(),
            false, false);
    }
    else
    {
        return;   // P285：空 else 保留 ORIG 体尾 jmp+nop 岛（4.4 纯正向 if 会 jump-thread）
    }
}