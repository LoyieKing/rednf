// ==== Dispatcher_AvatarOptionChange::process (ORIG 0x081e0774) ====
// 影子背包预演删除消耗品成功后，才落到真实背包；改写目标 avatar 装备的 option 字节并回包。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e0774 orig/ours=183/184 ndiff-blocks=27
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x20(%ebp) | mov    -0x20(%ebp),%edx | mov    %eax,0x4(%edx)
int Dispatcher_AvatarOptionChange::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    // [G4-312 2026-09-11 · P0/P183/P339] ORIG 槽序 param(-0x20) 先、msg(-0x1c) 后 ⇒
    //   按此声明；check_error 结果**直写 param+4 后重读**（0x081e0a40 段
    //   `mov -0x20(%ebp),%edx; mov %eax,0x4(%edx)` + 每处 `mov -0x20(%ebp),%eax;
    //   mov 0x4(%eax),%eax`），无 err 具名槽。
    stChangeAvatarOption& param = *reinterpret_cast<stChangeAvatarOption*>(&paramBase);
    MSG_CHANGE_AVATAR_OPTION& m = *reinterpret_cast<MSG_CHANGE_AVATAR_OPTION*>(&msg);

    param.m_result = check_error(user, msg);
    if (param.m_result > 0)
        return 0;
    if (param.m_result < 0)
        return LineFunc(0x53ab,
                        __PRETTY_FUNCTION__,
                        param.m_result, 0);

    CInventory* invW = user->getCurCharacInvenW();

    // ORIG 在栈上构造影子背包（CInventory + InventoryMemory 0xc140）做删除预演。
    CInventory shadowInv;
    InventoryMemory shadowMem;
    shadowInv.SetInventoryMemory(&shadowMem);
    // ORIG 0x081e083d：setCopy 参数经 getCurCharacInvenRefR 取得（与 R 同址，别名视图）。
    shadowInv.setCopy(*reinterpret_cast<const CInventory*>(user->getCurCharacInvenRefR()));

    if (shadowInv.delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1,
                              (eItemDelReason)0x2f, false) != 1)
    {
        param.m_result = 0x16;
        return 0;
    }
    Inven_Item* shadowRef = shadowInv.GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);
    if (shadowRef == 0)
    {
        param.m_result = 0x11;
        return 0;
    }

    invW->delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1, (eItemDelReason)0x2f, true);
    Inven_Item* avatarRef = invW->GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);

    // ORIG 0x081e0986：_ZN15cUserHistoryLog21UseAvatarOptionChangeEiiii（实现见
    // cUserHistoryLog.cpp:1365，CUser.h:2077 已声明）。
    user->m_historyLog.UseAvatarOptionChange(avatarRef->m_addInfo2,
        avatarRef->m_addInfo, avatarRef->m_endurance, (int)(signed char)m.m_option);

    avatarRef->m_endurance = (unsigned short)(signed char)m.m_option;
    invW->SendAvatarEvent(0x2c3, avatarRef->m_addInfo2,
                          (int)(signed char)m.m_option, 0, 0);

    param.m_srcSlot = m.m_consumeSlot;
    param.m_dstSlot = m.m_avatarSlot;
    param.m_optionValue = m.m_option;
    return 0;
}