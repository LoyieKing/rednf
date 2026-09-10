int DisPatcher_DisJointItem::_disjoint(CUser* user, int slot, int type,
                                       ENUM_CMDPACKET cmd, CUser* targetUser,
                                       unsigned short region)
{
    // 081f9301 _check_disjointable_user_state(user, cmd, (char)type, (short)slot)
    if (!_check_disjointable_user_state(user, cmd,
                                        static_cast<char>(type),
                                        static_cast<short>(slot)))
    {
        return 0;
    }

    PacketGuard packet;                              // local_74（try 块外构造）
    try
    {
        // +0x337 put_header(1, cmd)
        packet.put_header(1, cmd);

        // +0x042 GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)type)（W 0x080f7845，定义 CTitleBook.cpp）
        const int invenType =
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<ENUM_ITEMSPACE>(type));
        expert_job::CExpertJob* expertJob = 0;       // local_44
        if (targetUser != 0)
        {
            expertJob = targetUser->GetCurCharacExpertJob();   // 0x0822f8d4
            if (expertJob == 0)
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0x87b7,
                                      "expert job null point");
                throw (ENUM_ERROR)0x13;
            }
            if (expertJob->GetType() != 3)            // W 0x08234796，非虚
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0x87bd,
                                      "expert job permission error");
                throw (ENUM_ERROR)0x13;
            }
            expertJob->OnStartMission(targetUser, user);  // vptr slot0 虚调用
        }

        // +0x18b CUser::CheckItemLock(int,int) const
        if (user->CheckItemLock(invenType, slot))
        {
            throw (ENUM_ERROR)0xd5;
        }

        Inven_Item invenItem;                          // local_fd
        CItem* item = 0;                               // local_78
        if (region != 0xffff)
        {
            GameWorld* gw = G_GameWorld();
            if (gw->IsPVPChannel())
            {
                throw (ENUM_ERROR)0x60;
            }
        }
        // +0x263 _check_disjointable_item_state（C++ 名定义于 ExpertJobLink4.cpp）
        _check_disjointable_item_state(user, slot, invenType, invenItem, &item,
                                       targetUser, region);
        // +0x286 _check_disjoint_need_slot（C++ 名定义于 ExpertJobLink4.cpp）
        _check_disjoint_need_slot(
            user, item, invenItem,
            reinterpret_cast<expert_job::CDisjointer*>(expertJob));

        DisJointResult result;                         // local_c0，ctor 清零 10×int
        const int srcItemIdx = invenItem.m_addInfo;    // local_fb = item+0x02
        int ret = 0;                                   // local_40
        if (targetUser == 0)
        {
            DisJoint* dj = G_CDataManager()->m_disJoint;   // +0x1c 已具名
            ret = dj->GetResult(srcItemIdx, result);
        }
        else
        {
            ret = static_cast<expert_job::CDisjointer*>(expertJob)
                      ->get_disjoint_result(item, &invenItem, result, user);
        }
        if (0 < ret)
        {
            throw (ENUM_ERROR)ret;
        }
        if (region != 0xffff)
        {
            CInventory* invenW = user->getCurCharacInvenW();
            if (invenW->use_item((int)region, 1, 1) != 0)
            {
                throw (ENUM_ERROR)0x16;
            }
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)region);
        }

        CItem* foundItems[6];                          // local_90[0..5]
        foundItems[0] = 0;                             // local_90[0]=0（+0x41a）
        int sellPrice0 = 0;                            // 源物品卖价出参（GetSellItemPrice 第 5 参）
        Inven_Item outItems[5];                        // local_26b..，步长 0x3d
        for (int i = 0; i < 5; ++i)
        {
            if (result.m_f[i + 1] == 0)                // -0xb8+(i+1)*4 = result+4*(i+1)
            {
                continue;
            }
            foundItems[i + 1] = G_CDataManager()->find_item(result.m_f[i + 1]);
            if (foundItems[i + 1] == 0)
            {
                continue;
            }
            outItems[i].m_addInfo = result.m_f[i + 1];   // +0x2 ← m_f[i+1]
            outItems[i].set_add_info(result.m_f[i]);     // +0x7 ← m_f[i]
            foundItems[i + 1]->make_item(outItems[i]);   // vptr+0x8 槽
            const int slotResult =
                ((const CInventory*)user->getCurCharacInvenR())
                    ->tryInsertItemIntoInventory(outItems[i]);
            if (slotResult == -1)
            {
                throw (ENUM_ERROR)4;
            }
        }
        // 删除源物品（INVEN_TYPE, slot, 1, eItemDelReason 9, 1）
        const bool deleted = user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)invenType, slot, 1, (eItemDelReason)9, true);
        if (!deleted)
        {
            throw (ENUM_ERROR)ret;
        }
        // 源物品卖价统计（sellPrice 用 foundItems[0] 槽位承接，ORIG local_90[0]）
        foundItems[0] = 0;
        G_Store()->GetSellItemPrice(invenItem, item, (short)invenItem.m_addInfo2,
                                    false, sellPrice0);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x13, user, (unsigned int)sellPrice0);
        if (targetUser == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)7, 0, (ENUM_PACKETCLASS)0);
        }
        else if (user != 0)
        {
            if (user->get_charac_no(-1) == targetUser->get_charac_no(-1))
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    user, (APSystem::_ActionGroupIndex)0x20, 0,
                    (ENUM_PACKETCLASS)0);
            }
            else
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    targetUser, (APSystem::_ActionGroupIndex)0x2d, 0,
                    (ENUM_PACKETCLASS)0);
            }
        }
        Inven_Item unusedPacketTmp;  // ORIG +0x765 构造 -0x136 处 Inven_Item，后续未使用
        packet.put_byte(1);
        packet.put_short(slot);
        packet.put_byte(type);
        int countIndex = packet.get_index();           // local_94
        packet.put_byte(0);
        int outCount = 0;                              // local_34
        for (int i = 0; i < 5; ++i)
        {
            if (foundItems[i + 1] == 0)
            {
                continue;
            }
            if (foundItems[i + 1]->is_stackable() &&
                999 < outItems[i].get_add_info())
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x8867, 5);
                trace("DisPatcher_DisJointItem::dispatch_sig, out_item , ITEM #%d, %d",
                      (int)outItems[i].m_addInfo, (int)outItems[i].get_add_info());
            }
            const int newSlot =
                user->getCurCharacInvenW()->insertItemIntoInventory(
                    outItems[i], (eItemAddReason)10, true, false);
            if (newSlot >= 0)
            {
                const Inven_Item* ref =
                    user->getCurCharacInvenR()->GetInvenRef(1, newSlot);
                if (ref != 0 && ref->m_addInfo != (int)outItems[i].m_addInfo)
                {
                    cMyTrace trace2(__PRETTY_FUNCTION__, 0x888a, 0);
                    trace2("ENUM_CMDPACKET_DISJOINT_ITEM error item_id mismatch %d, %d",
                           (int)ref->m_addInfo, (int)outItems[i].m_addInfo);
                }
                packet.put_short(newSlot);
                packet.put_int((int)outItems[i].m_addInfo);
                packet.put_int((int)outItems[i].m_addInfo2);
                if (targetUser == 0)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        (int)outItems[i].m_addInfo, (int)outItems[i].m_addInfo2,
                        user, (CUBE_STATISTIC_FIELD)0);
                }
                else if (targetUser->GetCurCharacExpertJob() != 0)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        (int)outItems[i].m_addInfo, (int)outItems[i].m_addInfo2,
                        user, (CUBE_STATISTIC_FIELD)2);
                }
                int itemPrice = 0;
                G_Store()->GetSellItemPrice(outItems[i], foundItems[i + 1],
                                            (short)outItems[i].m_addInfo2,
                                            false, itemPrice);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)8, user, (unsigned int)itemPrice);
                ++outCount;
            }
        }
        packet.put_byte(countIndex, outCount);
        if (targetUser != 0)
        {
            static_cast<expert_job::CDisjointer*>(expertJob)
                ->complete_disjoint(user, packet);
        }
        packet.finalize(true);
        user->Send(packet);
        HistoryLog::WriteDisJoint(
            reinterpret_cast<FILE*>(user->m_field796f8), invenItem.m_addInfo);
    }
    catch (ENUM_ERROR e)
    {
        packet.put_byte(0);
        packet.put_byte((int)e);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
