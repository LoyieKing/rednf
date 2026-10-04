// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e81a4 orig/ours=98/99 ndiff-blocks=21
//   帧差 sub $30 vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x30,%esp | mov    %eax,-0x14(%ebp)
int Dispatcher_GrowthWeaponEventUseMaterial::process(CUser* user, MSG_BASE& msgBase, ParamBase& paramBase)
{
    // ORIG 0x081e81a4：消耗成长武器活动材料，累加怀表计数并发 DB 更新。
    stGrowthEquipment& param = *reinterpret_cast<stGrowthEquipment*>(&paramBase);
    param.m_4 = check_error(user, msgBase);
    if (param.m_4 >= 1)
    {
        return 0;
    }
    if (param.m_4 < 0)
    {
        return LineFunc(0x647c, __PRETTY_FUNCTION__, param.m_4, 0);
    }
    // [G5·P331/P339 声明序=槽序] ORIG 0x081e81ac param→-0x14（先声明，深槽）、
    //   0x081e821a `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` msg→-0x10（err 检查**之后**
    //   才物化）；我方原为 msg 先声明（-0x14）⇒ 两槽对调、整帧错位。交换声明序 + 后移。
    MSG_GROWTH_EQUIPMENT& msg = *reinterpret_cast<MSG_GROWTH_EQUIPMENT*>(&msgBase);
    // [G5·P52/P292] ORIG 0x081e821a 段：`call getCurCharacInvenW`（结果**直传**，无 inven
    //   具名槽）→ `movl $0x1,0x14(%esp); …; mov %eax,(%esp); call delete_item` →
    //   `xor $0x1,%eax; test %al,%al; je`（bool 取反直测，无 ret 具名槽）。我方两具名槽 +3 条
    //   ⇒ 单语句链式 + 裸 `!` 形（CInventory::delete_item 返回 bool，CInventory.h:737）。
    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)msg.m_0d),
            msg.m_11, 1, (eItemDelReason)0x36, true))
    {
        param.m_4 = 0x11;
        return 0;
    }
    GrowthWeaponEvent* gwe =
        static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata());
    gwe->m_field10 = static_cast<short>(gwe->m_field10 + 1);
    DB_EventUpdateGrowthWeaponTimepiece::makeRequest(user->get_acc_id(), gwe->m_field10, 0);
    param.m_10 = gwe->m_field10;
    param.m_8 = msg.m_0d;
    param.m_c = msg.m_11;
    return 0;
}