bool AvatarFixedHiddenOptionServer::ChangeHiddenOption(
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    // [b28d-56 判词·N → L4fEventAvatar 2026-09-11 收敛] ORIG 0x0817e8d6 230 条；
    // 本轮按 ORIG 直证复原 4 处后：prologue 已对齐（push edi/esi/ebx + sub $0x7c）、
    // traceDel/traceOk 已用链式临时形使实参先于 ctor 求值（4.1 调度）、
    // GetSkillIndex 与 Acquire 的结果改寄存器直传（无具名槽）、save.m_slot 的
    // `sig->m_avatarSlot+10` 改 unsigned 语境（ORIG 零扩展、无 movswl）。
    // 残余 N：我方 231 条 / 帧同 0x7c，仅剩**全函数槽位整体偏移 4**（ORIG ref
    // -0x20/item -0x1c/traceDel -0x40… 对 我方 -0x24/-0x20/-0x44…）——
    // ORIG 把 traceOk 第三实参的 int 临时放**对象区最深槽 -0x5c**，4.4 则把
    // 具名 int 放标量区最浅槽 -0x1c（删具名改用表达式临时实测劣化：帧 0x7c→0x8c、
    // 231→237 条）⇒ 属 4.1/4.4 栈槽分配器分区差异，无源级旋钮。语义完成。
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(1), (int)sig->m_materialSlot, 1,
                          eItemDelReason(3), true) != 1)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0xa1,
            "AVATAR HIDDEN OPTION CHANGE : delete_item() failed. [slot:%d] [charac_no:%d]",
            (int)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x13);
        return false;
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)sig->m_materialSlot);
    // [L4fEventAvatar 2026-09-11] P37：ORIG 0x0817e9b9..0x0817e9ed 在 cMyTrace ctor
    //   之前先求值 operator() 的两个实参（getCurCharacNo→%ebx、m_materialSlot→%esi）
    //   —— 这是**链式临时**形（`cMyTrace(...)(...)` 的实参先于对象构造求值），
    //   命名对象形（ctor 语句在前）在 4.4 下必然先 ctor 后求参。
    cMyTrace(__PRETTY_FUNCTION__, 0xa6, 0)(
        "AVATAR HIDDEN OPTION CHANGE : delete_item() success. [slot:%d] "
        "[charac_no:%d]",
        (int)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());

    Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(2, (int)sig->m_avatarSlot);
    if (ref == 0)
        return false;
    CItem* item = G_CDataManager()->find_item((int)ref->m_addInfo);
    if (item == 0)
        return false;
    if (!item->IsAvatarItem())
        return false;

    // [L4fEventAvatar 2026-09-11] P0/P183：ORIG 0x0817eaa3 `call GetSkillIndex; mov %eax,%edx;
    //   mov -0x20,%eax; mov %dx,0xd(%eax)` 全程寄存器直传、无槽往返 ⇒ 原码无具名
    //   skillIdx（我方具名形多 `mov %eax,-0x10; mov -0x10,%eax` 两次往返）。
    ref->m_fieldSplit.m_fielddLo = (short)GetSkillIndex((ENUM_RARITY)item->get_grade());
    sig->m_hiddenOption = (unsigned short)(short)ref->m_fieldSplit.m_fielddLo;

    // [L4fEventAvatar 2026-09-11] P0：ORIG 0x0817eb21 `movzwl -0x42(%ebp),%eax; cwtl;
    //   mov %eax,-0x5c(%ebp)` 后由 0x0817eb5a `mov -0x5c(%ebp),%eax` 读回 ⇒
    //   该 int 是具名局部（store+load 往返；且 -0x5c 是全函数最深槽 = 最后声明）。
    int hiddenOption;
    arad::SigAradAvatarHiddenOptionSave save;             // +0/…清零语义同 ORIG W ctor
    save.m_field0 = user->get_acc_id();
    save.m_characNo = (unsigned int)user->getCurCharacR()->m_characNo;
    save.m_itId = (unsigned int)ref->m_addInfo;
    // [L4fEventAvatar 2026-09-11] 类型轴：ORIG 0x0817eafb 为 `movzwl 0x2(%eax),%eax;
    //   add $0xa,%eax; mov %ax,-0x44`（**零扩展**、无 `movswl`）⇒ 原码该表达式在
    //   unsigned 语境（m_avatarSlot 声明为 signed short，`(int)` 转换会生 movswl）。
    save.m_slot = (short)((unsigned short)sig->m_avatarSlot + 10);
    save.m_hiddenOption = (short)ref->m_fieldSplit.m_fielddLo;

    hiddenOption = (int)(short)save.m_hiddenOption;
    // [L4fEventAvatar 2026-09-11] ORIG 0x0817eb14..0x0817eb51 在 ctor 前求值全部实参
    //   （getCurCharacNo→%ebx、itId→%edi、slot→%esi、hiddenOption→-0x5c）⇒ 链式临时形；
    //   ORIG 0x0817eb2c `movzwl -0x44(%ebp),%eax; movswl %ax,%esi` 是**从 save.m_slot
    //   读回**再符号扩展（不是重算 sig->m_avatarSlot+10）。
    cMyTrace(__PRETTY_FUNCTION__, 0xc2, 0)(
        "AVATAR HIDDEN OPTION CHANGE : success [slot:%d] [item_id:%d] "
        "[hidden_option:%d] [charac_no:%d]",
        (int)save.m_slot, save.m_itId,
        hiddenOption,
        (unsigned int)user->getCurCharacNo());

    // [L4fEventAvatar 2026-09-11] P0/P183：ORIG 0x0817eb94 `call Acquire` 后结果
    //   直接进 CStreamGuard ctor 的 0x4(%esp)、无 -0xc 具名槽（我方具名 stream 多
    //   `mov %eax,-0xc; mov -0xc,%eax` 往返）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp", 0xc6),
                       true);
    ARAD::DISPATCHER::make_internal_stream_jpn(
        guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2f, -1);
    guard.operator->()->put_binary(&save, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
                             (int)sig->m_avatarSlot);
    return true;
}