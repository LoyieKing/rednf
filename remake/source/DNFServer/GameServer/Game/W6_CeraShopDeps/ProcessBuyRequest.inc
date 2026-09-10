bool CCeraShop::ProcessBuyRequest(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
{
    // [b28d-S278 P0/P227] ORIG 0x0832047a：NumberToString 结果落 %ebx 无栈槽、
    // itemCount 在 cMyTrace ctor **之前**求值入 %esi ⇒ 原码是全表达式临时链
    // （具名 idStr/trace 两语句各多 store/load 并让实参求值后移到 ctor 之后）。
    cMyTrace(__PRETTY_FUNCTION__, 0x92, 0)(
        "NEXON BILLING : ProcessBuyRequest(m_id(%s),count(%d))",
        NumberToString(user->get_acc_id(), 0),
        static_cast<int>(msg.m_scalars.m_itemCount));

    // ORIG 0x77：36-dword 清零区（-0x2a8..-0x218，后续无任何读点，-O0 死
    // 清零照抄，P171）；随后 -0x84/-0x80 两槽写 0（-0x80 即 avatarField
    // 预置，-0x84 死写）。
    // [b28d-L14 实证] ORIG 0x0832047a 该区为 rep stos（36 dwords）⇒ 显式零初始化
    // 数组（P214③）；旧 memset 生成 call 形（P167 EXTRA memset ×3 之一）。
    unsigned int deadPad[36] = {0};
    (void)deadPad;
    int dead88 = 0;
    (void)dead88;
    unsigned int avatarSelField = 0;

    CCeraShopGoods* firstGoods =
        G_CDataManager()->FindGoods(msg.m_entrySection.m_arr[0].m_productNo);
    if (firstGoods == 0)
    {
        // [b28d-S278 P16] ORIG 0x08320538 `jne <主体>`：错误块直落 + `mov $1; jmp 尾`
        // ⇒ 原码为早返形（if(!=0)/else 会让 4.4 把错误块排到函数末尾，整段窗口错位）。
        ProcessError(user, 0x15, 0, msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                     static_cast<char>(msg.m_scalars.m_buyFlag));
        return true;
    }

    {
        if (WongWork::CEventMgr::IsChildrensDayEventing())
        {
            user->ClearChildrensDayEvent();
        }

        // [b28d-S278 P214③] ORIG 0x083205a9/0x083205bf 两段 `rep stos`（各 0x24
        // dwords）⇒ 原码是数组零初始化；逐元素 for 循环 4.4 恒生成循环体。
        CCeraShopGoods* goodsList[36] = {0};
        int relatedItems[36] = {0};

        // [b28d-S278 负例] ORIG -0x78 双循环复用，但函数级共用变量实测不可复现
        // 槽位（我方 -0x78 被 checkIdx 决策树占用、i 落他槽，净 +1 条）⇒ 回退。
        for (int i = 0; i < static_cast<int>(msg.m_scalars.m_itemCount); ++i)
        {
            goodsList[i] = G_CDataManager()->FindGoods(
                msg.m_entrySection.m_arr[i].m_productNo);
            if (goodsList[i] == 0)
            {
                int negIdx = -1;
                ProcessError(user, 0x15, negIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1, static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;

                cMyTrace t(__PRETTY_FUNCTION__, 0xc3, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0xc3);
                continue;
            }

            int groupIdx = goodsList[i]->GetGroupIdx();
            if (user->IsGameMasterMode() != 1)
            {
                // 非 GM：sticker/gift 禁售检查（ORIG 0x29c..0x46a）。
                if (goodsList[i]->GetSticker() == 99)
                {
                    ProcessError(user, 8, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0xde, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0xde);
                    continue;
                }
                if (static_cast<char>(msg.m_scalars.m_buyFlag) == 1 &&
                    goodsList[i]->isGiftDisallowance_() != 0)
                {
                    ProcessError(user, 8, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0xe8, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0xe8);
                    continue;
                }
            }
            // LAB_083208e9：GM 直落 / 非 GM 检查通过落入。
            if (goodsList[i]->GetGroupIdx() != 1)
            {
                relatedItems[i] =
                    reinterpret_cast<int>(goodsList[i]->GetRelatedItem());
                if (relatedItems[i] == 0)
                {
                    ProcessError(user, 0x11, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0x116, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0x116);
                    continue;
                }
            }

            // PVP 渠道禁购清单（ORIG 0x57e..0x68e 决策树，条目 itemIdx）。
            bool pvpRestricted = false;
            int checkIdx = goodsList[i]->GetItemIdx();
            if (checkIdx == 0x334)
            {
                pvpRestricted = true;
            }
            else if (checkIdx < 0x335)
            {
                if (checkIdx < 0xc6)
                {
                    if (checkIdx >= 0xc4 || checkIdx == 0xc2)
                    {
                        pvpRestricted = true;
                    }
                    else if (checkIdx == 0xc3)
                    {
                        user->isAffectedPremium(
                            static_cast<ENUM_PREMIUM_TYPE>(0x21));   // 副作用查询
                    }
                }
                else if (checkIdx == 0x2d0 ||
                         (checkIdx > 0x2cf &&
                          (static_cast<unsigned int>(checkIdx - 0x2e7) < 2)))
                {
                    pvpRestricted = true;
                }
            }
            else if (checkIdx <= 0x289832)
            {
                if (checkIdx >= 0x289831 || checkIdx == 0x2897c2 ||
                    checkIdx == 0x289806 || checkIdx == 0x398)
                {
                    pvpRestricted = true;
                }
            }
            else if (checkIdx == 0x28beba || checkIdx == 0x28d154)
            {
                pvpRestricted = true;
            }

            if (pvpRestricted)
            {
                char blockPvp = '\0';
                if (G_GameWorld()->IsPVPChannel())
                {
                    blockPvp = '\x01';
                    if (G_GameWorld()->IsPvPSkilTreeChannel() &&
                        user->get_pvp_grade() >= 10)
                    {
                        blockPvp = '\0';
                    }
                }
                if (blockPvp != '\0')
                {
                    ProcessError(user, 7, groupIdx,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0x16c, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0x16c);
                    continue;
                }
            }

            // _CheckBuyingCondition（ORIG 0x757：字段逐次从 msg 重读，P183）。
            unsigned short expiry =
                static_cast<unsigned short>(goodsList[i]->GetExpiryDate());
            if (!this->_CheckBuyingCondition(
                    user, msg.m_entrySection.m_arr[i].m_productNo,
                    static_cast<unsigned long>(goodsList[i]->GetItemIdx()),
                    static_cast<unsigned int>(goodsList[i]->GetStackCount()),
                    expiry,
                    msg.m_entrySection.m_arr[i].m_avatarType0,
                    static_cast<unsigned short>(static_cast<signed char>(
                        msg.m_entrySection.m_arr[i].m_avatarType1))))
            {
                ProcessError(user, 0x4f, groupIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1,
                             static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;
                cMyTrace t(__PRETTY_FUNCTION__, 0x185, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0x185);
            }
        }   // 条目循环（校验段）

        if (!_CheckValidCount(const_cast<const CCeraShopGoods**>(goodsList)))
        {
            int negOne = -1;
            ProcessError(user, 1, negOne, msg.m_entrySection.m_arr[0].m_productNo,
                         1, 1, static_cast<char>(msg.m_scalars.m_buyFlag));
            return true;
        }

        user->resetUsedCera();

        // ORIG 清零顺序：priceStr(0x24) rep stos + serialBuf(0x15) 内联慢路；
        // countStr/itemName 不清零（N78：4.4 生成 memset call）。
        // [b28d-L14 实证] 两 memset 均改显式零初始化数组（P214③，消 EXTRA memset）。
        char priceStr[0x24] = {0};
        char serialBuf[0x15] = {0};
        char countStr[8];
        char itemName[0x64];

        unsigned int nowSec =
            static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
        long long remMoney =
            static_cast<long long>(static_cast<int>(user->getCurCharacMoney()));
        long long remWP = static_cast<long long>(user->GetWinPoint());
        unsigned int ceraPrice = 0;
        int goldPrice = 0;
        unsigned int wpPrice = 0;

        for (int i = 0; i < static_cast<int>(msg.m_scalars.m_itemCount); ++i)
        {
            if (goodsList[i] == 0)
            {
                continue;
            }
            if (goodsList[i]->GetGroupIdx() != 0)
            {
                ceraPrice = static_cast<unsigned int>(goodsList[i]->GetCeraPrice());
                goldPrice = goodsList[i]->GetGoldPrice();
                wpPrice = static_cast<unsigned int>(goodsList[i]->GetWinPoint());
            }
            else
            {
                int groupSub = 0;
                CItem* related =
                    static_cast<CItem*>(goodsList[i]->GetRelatedItem());
                if (related->IsAvatarItem())
                {
                    related = static_cast<CItem*>(goodsList[i]->GetRelatedItem());
                    const std::vector<stAvatarTypeSelect_t>* selects =
                        static_cast<const std::vector<stAvatarTypeSelect_t>*>(
                            static_cast<CEquipItem*>(related)
                                ->getAvatarTypeSelect());
                    if (selects->empty() ||
                        static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1) >=
                            static_cast<int>(selects->size()))
                    {
                        ProcessError(user, 0x17, groupSub,
                                     msg.m_entrySection.m_arr[i].m_productNo,
                                     i + 1,
                                     static_cast<int>(msg.m_scalars.m_itemCount),
                                     static_cast<char>(msg.m_scalars.m_buyFlag));
                        goodsList[i] = 0;
                        cMyTrace t(__PRETTY_FUNCTION__, 0x253, 0);
                        t("[%s][%d]", __PRETTY_FUNCTION__, 0x253);
                        continue;
                    }
                    if (msg.m_entrySection.m_arr[i].m_avatarType0 >= 100)
                    {
                        ProcessError(user, 0x17, groupSub,
                                     msg.m_entrySection.m_arr[i].m_productNo,
                                     i + 1,
                                     static_cast<int>(msg.m_scalars.m_itemCount),
                                     static_cast<char>(msg.m_scalars.m_buyFlag));
                        goodsList[i] = 0;
                        cMyTrace t(__PRETTY_FUNCTION__, 0x25e, 0);
                        t("[%s][%d]", __PRETTY_FUNCTION__, 0x25e);
                        continue;
                    }
                    // ORIG 四字段各发一次 const operator[]（P157/P183）。
                    ceraPrice = static_cast<unsigned int>(
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_fieldc);
                    goldPrice =
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_field4;
                    wpPrice = static_cast<unsigned int>(
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_field8);
                    avatarSelField = static_cast<unsigned int>(
                        (*selects)[static_cast<signed char>(
                            msg.m_entrySection.m_arr[i].m_avatarType1)].m_field0);
                }
                else
                {
                    ProcessError(user, 0x17, groupSub,
                                 msg.m_entrySection.m_arr[i].m_productNo,
                                 i + 1,
                                 static_cast<int>(msg.m_scalars.m_itemCount),
                                 static_cast<char>(msg.m_scalars.m_buyFlag));
                    goodsList[i] = 0;
                    cMyTrace t(__PRETTY_FUNCTION__, 0x247, 0);
                    t("[%s][%d]", __PRETTY_FUNCTION__, 0x247);
                    continue;
                }
            }

            // LAB_083212dc：会员折扣（cera/wp 无符号乘除，gold 有符号）。
            unsigned short discountRate =
                user->GetPremiumInfo()->getCeraShopDiscountRate();
            if (discountRate != 0)
            {
                ceraPrice -=
                    static_cast<unsigned int>(discountRate) * ceraPrice / 100;
                goldPrice -= static_cast<int>(discountRate) * goldPrice / 100;
                wpPrice -=
                    static_cast<unsigned int>(discountRate) * wpPrice / 100;
            }

            // 额度校验（11=cera 不足，10=金币不足，12=WP 不足）。
            int errorReason = 0;
            if (user->GetCera() < ceraPrice)
            {
                errorReason = 0xb;
            }
            else
            {
                if (user->getCurCharacInvenR()->get_money() < goldPrice)
                {
                    errorReason = 10;
                }
                else if (user->GetWinPoint() < wpPrice)
                {
                    errorReason = 0xc;
                }
            }

            // ORIG 0xf55..0x1037：64 位余额复核 + 扣减，
            // std::max<long long>(0LL, rem-cost) 夹紧（两条）。
            if (remMoney < static_cast<long long>(goldPrice))
            {
                errorReason = 10;
            }
            if (remWP < static_cast<long long>(wpPrice))
            {
                errorReason = 0xc;
            }
            remMoney = std::max(0LL, remMoney - static_cast<long long>(goldPrice));
            remWP = std::max(0LL, remWP - static_cast<long long>(wpPrice));

            if (errorReason != 0)
            {
                unsigned int uWp = user->GetWinPoint();
                unsigned int uGold =
                    static_cast<unsigned int>(user->getCurCharacInvenR()->get_money());
                unsigned int uCera = user->GetCera();
                LogManager::logFormat(
                    1, "CeraShop.cpp", __PRETTY_FUNCTION__, 0x2d6,
                    "IPG ERROR [Dispatcher_BuyCeraShopItem::dispatch_sig] "
                    "UserID[%s] ACera[%d] AGold[%d] AWP[%d]",
                    user->getCurCharacName(), static_cast<int>(ceraPrice),
                    goldPrice, static_cast<int>(wpPrice));
                LogManager::logFormat(
                    1, "CeraShop.cpp", __PRETTY_FUNCTION__, 0x2d7,
                    " / UCera[%d] UGold[%d] UWP[%d]\n",
                    uCera, uGold, uWp);
                LogManager::logFormat(
                    1, "CeraShop.cpp", __PRETTY_FUNCTION__, 0x2d9,
                    "Error Goods No : %d",
                    msg.m_entrySection.m_arr[i].m_productNo);

                int errGroupIdx = goodsList[i]->GetGroupIdx();
                ProcessError(user, static_cast<unsigned short>(errorReason),
                             errGroupIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1,
                             static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;
                cMyTrace t(__PRETTY_FUNCTION__, 0x2e1, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0x2e1);
                continue;
            }

            if (user->IsRestrictedGoods(goodsList[i]->GetFeaturedIdx()))
            {
                int errGroupIdx = goodsList[i]->GetGroupIdx();
                ProcessError(user, 0x14, errGroupIdx,
                             msg.m_entrySection.m_arr[i].m_productNo,
                             i + 1,
                             static_cast<int>(msg.m_scalars.m_itemCount),
                             static_cast<char>(msg.m_scalars.m_buyFlag));
                goodsList[i] = 0;
                cMyTrace t(__PRETTY_FUNCTION__, 0x2ed, 0);
                t("[%s][%d]", __PRETTY_FUNCTION__, 0x2ed);
                continue;
            }

            // stream 组装（P103：operator-> 链保留；写入序照 ORIG 0x1359..）。
            Stream* reqStream =
                GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x37b);
            CStreamGuard guard(reqStream, false);
            stCeraShopIPGRequest_t* req =
                guard.operator->()->GetInBuffer<stCeraShopIPGRequest_t>();
            req->m_productNo =
                static_cast<unsigned int>(msg.m_entrySection.m_arr[i].m_productNo);
            req->m_groupIdx = goodsList[i]->GetGroupIdx();
            req->m_stackCount =
                static_cast<unsigned short>(goodsList[i]->GetStackCount());
            req->m_ceraPrice = ceraPrice;
            req->m_goldPrice = static_cast<unsigned int>(goldPrice);
            req->m_winPoint = wpPrice;
            req->m_itemIdx = goodsList[i]->GetItemIdx();
            req->m_avatarType0 = msg.m_entrySection.m_arr[i].m_avatarType0;
            req->m_avatarType1 = msg.m_entrySection.m_arr[i].m_avatarType1;
            req->m_avatarField = avatarSelField;
            req->m_featuredIdx = goodsList[i]->GetFeaturedIdx();
            req->m_itemCount = msg.m_scalars.m_itemCount;
            req->m_seq = i + 1;
            req->m_curSec = nowSec;
            req->m_buyFlag = msg.m_scalars.m_buyFlag;
            req->m_accId = msg.m_scalars.m_accId;
            req->m_characNo = msg.m_scalars.m_characNo;
            req->m_subCount = msg.m_entrySection.m_arr[i].m_subCount;
            for (int s = 0;
                 s < static_cast<int>(msg.m_entrySection.m_arr[i].m_subCount);
                 ++s)
            {
                *reinterpret_cast<unsigned int*>(&req->m_subEntries[s][0]) =
                    static_cast<unsigned int>(
                        msg.m_entrySection.m_arr[i].m_subEntries[s].m_itemIdx);
                req->m_subEntries[s][4] =
                    msg.m_entrySection.m_arr[i].m_subEntries[s].m_value;
            }
            req->m_optCount = msg.m_entrySection.m_arr[i].m_optCount;
            for (int o = 0;
                 o < static_cast<int>(msg.m_entrySection.m_arr[i].m_optCount);
                 ++o)
            {
                *reinterpret_cast<unsigned int*>(&req->m_optEntries[o][0]) =
                    static_cast<unsigned int>(
                        msg.m_entrySection.m_arr[i]
                            .m_optEntries[o + 0x16].m_itemUid);
                *reinterpret_cast<unsigned short*>(&req->m_optEntries[o][4]) =
                    msg.m_entrySection.m_arr[i].m_optEntries[o + 0x16].m_a;
                *reinterpret_cast<unsigned short*>(&req->m_optEntries[o][6]) =
                    msg.m_entrySection.m_arr[i].m_optEntries[o + 0x16].m_b;
            }

            OS_API::snprintf(priceStr, 0x24, "%d", static_cast<int>(ceraPrice));
            OS_API::snprintf(countStr, 8, "%d",
                             msg.m_entrySection.m_arr[i].m_productNo);

            if (goodsList[i]->GetItemIdx() != 1)
            {
                CItem* priced =
                    reinterpret_cast<CItem*>(relatedItems[i]);
                if (priced->is_stackable() &&
                    goodsList[i]->GetStackCount() > 1)
                {
                    OS_API::snprintf(itemName, 0x62,
                                     "\xbd\xc3\xb4\xdc\xc7\xd2\xc0\xaf %s %d\xb0\xb3",
                                     priced->GetItemName(),
                                     goodsList[i]->GetStackCount());
                }
                else
                {
                    strcpy(itemName, priced->GetItemName());
                }
            }
            else
            {
                OS_API::snprintf(itemName, 0x62,
                                 "\xbd\xc3\xb4\xdc\xc7\xd2\xc0\xaf %d\xb0\xb3",
                                 goodsList[i]->GetStackCount());
            }

            if (ceraPrice == 0)
            {
                const char* charName = user->getCurCharacName();
                char* uniqueNo = CGenUniqueNo::GenUniqueNo(serialBuf);
                GlobalData::s_pIPGHelper->IPGOutputSimulation(
                    user, countStr, priceStr, uniqueNo, itemName, charName,
                    static_cast<void*>(&guard));
            }
            else
            {
                const char* charName = user->getCurCharacName();
                char* uniqueNo = CGenUniqueNo::GenUniqueNo(serialBuf);
                GlobalData::s_pIPGHelper->IPGOutput(
                    user, countStr, priceStr, uniqueNo, itemName, charName,
                    static_cast<void*>(&guard));
            }
            // guard RAII 析构释放流（ORIG 1876 D1）。
        }
    }
    return true;
