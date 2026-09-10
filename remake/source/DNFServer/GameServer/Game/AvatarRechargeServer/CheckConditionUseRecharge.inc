bool AvatarRechargeServer::CheckConditionUseRecharge(
    CUser* user, arad::SigAradUseAvatarRecharge* sig)
{
    const Inven_Item* stone =
        user->getCurCharacInvenR()->GetInvenRef(1, (int)sig->stoneSlot);
    if (stone == 0 || stone->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }
    // b28d-lane25：P167 差集实证 ORIG 0x081907b8/0x081907cd 直接内联
    // map find/end + eq（end() 在左控实参序，P74 补充），不调 IsEmblemRechargeStone。
    // [b28d-solo-AvatarRechargeServer] ORIG 0x081907b8 先 `call end`（sret -0x20）后
    // `call find`（sret -0x1c，key 物化 -0x18）⇒ 具名 end 迭代器声明在前。
    // 残余 NEAR=2：4.1 先声明占深槽（end -0x20/find -0x1c）vs 4.4 相反（N93 类对象
    // 槽方向跨版本不可翻；P293 单表达式形 4.4 恒 find 先 ⇒ DIFF 更差，已回退）。
    std::map<int, int>::iterator itEnd = m_emblemRechargeStoneMap.end();
    std::map<int, int>::iterator it = m_emblemRechargeStoneMap.find(stone->m_addInfo);
    if (itEnd == it)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            "bool AvatarRechargeServer::CheckConditionUseRecharge(CUser*, "
            "arad::SigAradUseAvatarRecharge*)",
            0xad,
            "USE AVATAR RECHANGE : [item_id:%d] isn't RechargeStone [charac_no:%d]",
            stone->m_addInfo, user->getCurCharacNo());
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->stoneSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->stoneSlot);
        return false;
    }

    const Inven_Item* avatar =
        user->getCurCharacInvenR()->GetInvenRef(2, (int)sig->avatarSlot);
    if (avatar == 0 || avatar->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->avatarSlot);
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->avatarSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->avatarSlot);
        return false;
    }

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
            avatar->m_addInfo2);
    // [b28d-solo-AvatarRechargeServer] 具名化基类上限字段（原 `*(int*)((char*)this+4)`
    // 裸偏移，§4.3 禁）；ORIG 0x08190a66 `jl <成功块>` 先 call 后读 +4 的求值序。
    // ORIG 0x08190a66 段：`call getSocketCount; mov 0x4(%edx),%edx; imul %eax,%edx`
    // ⇒ 书写序 count * limit（左操作数先求值，与 UseEmblem 同）。
    sig->maxEndurance = getSocketCount(socketData) * m_rechargeLimit;
    if ((int)(unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur >=
        sig->maxEndurance)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd8);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            "bool AvatarRechargeServer::CheckConditionUseRecharge(CUser*, "
            "arad::SigAradUseAvatarRecharge*)",
            0xcd,
            "USE AVATAR RECHANGE : already max endurance [ui_id:%d] [charac_no:%d]",
            avatar->m_addInfo2, user->getCurCharacNo());
        return false;
    }
    return true;
