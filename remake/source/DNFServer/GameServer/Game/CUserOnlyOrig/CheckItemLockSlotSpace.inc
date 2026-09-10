bool CUser::CheckItemLockSlotSpace(int spaceType, int spaceIdx) const
{
    // ORIG 0x086469f0（报告 CheckItemLockSlotSpace.md 全文转写）。
    // 链：getCurCharacR → GetCurCharacInventoryRef(00680c8a) → m_amp.GetLock →
    //     GetCharacExpandDataR(2)=item_lock::CItemLock → CheckItemLock(lock 编号)。
    if (getCurCharacR() == 0)
        return false;
    const Inven_Item* slot = GetCurCharacInventoryRef((unsigned short)spaceType,
                                                      (unsigned short)spaceIdx);
    if (slot == 0)
        return false;
    unsigned char lockNo = slot->m_amp.GetLock();
    if (lockNo == 0)
        return false;
    // ORIG 0x08646a62 `movzbl -0x9(%ebp),%ebx`：lockNo 于声明点直落 callee-saved
    // ebx 跨 GetCharacExpandDataR 存活（技巧22）；0x08646a79-0x08646a80
    // `mov %ebx,0x4(%esp); mov %eax,(%esp)` = expand 结果免槽直用（技巧37，具名
    // lock 指针槽 -0xc 反产双 spill）；尾 `test;je;mov$1;jmp;mov$0` = if-return 岛。
    if (((item_lock::CItemLock*)GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2))
            ->CheckItemLock(lockNo))
        return true;
    return false;
