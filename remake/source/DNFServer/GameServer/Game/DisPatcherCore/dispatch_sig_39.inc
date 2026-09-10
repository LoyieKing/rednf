int Dispatcher_AddSocketToAvatar::dispatch_sig(CUser* user, PacketBuf& packet)

{
    MSG_ADD_SOCKET_TO_AVATAR msg;
    // [b28e-L5] P171/P232 死写入照抄：ORIG 0x0821a428 在 ctor 之后发
    // `movl $0x0,-0x28(%ebp)`（= msg 槽尾 -0x3d+0x15 之后的 4B 局部，全函数
    // **0 个读取点**，objdump 全区间仅 1 处 -0x28(%ebp) 写）⇒ 原码有未读标量局部。
    int unusedZero = 0;   // 死写入
    (void)unusedZero;
    if (!packet.get_short(msg.m_slotA))
        return LineFunc(0xcd60,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(msg.m_addInfo))
        return LineFunc(0xcd62,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(msg.m_slotB))
        return LineFunc(0xcd64,
                        __PRETTY_FUNCTION__,
                        0, 0);

    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item avatarItem = inven->GetInvenSlot(2, (int)msg.m_slotA);   // space=2 avatar
    if (avatarItem.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 4);
        return 0;
    }
    if ((int)msg.m_addInfo != avatarItem.m_addInfo)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 4);
        return 0;
    }

    Inven_Item cubeItem;                                              // ORIG ctor + 内联逐字段拷贝
    cubeItem = inven->GetInvenSlot(1, (int)msg.m_slotB);              // space=1 inventory
    if (cubeItem.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x16);
        return 0;
    }

    CItem* cube = G_CDataManager()->find_item(cubeItem.m_addInfo);
    if (cube == 0 || !cube->is_stackable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }
    // [Lane38 2026-09-10] 符号绑定（判据㉓）：ORIG 0x0821a8c6/0x0821a8d6 在
    // `mov -0x1c(%ebp),%eax; add $0x3a0,%eax; movl $0x0,0x4(%esp)` 后
    // **call `_ZNKSt6vectorIiSaIiEEixEj`**（const 重载）；我方非 const 指针绑到
    // `_ZNSt6vectorIiSaIiEEixEj`（P167 MISSING/EXTRA 各 1）。本函数对 stackable 的
    // 全部用法（verifyStackableAction 0x0822ca58 `_ZNK…`、get_grade、m_vec3a0）皆
    // const ⇒ 原码即 `const CStackableItem*`。
    const CStackableItem* stackable = static_cast<const CStackableItem*>(cube);
    if (!stackable->verifyStackableAction((StackableAction::Type)0x20))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 4);
        return 0;
    }
    if (!avatarItem.isAvatarItemType())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }

    CItem* source = G_CDataManager()->find_item((int)msg.m_addInfo);
    bool okSource = false;                                            // ORIG bVar1
    if (source != 0)
    {
        okSource =
            (source->get_grade() == stackable->get_grade())
            && source->IsAvatarItem();
    }
    if (!okSource)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }
    CEquipItem* equip = static_cast<CEquipItem*>(source);
    if (equip->check_eType('\b'))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }

    // [b28d] P0/P183：ORIG 0x0821a8xx 直接 `mov stackable,%eax; add $0x3a0,%eax`
    // 取 vector 成员地址（无 socketTypes 引用槽），我方引用槽撑帧 4B。
    if (stackable->m_vec3a0.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x16);
        return 0;
    }
    const int socketType = stackable->m_vec3a0[0];
    bool slotOk = false;                                              // ORIG local_d(-0x9)
    if (socketType == 0)
        slotOk = !equip->IsClearAvatar();
    else if (socketType == 1)
        slotOk = equip->IsClearAvatar();
    else if (socketType == 2)
        slotOk = true;
    if (!slotOk)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }

    // ---- 主路径：avatar key 门禁 → 徽章写槽 → 删宝石 → DB 同步 → 回包 ----
    // [b28d] P0：ORIG 无 avatarKey 具名局部——每次直读 `mov -0x9e(%ebp),%eax`
    // （= avatarItem(-0xa5) 的 m_addInfo2 @+7）。
    // ORIG 0x0821aa8x：IsTempKey 判定前有一次 GetAvatarItemMgrW（结果丢弃，call site 保留）。
    inven->GetAvatarItemMgrW();
    if (WongWork::CAvatarItemMgr::IsTempKey(avatarItem.m_addInfo2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }
    if (inven->GetAvatarItemMgrW()->isSocketAvatar(avatarItem.m_addInfo2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x13);
        return 0;
    }

    stAvatarEmblemInfo_t emblem;
    emblem.init();
    if (source != 0 && source->IsAvatarItem())
        equip->getAvatarSocket(-1, emblem);
    inven->GetAvatarItemMgrW()->setEmblemSocketData(avatarItem.m_addInfo2, emblem);

    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, (int)msg.m_slotB, 1,
            (eItemDelReason)0x1b, true))
        return 0x11;

    DB_UpdateAvatarJewelSlot::makeRequest(
        (unsigned int)user->getCurCharacNo(),
        (unsigned int)avatarItem.m_addInfo2, &emblem);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1,
                             (int)(unsigned short)msg.m_slotA);

    PacketGuard packetGuard;
    packetGuard.put_header(1, 0xd1);
    packetGuard.put_byte(1);
    packetGuard.put_short((int)(unsigned short)msg.m_slotA);
    packetGuard.put_short((int)(unsigned short)msg.m_slotB);
    packetGuard.finalize(true);
    user->Send(packetGuard);
    return 0;
}
