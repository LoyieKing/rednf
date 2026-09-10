bool AvatarFixedHiddenOptionServer::ChangeHiddenOption(
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    // [b28d-56 判词·N] N7/N82附：ORIG 0x0817e8d6 230 条 vs 我们 234 条；残余为
    // prologue 寄存器分配（ORIG push edi/esi/ebx + sub 0x7c vs 我们 push esi/ebx +
    // sub 0x70）与实参装载序。语义完成。
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
    cMyTrace traceDel(
        __PRETTY_FUNCTION__,
        0xa6, 0);
    traceDel("AVATAR HIDDEN OPTION CHANGE : delete_item() success. [slot:%d] "
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

    int skillIdx = GetSkillIndex((ENUM_RARITY)item->get_grade());
    ref->m_fieldSplit.m_fielddLo = (short)skillIdx;      // +0x0d word 写入
    sig->m_hiddenOption = (unsigned short)(short)ref->m_fieldSplit.m_fielddLo;

    arad::SigAradAvatarHiddenOptionSave save;             // +0/…清零语义同 ORIG W ctor
    save.m_field0 = user->get_acc_id();
    save.m_characNo = (unsigned int)user->getCurCharacR()->m_characNo;
    save.m_itId = (unsigned int)ref->m_addInfo;
    save.m_slot = (short)((int)sig->m_avatarSlot + 10);
    save.m_hiddenOption = (short)ref->m_fieldSplit.m_fielddLo;

    cMyTrace traceOk(
        __PRETTY_FUNCTION__,
        0xc2, 0);
    traceOk("AVATAR HIDDEN OPTION CHANGE : success [slot:%d] [item_id:%d] "
            "[hidden_option:%d] [charac_no:%d]",
            (int)((int)sig->m_avatarSlot + 10), save.m_itId,
            (int)(short)save.m_hiddenOption,
            (unsigned int)user->getCurCharacNo());

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp", 0xc6);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(
        guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2f, -1);
    guard.operator->()->put_binary(&save, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
                             (int)sig->m_avatarSlot);
    return true;
