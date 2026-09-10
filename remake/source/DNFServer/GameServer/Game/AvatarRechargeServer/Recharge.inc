bool AvatarRechargeServer::Recharge(CUser* user,
                                    arad::SigAradUseAvatarRecharge* sig)
{
    int mapKey;   // ORIG 0x08190cae 的 key 物化槽（-0x3c）——声明提前试验
    const Inven_Item* stoneRef =
        user->getCurCharacInvenR()->GetInvenRef(1, (int)sig->stoneSlot);
    const int stoneItemId = stoneRef->m_addInfo;   // ORIG 判空前读取，忠实保序
    if (stoneRef == 0 || stoneRef->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }

    // [b28d-solo-AvatarRechargeServer] P59/P168a：ORIG 0x08190b5a
    // `xor $1,%eax; test %al,%al; je <成功块>` ⇒ 负极性 guard（成功路径 fall-through）。
    if (!user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, sig->stoneSlot, 1,
                            (eItemDelReason)3, true))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x13);
        ARAD::PRINT_ERROR::delete_item((char*)"USE AVATAR RECHANGE", user,
                                       sig->stoneSlot);
        return false;
    }

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             sig->stoneSlot);
    // [b28d-solo-AvatarRechargeServer] P227 全表达式临时链 + P139 __PRETTY_FUNCTION__
    // （ORIG 0x08190bd0..0x08190c20 先求 varargs 再 call cMyTrace::C1）。
    cMyTrace(__PRETTY_FUNCTION__, 0xe8, 0)(
        "USE AVATAR RECHANGE : delete_item() success. [slot:%d] "
        "[charac_no:%d]",
        (int)sig->stoneSlot, user->getCurCharacNo());

    Inven_Item* avatar = user->getCurCharacInvenW()->GetInvenRef(2, (int)sig->avatarSlot);
    // ORIG 0x08190c50 只判 isEmpty（无 `avatar==0` 比较，原码直接解引用）⇒ P171 照抄。
    if (avatar->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->avatarSlot);
        return false;
    }

    // ORIG 0x08190cae `mov -0x24(%ebp),%eax; mov %eax,-0x3c(%ebp); lea -0x3c(%ebp),%eax`
    // ⇒ map 键经临时槽中转（4.1 对 operator[] const key& 的物化）。
    int newEndur = (int)avatar->m_fieldSplit.m_avatarEndur +
                   m_emblemRechargeStoneMap[mapKey = stoneItemId];
    if (newEndur > sig->maxEndurance)
        newEndur = sig->maxEndurance;
    sig->addEndurance =
        newEndur - (int)avatar->m_fieldSplit.m_avatarEndur;
    avatar->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;
    sig->enduranceAfter =
        (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
                             sig->avatarSlot);
    cMyTrace(__PRETTY_FUNCTION__, 0xfe, 0)(
        "USE AVATAR RECHARGE : success [slot:%d] [item_id:%d] "
        "[Endurance:%d] [charac_no:%d]",
        (int)sig->avatarSlot, avatar->m_addInfo,
        (int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur,
        user->getCurCharacNo());

    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)avatar->m_addInfo2;
    save.endurance =
        (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;
    SaveAvatarEnduranceUnit(save);
    return true;
