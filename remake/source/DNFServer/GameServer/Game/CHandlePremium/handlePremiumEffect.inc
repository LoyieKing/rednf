// ---- handlePremiumEffect —— ORIG 0x086afc32 (T) ----
// 逐 premium 项处理：到期前定时、到期后发优势物品/金币、每日检查定时、
// 收尾 RecalcAdditionalInfo/SendNotiPacket + ProcPremiumFatigue/SendFatigue/SendCoin。
void CHandlePremium::handlePremiumEffect(CUser* user, ENUM_PREMIUM_TYPE type)
{
    // [b28d-56 判词·N → L6S5 复修] ORIG 0x086afc32 456 条 vs 旧码 434 条（帧 0xec vs 0xfc，
    // 槽号整体深 0x14 = 5 槽）。根因 = 旧码 5 个**发明具名局部**（ORIG 全无）+ 9999 判定用错
    // 接收者，三旋钮一次到位（0x086afc32..0x086b02f3 逐地址核对）：
    //   ① iType（-0x44）：ORIG 6 处 fresh 重发 operator[]（0x086afcd3/0x086afd7b/0x086affb2/
    //      0x086b00d1/0x086b01be + GetAdvantageItem 0x086afd7b）⇒ 删局部，逐点写
    //      infoList[i].m_type（P157）；
    //   ② invR/coin/curCoin/add（-0x2c/-0x28/-0x24/-0x20）：ORIG 0x086aff90..0x086b0037 无槽，
    //      getCurCharacInvenR 两处 fresh call、GetCoin 直落 %ebx/%eax ⇒ 内联表达式（P183）；
    //   ③ `if (iType != 9999)` → ORIG 0x086aff83 `cmpl $0x270f,0xc(%ebp)` 比较 **type 形参**，
    //      旧码比 iType 属接收者错（语义轴）；④ 删 `item.m_field1 = itemIdx >> 0x10` ——
    //      ORIG 0x086afe20 只读 +0x01（-0x97(%ebp) 全函数仅此一处）无写，该字段由
    //      make_item 写；旧码覆盖 make_item 结果并改变 m_addInfo2 取值（语义轴）。
    //   ⑤ P214① `if (!IsPremiumUser()) return;` → `if (IsPremiumUser()) { 主流程 }`（ORIG
    //      0x086afcc2 `test %al,%al; je 086b02f3` 仅 2 条无 xor）；⑥ P222 insertRet 分支
    //      正向包裹（ORIG `js` 跳后置 mail 块）；⑦ 删 `if (ci == 0) continue;`（ORIG
    //      0x086afdf8→0x086afdfb 直连 call *%edx 无判空 = 发明防御）；⑧ m_addInfo2 比较
    //      字段错位修复见下方 m_start 注释；⑨ 去掉 coin 加法的冗余 (int) cast（4.4 少 1 条 mov）。
    // 探针（4.4.7）：/tmp/l6s5_probe1.cpp（P183 内联形无槽）、/tmp/l6s5_probe4.cpp（P272）、
    // /tmp/l6s5_probe6.cpp（t_pos 复现 `test %al; je`）。
    // [L6S5 实测] 旧 434 条/ndiff 490 → **454 条/ndiff 56**（ORIG 456；gap=+2，我方少 2）。
    // 残余 56 行全为 N 类（无源码解）：①5 组 `%esi`↔`%ebx` callee-saved 角色对调（N82附，
    // 与 handleSetUserPCRoom 同族）；②N7 间接 call 目标 %edx↔%eax；③m_addInfo2 三目块序镜像
    // （P47 取反形实测 59→59 无效，已回滚）；④g_SPremiumInfo[..].m_premiumCount 的
    // `mov <A>(%eax),%eax; mov %eax,%ebx` 两段（4.1 保留 scratch 中转，P254 族）。
    CUserPremium* premium = user->GetPremiumInfoW();
    int curSec = GlobalData::s_systemTime_.getCurSec();
    // [P214① 正条件 if 包裹] ORIG 0x086afcc2 `test %al,%al; je 086b02f3`（函数尾，仅 2 条）
    // ⇒ 原码是 `if (IsPremiumUser()) { 主流程 }`（探针 /tmp/l6s5_probe6.cpp t_pos 实测
    // 4.4.7 逐条同形）；旧码 `if (!f()) return;` 产 `xor $1,%eax; test; jne` 三件套
    // （t_neg 实证）。premium/curSec 的求值在 if **之外**（ORIG 0x086afc3e/0x086afc8a 在先）。
    // 注：本块内层缩进未随包裹调整，避免大范围重排引入抄写风险（结构由花括号决定）。
    if (user->IsPremiumUser())
    {
    std::vector<SUserPremiumInfo> infoList = premium->GetPremiumInfoList((int)type);  // ORIG 0x86afc6e 传 type 参数

    for (size_t i = 0; i < infoList.size(); ++i)
    {
        // [b28d-56 判词·P157] ORIG 每处字段访问都重发 `vector<SUserPremiumInfo>::operator[]`
        // （p167: ORIG 12 次 vs 旧码引用缓存 1 次 ⇒ MISSING 11）；逐处写
        // `infoList[i].m_xxx`，禁引用缓存。
        // [b28d-56 判词·P157 续] ORIG **无** iType 具名槽：每处 fresh 重发 operator[]
        // （0x086afcd3/0x086afd7b/0x086affb2/0x086b006e/0x086b00d1/0x086b01be 六处
        //  `mov -0x34(%ebp),%eax; … call operator[]; mov (%eax),%edx`，各后接 0x270f/
        //  g_SPremiumInfo/GetAdvantageItem 消费）；我方旧码 `int iType = infoList[i].m_type;`
        // 一次读 + 6 处 reload ⇒ 少 5 次 call + 多 1 槽（帧 0xfc vs ORIG 0xec 的来源之一）。
        if (curSec < infoList[i].m_start)   // ORIG 0x086afd3f：未开始（+0x4）→ 定时
        {
            // 未开始：注册开始定时（msg 6）。
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                        (TIMER_MESSAGE)6, infoList[i].m_start - curSec,
                                        infoList[i].m_type, user->get_acc_id());
        }
        else
        {
            // [语义轴·字段错位修复] ORIG 0x086afd5e `mov 0x4(%eax),%eax` + Ghidra C
            // `if (iVar3 <= *(int *)(iVar8 + 4))` —— 该处比较的是 **m_start(+0x04)**
            // （同函数 0x086b009b 的 m_checkTerm 分支才用 `mov 0x8(%eax)` = m_end）。
            // 旧码写 m_end 属字段错位：premium 已开始（curSec>=m_start）时，
            // ORIG 判「上次游玩 tick <= **开始**时刻」（本期尚未游玩）才发特权物品。
            if (user->getCurCharacLastPlayTick() <= infoList[i].m_start)
            {
                // 到期且上次游玩不晚于结束：发优势物品 + 金币。
                std::vector<std::pair<int, int> > items =
                    premium->GetAdvantageItem(infoList[i].m_type);
                for (size_t j = 0; j < items.size(); ++j)
                {
                    int itemIdx = items[j].first;
                    int count = items[j].second;
                    Inven_Item item;
                    CItem* ci = G_CDataManager()->find_item(itemIdx);
                    // [P140/P171 发明防御删除] ORIG 0x086afdf3(call find_item) → 0x086afdf8
                    // `mov %eax,-0x24(%ebp)` → 0x086afdfb 直接 `mov (%eax),%eax; add $0x8,%eax;
                    // mov (%eax),%edx; … call *%edx` —— **无 cmpl $0 判空分支**（全函数仅
                    // 0x086afef8 一处 `cmpl $0x0,-0x20` 属 insertRet）。旧码 `if (ci == 0)
                    // continue;` 是发明防御：多 2 条指令 + 改后续块序，且把 ORIG 的越界
                    // 解引用语义换成静默跳过。
                    ci->make_item(item);
                    item.m_addInfo = itemIdx;                  // +0x02
                    // [语义轴] ORIG 0x086afe15(call *%edx=make_item) → 0x086afe17 写 +0x02
                    // → 0x086afe20 `movzbl -0x97(%ebp),%eax`（读 +0x01）**其间无任何 +0x01
                    // 写入**（全函数 `-0x97(%ebp)` 仅此一处出现，grep 实证）⇒ 旧码
                    // `item.m_field1 = (unsigned char)(itemIdx >> 0x10);` 是发明写入，
                    // 会覆盖 make_item 设置的 ITEM_TYPE（+0x01）并改变 m_addInfo2 取值。
                    // [P47 试形记录·负结论] ORIG 0x086afe27 `cmp $0x1,%al; je L; <m_addInfo2=
                    // count>; jmp E; L: call GetSeedFromDate; …=eax`（真臂在跳转目标）；
                    // 取反三目 `(field1 != 1) ? count : GetSeedFromDate()` 实测 ndiff 59→59
                    // （4.4 规范化同形）⇒ 已回滚，保留原正极性三目。
                    item.m_addInfo2 = (item.m_field1 == 1)
                                      ? user->GetSeedFromDate() : count;  // +0x07
                    int insertRet = user->getCurCharacInvenW()->insertItemIntoInventory(
                        item, (eItemAddReason)0xe, true, true);
                    // [P222 正向包裹] ORIG 0x086afef8 `cmpl $0x0,-0x20(%ebp); js 086aff22`
                    // —— 条件为真跳 **后置** mail 块，fall-through 是 SendUpdateItemList；
                    // 旧码 `if (insertRet < 0) {mail} else {sendlist}` 被 4.4 反产 `jns <sendlist>`
                    // + mail 直落。交换极性（等价语义）以复现 ORIG 的块落位。
                    if (insertRet >= 0)
                    {
                        user->SendUpdateItemList(CUser::eSendTarget_1,
                                                 (ENUM_ITEMSPACE)0, insertRet);
                    }
                    else
                    {
                        CMailBoxHelper::ReqDBSendNewMail(
                            user, item, 0, (unsigned int)user->getCurCharacNo(), true, 0, 0);
                    }
                }
                // [语义轴] ORIG 0x086aff83 `cmpl $0x270f,0xc(%ebp)` —— 比较的是 **type 形参**
                // （非 vec[i].m_type）；旧码用 iType 局部属接收者错，若列表内 m_type 与
                // 形参 type 不一致会走错分支（Or：type==9999 时应整块跳过）。
                if ((int)type != 9999)
                {
                    // [P183/P0] ORIG 0x086aff90..0x086b0037 全段**无** invR/coin/curCoin/add
                    // 具名槽：getCurCharacInvenR 两处 fresh call（0x086aff96/0x086b000e）、
                    // GetCoin 结果直落 %ebx/%eax、m_premiumCount 每处 fresh 读
                    // `vec[i].m_type`（0x086affb2/0x086affef 两次 operator[]）。旧码 4 个具名
                    // 局部 ⇒ 少 2 次 call + 多 4 槽（帧 0xfc vs ORIG 0xec 的主因）。
                    user->getCurCharacInvenW()->SetCoin(
                        user->getCurCharacInvenR()->GetCoin()
                        + g_SPremiumInfo[infoList[i].m_type].m_premiumCount);
                    user->m_historyLog.CoinAdd(
                        (int)user->getCurCharacInvenR()->GetCoin(),
                        g_SPremiumInfo[infoList[i].m_type].m_premiumCount,
                        (eCoinAddReason)4);
                }
            }

            // 每日定时（仅在 premium 定义需每日检查时）。
            if (g_SPremiumInfo[infoList[i].m_type].m_checkTerm != 0 &&
                infoList[i].m_end > curSec)
            {
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                            (TIMER_MESSAGE)4, infoList[i].m_end - curSec,
                                            infoList[i].m_type, user->get_acc_id());
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
                                            infoList[i].m_type, user->get_acc_id());
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
    }   // if (user->IsPremiumUser())
}