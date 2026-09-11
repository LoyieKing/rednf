// §6.2 巨件核销：P167 111/111 [0缺0多]；gap 24→24；残差归因 N82（4.4 槽复用全帧错位，
// 详见 docs/asm_translation_patterns.md 负例表）。batch-20「Inter 巨兽」已核销，本次维持。
// [b28d-lane2] verdict=DIFF ndiff=1035（ORIG 0x084d5930）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+21。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=802（ORIG 0x84d5930）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
// [IG2PartyCera 2026-09-10] 语义轴真缺口修复（清单 our_only 0x3/0xd 复核）：
//   ① 0xd 来自旧 `for(i<sizeof(RandomOption))` 循环界，ORIG 0x084d66e5..0x084d66f7 起为**结构赋值展开**
//      ⇒ 改 `pck.item_info.m_opt = item.m_random.m_random;`（源 item+0x25 ✓ 逐条同构）。
//   ② 核 ORIG 该段（0x084d66d9..0x084d6730）直证：连续 24B 目标 item_info+0x13..+0x2b 分
//      两段写入 —— 前 0xa = item.m_amp.m_guid（源 item+0x15/0x19/0x1d = amp+0x04/08/0c，
//      4+4+2），后 0xe = RandomOption。旧实现只拷后者 ⇒ item_info+0x13..0x1c（m_pad）恒 0，
//      与 ORIG 发出的包内容不一致（**真缺口**）⇒ 补 `*(stAmplifyGuidView*)m_pad = item.m_amp.m_guid;`。
//   ③ 余 0x3（switch 决策树节点）为 lowering 差异（ORIG 用 case 0..5 跳表 0x8c8922c，
//      我方树；case 集合 1/2/5→err0x91、3→flag、0/4→default 逐点核对一致）⇒ 记 N。
//   读数：our 1074→1071（ORIG 1073，gap -1→+2，语义轴不受闸门）；TU 四数
//   135/1370/1/115/703 改前改后逐值不变。
int Inter_AuctionResultAskRegistedItemNum::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultAskRegistedItemNumPktView* pPkt = reinterpret_cast<Inter_AuctionResultAskRegistedItemNumPktView*>(data);
    if (user == 0)                                   // asm cmpl $0,user
        return 0;

    // [K3 2026-09-11] P0 死变量：ORIG 084d5950 `mov -0x68(%ebp),%eax; mov 0x4(%eax),%eax;
    //   mov %eax,%ebx` 直跨 getCurCharacNo 调用，无槽 ⇒ 原码无 reqCharacNo 局部（旧稿多 2 条）。
    if (pPkt->m_4 != user->getCurCharacNo()) // 差值即 ORIG setne 判定
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x37a3, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultAskRegistedItemNum",
                  user->getCurCharacNo());
            break;
        }
    }

    const unsigned int payType =
        pPkt->m_0;                      // msg+0x00
    const short nowRegisted =
        pPkt->m_8;                         // msg+0x08
    if (nowRegisted == -1)
    {   // 上限查询失败应答 err=0x9e
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x9e, (PAY_TYPE)payType);
        return 0;
    }
    if (nowRegisted == -2)
    {   // 非法区/黑名单 err=0x91
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
        return 0;
    }

    // 已登记数量上限校验：premium/gold 两套上限表（err=0x97）
    const bool premiumAffected = user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x1d);
    if (!premiumAffected)
    {
        const int maxCount = (pPkt->m_0 == 1) ? 3 : 10;
        if (maxCount <= nowRegisted)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x97, (PAY_TYPE)payType);
            return 0;
        }
    }
    else
    {
        const int maxCount = (pPkt->m_0 == 1) ? 10 : 30;
        if (maxCount <= nowRegisted)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x97, (PAY_TYPE)payType);
            return 0;
        }
    }

    // 手续费门槛（gold 登记 10000 / cera 0），不足 err=0x90
    int registMoney = (pPkt->m_0 == 1) ? 0 : 10000;
    if (user->getCurCharacMoney() < registMoney)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x90, (PAY_TYPE)payType);
        return 0;
    }

    const unsigned int itemId =                                       // msg+0x0d
        pPkt->m_d;
    // cera 礼包手续费基数（ORIG 仅当 pay==1 且 itemId∈礼包区间时换算登记费）
    long bonusSec = 0;
    if ((0x28d287 < itemId && itemId < 0x28d29a) &&
        pPkt->m_0 == 1)
    {
        if (itemId < 0x28d292)
            bonusSec = static_cast<long>(itemId) * 1000000 + 0x199b9040;
        else
            bonusSec = static_cast<long>(itemId) * 10000000 - 0x549a800;
    }

    const char invenKind = *(data + 0xa);                             // msg+0x0a
    const unsigned short slotNo =                                     // msg+0x0b
        pPkt->m_b;
    const int price = pPkt->m_15;     // msg+0x15
    const int instantPrice = pPkt->m_19;  // msg+0x19
    const int restrictAmount = pPkt->m_11;  // msg+0x11

    PCK_AUCTION_REGIST_ITEM_GA pck;

    // ---- 公共头域填写（asm +0x2f6..+0x42b 区）----
    pck.char_idx = static_cast<int>(user->GetUID());                  // GetUID
    pck.owner_id = user->getCurCharacNo();                            // getCurCharacNo
    strncpy(pck.owner_name,
            const_cast<char*>(reinterpret_cast<const char*>(user->getCurCharacR())) + 4, 0xd);
    pck.price = price;
    pck.instant_price = instantPrice;
    pck.black_point = user->getCurrCharacBlackCount();
    pck.unit_price = pPkt->m_1d;      // msg+0x1d
    pck.user_type = static_cast<unsigned char>(
        user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x1d));             // ORIG 第二次 isAffectedPremium → local_158
    {   // ORIG 三次 movl 装载 msg+0x21/0x25/0x29（12B ROI_Category），非 memcpy（_sort 延至块尾，ORIG +0x1577）
        unsigned int* roiw = reinterpret_cast<unsigned int*>(&pck._reg_roi_category_key);
        const unsigned int* roir = reinterpret_cast<const unsigned int*>(data + 0x21);
        roiw[0] = roir[0];
        roiw[1] = roir[1];
        roiw[2] = roir[2];
    }

    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item item;                                                  // 局部空 Inven_Item

    if (itemId != 0)
    {
        // 槽位域 → INVEN_TYPE 映射（0→INVEN、1→AVATAR、7→CREATURE）
        int invenType = 1;
        if (invenKind == '\x01')
            invenType = 2;
        else if (invenKind == '\a')
            invenType = 3;
        else if (invenKind != '\0')
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }

        // cera 礼包专区（itemId∈礼包区间 & pay==1）：直接扣登记费 + 转发cera代理
        if ((0x28d287 < itemId && itemId < 0x28d29a))
        {
            if (pPkt->m_0 != 1)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x93, (PAY_TYPE)payType);
                return 0;
            }
            if (!inven->use_money(static_cast<int>(bonusSec), (eMoneySubReason)0x26, true))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x93, (PAY_TYPE)payType);
                return 0;
            }
            user->SaveInventory();
            user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
            pck.setCategory(0x12);
            pck.item_info.m_addInfo = restrictAmount;                  // ORIG local_149 = msg+0x11
            pck.jewel_info.init();                                    // ORIG stAvatarEmblemInfo_t::init(pck+0x65)
            pck.item_info.m_seal = false;                               // ORIG local_14f = 0
            pck.item_info.m_itemId = itemId;                            // ORIG local_14e = msg+0x0d
            pck.item_info.m_attr = 0;                             // ORIG local_145 = 0
            strncpy(pck.owner_nexon_id, user->get_acc_name(),
                    strlen(user->get_acc_name()));                     // ORIG 形：strncpy(dst,acc,strlen)（不补 \0）
            GlobalData::s_cera_auction_proxy->SendPacket(&pck);
            return 0;
        }

        // 实体槽位副本与合法链
        item = inven->GetInvenSlot(invenType, slotNo);
        if (item.m_addInfo != static_cast<int>(itemId) ||
            item.m_addInfo == 0x10ec)                                  // 禁拍 ID
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }
        if (IsTradeLimitAttachTypeItemByUser(*user, invenType, slotNo))
        {
            bool movable = true;
            CheckItemTradeLimitCountToMoveByUser(*user, invenType, slotNo, movable);
            if (!movable)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x72);
                return 0;
            }
        }
        if (!item.isEquipableItemType() &&
            static_cast<int>(item.m_addInfo2) < restrictAmount)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x92, (PAY_TYPE)payType);
            return 0;
        }

        CDataManager* dm = G_CDataManager();
        CItem* ci = dm->find_item(static_cast<int>(itemId));
        if (ci == 0 || !ci->isPackagable())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }

        // [b28d-solo-InterDispatcher] P282 槽序还原：ORIG 0x084d617e/0x084d64a1/0x084d65ab
        // 三次 `vptr+0x10` 间接调用 = _ZTV5CItem word6 = CItem::IsAvatarItem()（每次重调，
        // 非缓存 bool）；旧 avatarFlow 缓存使三处虚调用坍缩为一次。
        if (ci->IsAvatarItem())
        {
            const WongWork::CAvatarItemMgr* mgr =
                user->getCurCharacInvenR()->GetAvatarItemMgrR();      // ORIG getCurCharacInvenR→GetAvatarItemMgrR→GetExpireDate
            const int expireDate = mgr->GetExpireDate(item.m_addInfo2);
            if (expireDate != 0 &&
                GlobalData::s_systemTime_.getCurSec() > expireDate)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
        }
        else
        {
            const int usablePeriod = ci->getUsablePeriod();
            const int expirationDate = ci->getExpirationDate();
            if (usablePeriod != 0 || expirationDate != 0)
            {   // ORIG 双重判定（重复 getter 调用——不得合并为局部缓存）：
                //   第二次重读 usablePeriod/expirationDate 同时为 0 → err=0x91（ORIG +0x0a27）；
                //   否则到期时间 local_28 <= getCurSec → err=0x91（ORIG +0x0a31）。
                if (ci->getUsablePeriod() == 0 && ci->getExpirationDate() == 0)
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                    return 0;
                }
                const long endTime =
                    static_cast<long>(item.m_fieldd & 0xff) * 0x15180 +
                    0x44a54a80;
                if (endTime <= GlobalData::s_systemTime_.getCurSec())
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                    return 0;
                }
            }
        }

        if (inven->GetCreatureMgrR()->FindCreatureItem(item.m_addInfo2) == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }

        if (pck.user_type == 0 && ci->GetCashPrice() != 0 &&
            !G_CDataManager()->is_AuctionRegFreeCeraItem(itemId))     // ORIG 第二次 G_CDataManager（+0x0ae4）
        {   // 非白名单付费道具禁止金币拍卖（err=0x95）
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x95, (PAY_TYPE)payType);
            return 0;
        }

        switch (ci->GetAttachType())
        {
        case 1:
        case 2:
        case 5:
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        case 3:                                                        // 绑定型须非空
            if (item.m_addInfo == 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
            break;
        default:
            break;
        }

        if (item.m_upgradeSep.IsTradeRestriction())                    // 分离升级位限制
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91);
            return 0;
        }
        if (ci->IsAvatarItem())                                   // ORIG +0xb71 第二次 vptr+0x10
        {
            const WongWork::CAvatarItemMgr* mgrT = inven->GetAvatarItemMgrR();   // ORIG 第 5 处 GetAvatarItemMgrR（+0x0bee 前置 mgr 装载）
            if (mgrT->IsTempKey(item.m_addInfo2))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
            if (inven->GetAvatarItemMgrR()->GetExpireDate(item.m_addInfo2) != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
        }

        // ==== 包主体详细字段化（报告反编译 local_14f..local_126 全链）====
        pck.item_info.m_seal = false;
        pck.item_info.m_itemId = itemId;
        pck.item_info.m_attr = item.GetItemAttr();                // 提升档位/seal 位
        pck.item_info.m_endurance = item.m_fieldb;
        pck.item_info.m_extendInfo = item.m_fieldd;
        pck.item_info.m_addInfo =
            item.isEquipableItemType() ? item.m_addInfo2 : restrictAmount;
        item.m_amp.getValues(pck.item_info.m_abilityType, pck.item_info.m_abilityValue);
        pck.jewel_info.init();                                         // ORIG 无条件 stAvatarEmblemInfo_t::init(local_11a)
        if (ci->IsAvatarItem())                                   // ORIG +0xc7b 第三次 vptr+0x10
        {
            const WongWork::CAvatarItemMgr* mgrJ = inven->GetAvatarItemMgrR();   // ORIG +0x0cad
            memcpy(&pck.jewel_info,
                   mgrJ->getJewelSocketData(item.m_addInfo2), 0x1e);             // ORIG memcpy(local_11a,src,0x1e)
            const WongWork::CAvatarItemMgr* mgrE = inven->GetAvatarItemMgrR();   // ORIG +0x0ce2
            const void* exp = mgrE->GetExpansionInfo(item.m_addInfo2);           // ORIG +0x0ce7
            if (exp == 0)
                pck.expansion.init();                                            // ORIG stAvatarExpansionInfo_t::init
            else
                memcpy(&pck.expansion, exp, 4);                                  // ORIG memcpy(asStack_fc,src,4)
            if (pck.jewel_info.stEmblemSocket[0].emblemSocketType_ != 0)
                pck.item_info.m_attr =
                    (unsigned char)((pck.item_info.m_attr & 0xe0) | 0x1);
        }
        {   // ORIG 逐字段（CONCAT13/CONCAT33）写 random_option，非 memcpy；随后 SetUpgradeSeparate
            // [IG2PartyCera] ORIG 0x084d66d9..0x084d6730 直证：连续 24 字节目标
            // item_info+0x13..+0x2b 分两段结构赋值 —— 前 10B(0x84d66d9/d66e5/d66f1)
            // = item.m_amp.m_guid（源 item+0x15/+0x19/+0x1d = amp+0x04/08/0c，4+4+2），
            // 后 14B = RandomOption（源 item+0x25）。旧实现只拷后者 ⇒ DnfItemInfo
            // +0x13..0x1c（m_pad）恒 0，与 ORIG 发出的包内容不一致（真缺口）。
            *reinterpret_cast<stAmplifyGuidView*>(pck.item_info.m_pad) =
                item.m_amp.m_guid;
            pck.item_info.m_opt = item.m_random.m_random;
            pck.item_info.m_up.SetUpgradeSeparate(
                item.m_upgradeSep.GetUpgradeSeparate());
        }

        // [b28d-solo-InterDispatcher] P282 槽序还原：ORIG 0x084d675f `vptr+0x4c` ⇒
        // _ZTV5CItem word21 = CItem::IsRandomOption()（0x08514412）；循环体在其 true 分支内。
        if (ci->IsRandomOption())
        {
            for (int k = 0; k <= 2; ++k)
            {
                if (pck._reg_roi_category_key.m_keys[k] == 0)
                    pck._reg_roi_category_key.m_keys[k] = 30000;
            }
        }
        pck._reg_roi_category_key._sort();                            // ORIG ROI_Category::_sort（+0x1577）
    }

    // ---- END 收尾段（LA_084d67be 起）----
    const int fee = (pPkt->m_0 == 1) ? 0 : 10000;
    if (!inven->use_money(fee, (eMoneySubReason)0x19, true))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x93, (PAY_TYPE)payType);
        return 0;
    }
    if (item.m_addInfo != 0)
    {
        if (item.m_field1 == '\b')                                     // 化身穿戴位删除
        {
            if (!inven->delete_item((INVEN_TYPE)2, slotNo, 1, (eItemDelReason)5, true))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x94, (PAY_TYPE)payType);
                const int gained = inven->gain_money(10000, (eMoneyAddReason)0x13, true, 0);
                if (gained > 9999)
                    return 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, 10000,
                                          static_cast<unsigned long>(gained));
                return 0;
            }
            inven->SendAvatarEvent(0x36, item.m_addInfo2, 4, slotNo + 10, 0);
            inven->GetAvatarItemMgrW()->UnRegistItem(item.m_addInfo2);
        }
        else
        {
            if (!inven->delete_item((INVEN_TYPE)item.m_field0, slotNo, static_cast<int>(pck.item_info.m_addInfo),
                                    (eItemDelReason)0x19, true))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x94, (PAY_TYPE)payType);
                const int gained = inven->gain_money(10000, (eMoneyAddReason)0x13, true, 0);
                if (gained > 9999)
                    return 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, 10000,
                                          static_cast<unsigned long>(gained));
                return 0;
            }
        }
        if (item.IsCreatureItemType())
        {
            inven->GetCreatureMgrW()->AuctionRegist(&item, slotNo, 0x19);
        }
    }
    user->SaveInventory();
    strncpy(pck.owner_nexon_id, user->get_acc_name(), strlen(user->get_acc_name()));  // ORIG strncpy（END）
    GlobalData::s_auction_proxy->SendPacket(&pck);
    return 0;
}