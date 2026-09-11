// ---------------------------------------------------------------------------
// ProcessBuyRequest —— ORIG T 0x0832047a (0x18b3)
// MSG 条目逐个 FindGoods → 非 GM 走 sticker/gift 禁售检查 → GM/常规路径查
// GetRelatedItem/PVP 禁购清单/_CheckBuyingCondition(0x8323f86) →
// _CheckValidCount 通过后重置 UsedCera 并逐条折扣计价、64 位余额扣减
// （std::max<long long>(0LL,·) 夹紧）→ 组装 stCeraShopIPGRequest_t 经
// CStreamGuard 发往 s_pIPGHelper 的 IPGOutputSimulation(+0x18)/IPGOutput
// (+0x14)（ceraPrice==0 分支）。控制流、错误码、cMyTrace 行号与字符串均照
// 报告 /tmp/pbr_report.md（ORIG 汇编 + Ghidra C）逐块转写；MSG 访问一律
// 经 SigTypes_A.h 具名字段视图（条目步长 0x1cf，P161 组件视图）。
// 说明【忠实度局限】：0x8323f86 六参校验在 ORIG 以 6 数据槽装载
// （goodsNo / itemIdx:stackCount 组合 long / expiry / byte37 / byte38sext），
// 对齐现存 CCeraShop_Special.cpp 实现体的头声明，末参 e 以 0 补位。
// 【§6.2 验收标注】orig=1551 our=1562；P167 调用点差集归零（仅余 N78 三处
// rep stos→memset call 差：deadPad/priceStr/serialBuf 清零区）；其余 |gap|
// 为 N82 帧/槽号与 4.1/4.4 条件物化（sete/setne、块排布）残余 ⇒ 语义完成、
// byte 层 N 封顶，勿再耗轮次。
// ---------------------------------------------------------------------------
// [b28c-L7][SIGFIX:WongWork::CCeraShop::ProcessBuyRequest] int→bool（判词见
// CCeraShop.h:153）；体内两处 return 值实测仅 1（无错误码语义）。
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N82 封顶（orig 1551 / our 1588，
// gap=37=2.4%；p167 CLEAR 127/127）。本轮修法：P0/P227 全表达式临时链
// （NumberToString 结果无槽、itemCount 在 cMyTrace ctor 前求值）、P214③
// goodsList/relatedItems 数组零初始化复现 ORIG 两段 rep stos、P16 早返形
// （firstGoods==0 错误块直落 + jmp 尾）。残余：帧 0x43c vs ORIG 0x40c 及全帧
// 槽号平移（N82：ORIG 给 12 个 cMyTrace 各占独立 0x10 槽，4.4 复用同槽）
// + N7 间接调用目标寄存器 + 跨版本寄存器/寻址形态 ⇒ §6.2 三件套判语义完成、
// byte 层 N 封顶。
// [N1A 2026-09-11 · 本波] ①PVP 决策树改 goto 形（消 bool 物化）−7；
// ②`m_optEntries[o + 0x16]`→`[o]` 真 bug 修正（读偏移多 0xb0）−2；
// 合计 our 1588→1579，gap −37→−28（n1_v 口径；clean 计数 −37→−23）。
// 残余为实参装载序/JCC 极性（`cmpl $0x289830; jg` vs ORIG `cmp $0x289831;
// jge`，P59 极性族）+ N82 槽号平移，指令数 1:1 ⇒ 封顶。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N78）→ 试=新口径 n1_v 1551/1579 gap=-28 blocks=245；残差=N78 三处 rep stos→memset 差（deadPad/priceStr/serialBuf 清零区，帧 0x40c vs 0x43c）+ N82 槽号/块排布 + JCC 极性（`cmp $0x289831;jge` vs 我方 `cmp $0x289830;jg`）⇒ 维持（§6.2 判语义完成，勿再耗轮次）。
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
            // [N1A 2026-09-11] ORIG 形态 = 判定树各命中点直接 `je <单份 body>`
            //（0x08320a0f/0a28/0a33/0a4a/0a65/0a73/0a7a/0a88/0a94/0a9b），body 单份
            // 落 0x08320aba（`movb $0x0,-0x45(%ebp)` 起）；无 bool 槽物化，
            // 故不设 `pvpRestricted` 局部，直接 goto 共享标签。
            int checkIdx = goodsList[i]->GetItemIdx();
            if (checkIdx == 0x334)
                goto pvpBlock;
            else if (checkIdx < 0x335)
            {
                if (checkIdx < 0xc6)
                {
                    if (checkIdx >= 0xc4 || checkIdx == 0xc2)
                        goto pvpBlock;
                    else if (checkIdx == 0xc3)
                    {
                        user->isAffectedPremium(
                            static_cast<ENUM_PREMIUM_TYPE>(0x21));   // 副作用查询
                    }
                }
                else if (checkIdx == 0x2d0 ||
                         (checkIdx > 0x2cf &&
                          (static_cast<unsigned int>(checkIdx - 0x2e7) < 2)))
                    goto pvpBlock;
            }
            else if (checkIdx <= 0x289832)
            {
                if (checkIdx >= 0x289831 || checkIdx == 0x2897c2 ||
                    checkIdx == 0x289806 || checkIdx == 0x398)
                    goto pvpBlock;
            }
            else if (checkIdx == 0x28beba || checkIdx == 0x28d154)
                goto pvpBlock;
            goto pvpDone;

        pvpBlock:
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
        pvpDone:;

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
                // [N1A 2026-09-11 真 bug 修正 · P92 族] 原写 `[o + 0x16]`
                // 使读取地址多 0xb0（0x16×8），真因：4.4 把 ORIG 的
                // `m_optEntries[o]`（+0xbe = 0x16*8+0xe）规范化为
                // `(o+0x16)*8 + 0xe`，误把索引常量抄进源码。直证：
                // ORIG 0x08320b3x 读侧 `add $0x16,%ebx; shl $3,%ebx`（一次
                // +0x16）＋`mov 0xe(%eax),%edx`；我方原形 `lea 0x16(%edx),%ebx;
                // add $0x16,%ebx; shl $3,%ebx`（两次）⇒ 地址 0x16e+o*8 vs
                // ORIG 0xbe+o*8。旁证 GlobalData.cpp:5340 同源消费点用 `[k]`。
                *reinterpret_cast<unsigned int*>(&req->m_optEntries[o][0]) =
                    static_cast<unsigned int>(
                        msg.m_entrySection.m_arr[i]
                            .m_optEntries[o].m_itemUid);
                *reinterpret_cast<unsigned short*>(&req->m_optEntries[o][4]) =
                    msg.m_entrySection.m_arr[i].m_optEntries[o].m_a;
                *reinterpret_cast<unsigned short*>(&req->m_optEntries[o][6]) =
                    msg.m_entrySection.m_arr[i].m_optEntries[o].m_b;
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
}