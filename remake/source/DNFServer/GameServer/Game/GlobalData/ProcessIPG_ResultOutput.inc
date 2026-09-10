void ItemVendingMachine::ProcessIPG_ResultOutput(CUser* user,
                                                 WongWork::IPG::SIPGData* reqData,
                                                 WongWork::IPG::SIPGData* resData,
                                                 Stream* stream)
{
    // ORIG 函数内 static：__cxa_guard + Arad_IPGProcess(this)/Arad_MileageProcess(this)
    static Arad_IPGProcess IPGProcess(this);
    static Arad_MileageProcess mMileageProcess(this);

    stCeraShopIPGRequest_t* req = stream->GetOutBuffer<stCeraShopIPGRequest_t>();

    // ORIG 局部快照（-0x74..-0x38，全部来自 req 具名字段）
    int groupIdx = req->m_groupIdx;              // local_78
    int seq = req->m_seq;                        // local_74
    int itemCount = req->m_itemCount;            // local_70
    int featuredIdx = req->m_featuredIdx;        // local_6c
    unsigned int avatarField = req->m_avatarField;   // local_68
    unsigned int productNo = req->m_productNo;   // local_64（itemIdx 64 位读的低 32 位）
    unsigned long itemIdx = (unsigned long)req->m_itemIdx;  // local_60（LP32 ulong 4B）
    unsigned short stackCount = req->m_stackCount;   // local_5c
    unsigned char avatarType0 = req->m_avatarType0;  // local_5a
    unsigned char avatarType1 = req->m_avatarType1;  // local_59
    unsigned int ceraPrice = req->m_ceraPrice;       // local_58
    unsigned int winPoint = req->m_winPoint;         // local_54
    unsigned int goldPrice = req->m_goldPrice;       // local_50
    unsigned int curSec = req->m_curSec;             // local_4c
    char buyFlag = (char)req->m_buyFlag;             // local_45
    int characNo = req->m_characNo;                  // local_44
    unsigned short buytype = req->m_buytype;         // local_3e
    unsigned int accId = req->m_accId;               // local_3c

    std::vector<Inven_Item> items;                   // local_c0
    WongWork::CCeraShop::stCeraShopItemParam_t param;  // local_d8（ORIG ctor 0x0817a244）

    for (int i = 0; i < (int)(unsigned int)(unsigned char)req->m_subCount; ++i)
    {
        // ORIG 0x08178823..0x08178861：直接 `lea` 两个缓冲地址作 make_pair 实参
        // （实例 `make_pair<unsigned long&, unsigned char&>`，随后
        // `pair<int,int>::C1<unsigned long,unsigned char>` 转换构造）——无局部
        // 拷贝、无 memcpy（旧写 memcpy×2 + make_pair<int&,int&> 是 p167 EXTRA）。
        param.m_pairs.push_back(std::make_pair(
            *(unsigned long*)&req->m_subEntries[i][0],
            *(unsigned char*)&req->m_subEntries[i][4]));
    }
    for (int i = 0; i < (int)(unsigned int)(unsigned char)req->m_optCount; ++i)
    {
        // ORIG 直接 push_back(buf+0x164+8i)（引用实参，无 memcpy/无局部 sel）。
        param.m_sel.push_back(
            *(tagPackageItemSelection*)&req->m_optEntries[i][0]);
    }

    int extraInt = 0;   // local_dc：stream >> int（购买附加参数）
    *stream >> extraInt;

    int retCode = atoi(resData->m_cmd.m_output.m_resultCode);   // local_e0
    if ((retCode != 0) || user == 0 || user->getCurCharacR() == 0)
    {
        // 错误路（ORIG 0x8178923..0x8178a5e）
        if (user == 0 || user->getCurCharacR() == 0)
        {
            cMyTrace trace(kPrettyResultOutput, 0x59d, 5);
            trace("There's abnormal transaction that the client doesn't exist, "
                  "client would be %s(m_id: %d) and the Goodis is %d, ret_code = %d ret_msg = %s",
                  resData->m_accName, atoi(resData->m_account),
                  atoi(resData->m_cmd.m_output.m_price), retCode,
                  resData->m_cmd.m_output.m_message);
        }
        m_ceraShop._ProcessIPGError(user, reqData, resData, (unsigned char)0x19, groupIdx,
                                    productNo, seq, itemCount, buyFlag);
        return;   // ORIG 0x81798d7 清理后 ret
    }

    // 成功路：retCode=0；creature 结果槽（local_e4）初值 -1；local_38 初值 -1
    retCode = 0;
    int creatureResult = -1;   // local_e4
    int groupFinal = groupIdx; // local_78 复用（case 10 可能改写为 7）

    switch (groupIdx)
    {
    case 0:
        IPGProcess.CashShopModeAvatar(user, itemIdx, productNo, avatarField,
                                      (char)req->m_avatarType0, (char)req->m_avatarType1,
                                      reqData->m_cmd.m_output.m_arg7D, retCode, accId, characNo,
                                      buyFlag);
        break;
    case 1:
        IPGProcess.CashShopModeCoin(user, stackCount, accId, characNo, buyFlag);
        break;
    case 2:
    case 7:
    case 9:
        IPGProcess.CashShopModeItem(user, itemIdx, stackCount, featuredIdx, (long)curSec,
                                    items, param, retCode, accId, characNo, buyFlag);
        break;
    case 3:
        IPGProcess.CashShopModeCreature(user, itemIdx, stackCount, creatureResult, retCode,
                                        accId, characNo, buyFlag);
        break;
    case 10:
    {
        m_ceraShop._processAdditionalItem(user, itemIdx, stackCount, avatarType0,
                                          avatarType1, items, param, 0);
        groupFinal = -1;
        CItem* item = G_CDataManager()->find_item((int)itemIdx);
        if (item != 0 && item->isCerashopPackageItem())
            groupFinal = 7;
        if (groupFinal != 7)
            items.clear();

        CItemLimitEditionMgr* mgr = G_CDataManager()->getItemLimitEditionMgr();
        CItemLimitEdition* limit = mgr->getItemInfo(productNo);
        if (limit == 0)
            throw (ENUM_ERROR)0x11;   // ORIG __cxa_throw(0x11, ENUM_ERROR)

        bool isSpecial = false;
        int outA = 0, outB = 0;   // ORIG local_100[2]/local_100[1]
        if (limit->getFeaturedIdx() == 0)
        {
            if (m_ceraShop.IsSpecialItem(limit->getRelateditem(), outA, outB, isSpecial))
                isSpecial = true;
        }
        int restrictCode = 0;
        if (isSpecial)
            restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(
                limit->getRelateditem());
        if (restrictCode != 0)
        {
            m_ceraShop.SaveFeaturedIdx(user, limit->getFeaturedIdx(),
                                       GlobalData::s_systemTime_.getCurSec(), 0);
        }
        // ORIG case 10 尾直接落入 default 的 Invalid Group Index 日志（0x8178f97 jmp
        // 链与跳转表目标核对为 fall-through 前置 0x8179xxx 段）【按 Ghidra case 排列】。
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultOutput,
                              0x653, "Invalid Group Index [%d]", groupFinal);
        break;
    }
    case 0xb:
    {
        int failCode = mMileageProcess.BuyMileageItem(
            user, productNo, itemIdx, stackCount, featuredIdx, (long)curSec, avatarField,
            (char)req->m_avatarType0, (char)req->m_avatarType1, extraInt, itemCount, seq,
            reqData->m_cmd.m_output.m_arg7D, param);
        if (failCode != 0)
            mMileageProcess.BuyFail(user, (short)failCode, productNo, seq, itemCount);
        return;   // ORIG LAB_081798d7 清理后 ret
    }
    default:
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultOutput,
                              0x653, "Invalid Group Index [%d]", groupFinal);
        break;
    }

    if (retCode != 0)
    {
        m_ceraShop._ProcessIPGError(user, reqData, resData, (unsigned char)retCode, groupFinal,
                                    productNo, seq, itemCount, buyFlag);
        cMyTrace trace(kPrettyResultOutput, 0x65b, 0);
        trace("[%s][%d]", kPrettyResultOutput, 0x65b);
        return;   // ORIG LAB_081798d7
    }

    if (groupFinal == 7)
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->setMissionValue(
            user, (HeroMissionCondition::MissionNo::T)1, 7);
    }

    // ORIG 0x08178fd2 / 0x0817907c：两分支各自独立调用 getCurCharacInvenW
    // （P48 无 CSE + P157 禁 receiver 缓存；旧具名 invenW 使 p167 MISSING
    // getCurCharacInvenW×1 且多一条 -0x?? 槽往返）。
    if (buytype == 1)
    {
        user->getCurCharacInvenW()->use_money((int)goldPrice, (eMoneySubReason)0, true);
        user->UseWinPoint((int)winPoint, (eWPSubReason)2);
        user->UseCeraPoint(ceraPrice);
        user->m_historyLog.CeraPoint((ENUM_CERAPOINT_LOG_TYPE)3,
                                     ceraPrice);
    }
    else if (buytype == 0)
    {
        GlobalData::s_pIPGHelper->IPGOutputConfirm(reqData, resData);   // ORIG vptr+0x28 slot10
        user->getCurCharacInvenW()->use_money((int)goldPrice, (eMoneySubReason)0, true);
        user->UseWinPoint((int)winPoint, (eWPSubReason)2);
        user->UseCera(ceraPrice);
        user->addUsedCera(ceraPrice);
    }

    mMileageProcess.GiveMileage(user, (int)productNo, (int)ceraPrice);

    if (CheckPurcahseBonusItem(itemIdx))
    {
        DB_UpdateCeraShopPurchaseCount::makeRequest(user->get_acc_id(), itemIdx,
                                                    productNo, user->GetUID());
    }
    CerashopAddRestrict::Manager::UpdateBuyableRestrictItem(user, productNo, 1);

    if (featuredIdx != 0 && !user->IsRestrictedGoods(featuredIdx))
    {
        user->SetRestrictedGoods(featuredIdx);
        m_ceraShop.SaveFeaturedIdx(user, featuredIdx, (int)curSec, 0);
    }

    unsigned long bonusItemIdx = (unsigned long)-1;   // local_100[0]（ORIG 低 4B 写 0xffffffff）
    unsigned int bonusCount = 0;   // local_104

    PacketGuard response;
    response.put_header(1, 0x43);
    response.put_byte(1);
    response.put_byte((int)buyFlag);
    response.put_int(groupFinal);
    response.put_int((int)productNo);
    response.put_int(seq);
    response.put_int(itemCount);
    response.put_int(creatureResult);
    response.put_short((int)items.size());
    for (std::vector<Inven_Item>::const_iterator it = items.begin(); it != items.end(); ++it)
    {
        response.put_int(it->m_addInfo);    // ORIG put_int(it+2)
        response.put_int(it->m_addInfo2);   // ORIG put_int(it+7)
    }
    response.put_short(0);

    if (buyFlag == (char)1 && accId != 0)
    {
        DB_UpdateGiftCount::makeRequest(accId, 1);
    }

    if (itemCount == seq)
    {
        user->SendCashData(false);
        PacketGuard itemPacket;
        itemPacket.clear();
        itemPacket.put_header(0, 0xe);
        itemPacket.put_byte(0);
        itemPacket.put_short(2);
        const CInventory* invenR = user->getCurCharacInvenR();
        invenR->MakeItemPacket((INVEN_TYPE)1, 0, itemPacket);
        invenR->MakeItemPacket((INVEN_TYPE)1, 2, itemPacket);
        itemPacket.finalize(true);
        user->Send(itemPacket);

        CEventBase* bonusEvent = GlobalData::s_event_manager->GetRepeatEvent(0x11);
        if (bonusEvent->IsEventing(0))
        {
            // [Main 判据②/① 回扫 · P0/P134/P226] ORIG 0x081796xx 段：
            //   `call GetRepeatEvent; mov %eax,(%esp); call getBonusRate;
            //    mov %eax,%ebx; movl $0x3e8,(%esp); call get_rand_int;
            //    cmp %eax,%ebx; setge %al`
            // ⇒ GetRepeatEvent 结果**直作 receiver**（无具名指针槽往返），
            //    getBonusRate 结果升格 %ebx 跨 get_rand_int call 保活 ⇒ 原码是
            //    **单条嵌套实参表达式**（RHS 先求值 = 链），且比较写作
            //    `get_rand_int(1000) <= <链>`。旧具名 `bonusEvent`(二次赋值)+
            //    `bonusRate` 使 4.4 落 -0x24/-0x20 两槽、并把比较反序折成
            //    `cmp -0x20(%ebp),%eax; setle`（语义等价但形态/寄存器全异）。
            // [Main 判据②/① 回扫] 操作数书写序=求值序：ORIG 先求 **链**
            // （→%ebx）后求 get_rand_int（→%eax），比较为 `cmp %eax,%ebx; setge`
            // ⇒ 源码写 `链 >= get_rand_int(1000)`（链在左）。反过来写
            // （get_rand_int 在左）4.4 先求 get_rand_int，得 `cmp %eax,%ebx;
            // setle`（语义等价、形态/寄存器/指令序全异）。
            if (static_cast<CCeraShopBonusItemEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(0x11))->getBonusRate() >=
                get_rand_int(1000))
            {
                unsigned int usedCera = user->getUsedCera();
                G_CDataManager()->m_itemList->getCeraShopBonusItem((int)usedCera,
                                                                      bonusItemIdx, bonusCount);
                if ((int)bonusItemIdx != -1)
                {
                    std::vector<Inven_Item> bonusItems;
                    m_ceraShop._processAdditionalItem(user, bonusItemIdx,
                                                      (unsigned short)bonusCount, 0, 0xff,
                                                      bonusItems, param, 0);
                }
            }
        }
    }

    response.put_int(bonusItemIdx);
    response.put_int((int)bonusCount);
    response.finalize(true);
    user->Send(response);

    GlobalData::s_BreakAwaySys->updateUsedCera(user->get_acc_id(), ceraPrice);

    unsigned long long billUid =
        resData->m_cmd.m_output.m_tranNo;   // ull@+0x1e0（两 dword 拼接即此字段，TaiwanCash.h 实证）
    cUserHistoryLog* history = &user->m_historyLog;
    if (buytype == 1)
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      resData->m_cmd.m_output.m_arg7D, resData->m_cmd.m_output.m_message,
                      -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount,
                                 resData->m_cmd.m_output.m_arg7D,
                                 buyFlag, (CeraShopPaymentType::T)1, billUid);
    }
    else
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      reqData->m_cmd.m_output.m_arg7D, reqData->m_cmd.m_output.m_message,
                      -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount,
                                 reqData->m_cmd.m_output.m_arg7D,
                                 buyFlag, (CeraShopPaymentType::T)0, billUid);
    }
