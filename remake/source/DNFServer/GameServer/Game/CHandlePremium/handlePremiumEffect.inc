void CHandlePremium::handlePremiumEffect(CUser* user, ENUM_PREMIUM_TYPE type)
{
    // [b28d-56 判词·N] N82：ORIG 0x086afc32 456 条 vs 我们 434 条（帧 0xec vs 0xfc、
    // 槽号整体平移）；P157 已修 11 处 operator[] MISSING→6；残余 6 处为缺字段访问
    // （ORIG 12 次 vs 我们 6 次）+ getCurCharacInvenR 内联差。
    CUserPremium* premium = user->GetPremiumInfoW();
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (!user->IsPremiumUser())
        return;

    std::vector<SUserPremiumInfo> infoList = premium->GetPremiumInfoList((int)type);  // ORIG 0x86afc6e 传 type 参数

    for (size_t i = 0; i < infoList.size(); ++i)
    {
        // [b28d-56 判词·P157] ORIG 每处字段访问都重发 `vector<SUserPremiumInfo>::operator[]`
        // （p167: ORIG 12 次 vs 旧码引用缓存 1 次 ⇒ MISSING 11）；逐处写
        // `infoList[i].m_xxx`，禁引用缓存。
        int iType = infoList[i].m_type;
        if (curSec < infoList[i].m_start)   // ORIG 0x086afd3f：未开始（+0x4）→ 定时
        {
            // 未开始：注册开始定时（msg 6）。
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                        (TIMER_MESSAGE)6, infoList[i].m_start - curSec,
                                        iType, user->get_acc_id());
        }
        else
        {
            if (user->getCurCharacLastPlayTick() <= infoList[i].m_end)
            {
                // 到期且上次游玩不晚于结束：发优势物品 + 金币。
                std::vector<std::pair<int, int> > items = premium->GetAdvantageItem(iType);
                for (size_t j = 0; j < items.size(); ++j)
                {
                    int itemIdx = items[j].first;
                    int count = items[j].second;
                    Inven_Item item;
                    CItem* ci = G_CDataManager()->find_item(itemIdx);
                    if (ci == 0)
                        continue;
                    ci->make_item(item);
                    item.m_addInfo = itemIdx;                  // +0x02
                    item.m_field1 = (unsigned char)(itemIdx >> 0x10);  // +0x01 高 16 位
                    item.m_addInfo2 = (item.m_field1 == 1)
                                      ? user->GetSeedFromDate() : count;  // +0x07
                    int insertRet = user->getCurCharacInvenW()->insertItemIntoInventory(
                        item, (eItemAddReason)0xe, true, true);
                    if (insertRet < 0)
                    {
                        CMailBoxHelper::ReqDBSendNewMail(
                            user, item, 0, (unsigned int)user->getCurCharacNo(), true, 0, 0);
                    }
                    else
                    {
                        user->SendUpdateItemList(CUser::eSendTarget_1,
                                                 (ENUM_ITEMSPACE)0, insertRet);
                    }
                }
                if (iType != 9999)
                {
                    CInventory* invR = const_cast<CInventory*>(user->getCurCharacInvenR());
                    int coin = (int)invR->GetCoin()
                             + g_SPremiumInfo[iType].m_premiumCount;
                    user->getCurCharacInvenW()->SetCoin((unsigned int)coin);
                    int curCoin = (int)invR->GetCoin();
                    int add = g_SPremiumInfo[iType].m_premiumCount;
                    user->m_historyLog.CoinAdd(curCoin, add, (eCoinAddReason)4);
                }
            }

            // 每日定时（仅在 premium 定义需每日检查时）。
            if (g_SPremiumInfo[iType].m_checkTerm != 0 &&
                infoList[i].m_end > curSec)
            {
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                            (TIMER_MESSAGE)4, infoList[i].m_end - curSec,
                                            iType, user->get_acc_id());
            }
            if (premium->IsNeedCheckEveryday())
            {
                time_t t = (time_t)curSec;
                struct tm tmv;
                localtime_r(&t, &tmv);
                tmv.tm_mday += 1;
                tmv.tm_hour = 0;
                tmv.tm_min = 0;
                tmv.tm_sec = 0;
                time_t nextDay = mktime(&tmv);
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                            (TIMER_MESSAGE)5, (int)(nextDay - t),
                                            iType, user->get_acc_id());
            }
        }
    }

    if (user->IsHavePremiumAdvantage() && (int)type != 9999)
    {
        premium->RecalcAdditionalInfo(user);
        user->SendNotiPacket(CUser::eSendTarget_1, (ENUM_NOTIPACKET)2, 1);
    }
    user->ProcPremiumFatigue();
    user->SendFatigue();
    user->SendCoin();
    if ((int)type == 9999)
        user->setCurCharacLastPlayTick((long)curSec);
