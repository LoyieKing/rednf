// ---- change_trade_state @ 0x0852f134 ----
// 残差豁免（O=1096/N=1098，+2）：bool hack 汇聚岛与 4.4 折叠的 ±差异、
// [jump-threading] case2 cancel 成功/失败块序。float/double 阈值块按
// ORIG Ghidra 精度（第一块 float 0.7+0.01*r1，第二块 double 0.9）。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=824（O1088/N1086, gap 2）]
//   N 封顶（§6.2 三件套）：p167 112/112 CLEAR；gap 2；槽位集 N82 全帧平移
//   （PacketGuard -0xc0↔-0xd0、标量组 -0x120/-0x11c/-0x118 无对应）。
//   残差归因：N82（4.1 不按 lifetime 复用槽 vs 4.4 复用）+ [jump-threading]
//   case2 成功/失败块序 + bool 汇聚岛折叠。语义逐分支对照完整。
// [b28e-SweepTradeSpace verdict=DIFF ndiff=813（O1095/N1091, gap 4）]
//   真修：P0 删 `unsigned char oppErr` 具名局部——ORIG +0x2c0 为
//   `call GetOppositeErr; movzbl %al,%eax`（无槽往返）⇒ 实参内联；
//   ndiff 824→813。残余 813 归因：PacketGuard 槽差 0x0c（ORIG -0xc0 vs
//   我方 -0xcc）+ target/opp/me 槽序（ORIG -0x74/-0x70/-0x6c vs 我方
//   -0x78/-0x7c/-0x80；4.4 按首次使用序，P212 声明序不可翻）+ needCnt/
//   emptyCnt 数组槽差 0x10（-0xec vs -0xfc）+ case2/case3 块序（N82/P272）。
// [b28f-SweepCTradeSpace verdict=DIFF ndiff=1190（O1095/N1095, gap 0）]
//   真修四处（推翻 b28d「虚调用非真 bug」判词）：
//   ① **P282 真 bug：虚调用槽 0x14→0x10**。ORIG +0xa29/+0xaf9 为 `add $0x10;
//      call *`；用 ORIG 自身派生类 vtable 实证槽语义：_ZTV14CStackableItem
//      @0x08c96ea0 word[6]=0x08514a92=CStackableItem::IsAvatarItem（vptr+0x10）、
//      word[7]=0x08514a9c=IsCreatureItem（vptr+0x14）⇒ 原码调 **IsAvatarItem**，
//      旧重建误调 IsCreatureItem（运行时选中错误虚函数，hack 检测语义错）。
//   ② P222 负极性：ORIG +0x5f1 `call isEquipableItemType; xor $1; test; je`
//      ⇒ 原码把否定写在条件里（非装备走 check_item_exist 支）。
//   ③ P0 表达式源：ORIG 各处按 `m_pTraders[opp]->…` 逐点重算索引
//      （+0x5ef 段 `mov -0x70(%ebp),%edx; add $0x8,%edx; mov (%eax,%edx,4)`），
//      非读 target 具名槽。
//   ④ P222 嵌套 if：ORIG +0x8cb/+0x92b 三条件各自 `xor $1; test; je` 落共享
//      出口，4.4 对 `&&` 链折叠首条 xor ⇒ 拆嵌套 if（O1092→1095）。
//   残余归因：全帧槽图恒定平移 0x0c（PacketGuard -0xc0↔-0xcc、target
//   -0x74↔-0x80、needCnt -0xec↔-0xf8；4.1 首槽基 -0x4 vs 4.4 -0xc 属
//   N126 版本常数 + case3 两循环槽复用差，声明序反转实测无效=首次使用序）
//   + case3 块序（difflib 对齐漂移，N127）。p167 112/112 CLEAR（仅 C1/C2、
//   D1/D2 同址别名差，P93 豁免）。
// [K2 2026-09-11 复验] fresh .o：101 组 slot-pair 中 86 组为**恒定 +0x0c**
//   （PacketGuard -0xc0↔-0xcc、-0x74/-0x70/-0x6c↔-0x80/-0x7c/-0x78、trace
//   -0xb4↔-0xc0），与 N126「4.1 首槽基 -0x4 vs 4.4 -0xc」一致（帧 sub 0x13c 双侧
//   相同 ⇒ 非帧差）；声明序反转已多轮实测无效 ⇒ 维持 N126/N82 封顶。
bool CTradeSpace::change_trade_state(CUser* user, int state)   // SIGFIX: bool（ORIG 串）
{
    // b28f-SweepCTradeSpace：ORIG @0x0852f134；虚调用槽已按 P282 修正为 0x10。
    PacketGuard packet;  // ORIG 入口即构造（-0xc0），各分支复用
    // ORIG 槽深序 target(-0x74)/opp(-0x70)/me(-0x6c)；实测 4.4 按首次使用序
    // 分配（target 最浅 -0x78），声明序调整无效 ⇒ 该槽序差归 N82。
    CUser* target = 0;
    int opp;
    int me;
    if (m_pTraders[0] == user) {
        target = m_pTraders[1];
        opp = 1;
        me = 0;
    } else {
        target = m_pTraders[0];
        opp = 0;
        me = 1;
    }
    if (target == 0) {
        ((InterfacePacketBuf*)&packet)->clear();
        ((InterfacePacketBuf*)&packet)->put_header(1, 0x1a);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_byte(1);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        user->Send(packet);
        return 1;
    }
    switch (state) {
    case 2:  // 取消交易
        if (cancel_trade()) {
            G_CGameManager()->PutTradeSpace(this);
        } else {
            cMyTrace(__PRETTY_FUNCTION__, 0x9db, 5)(
                "change_trade_state, cancel_trade Failed 1!  %d %d",
                user->getCurCharacNo(), target->getCurCharacNo());
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1, packet);
        }
        {
            // 取消后清点：若槽位仍有剩余物品且双方在场 → 逐项打日志
            int count = 0;
            for (int trader = 0; trader <= 1; ++trader) {
                for (int slot = 3; slot <= 0x1a; ++slot) {
                    if (m_slotsData[trader][slot].m_item.m_addInfo != 0)
                        ++count;
                }
            }
            if (count > 0 && m_pTraders[0] != 0 && m_pTraders[1] != 0) {
                cMyTrace(__PRETTY_FUNCTION__, 0x9eb, 5)(
                    "TRADE CANCELED! BUT, ITEM REMAINED! %d Count , "
                    "Ch1 %d Ch2 %d",
                    count, m_pTraders[0]->get_charac_no(-1),
                    m_pTraders[1]->get_charac_no(-1));
                for (int trader = 0; trader <= 1; ++trader) {
                    if (m_pTraders[trader] != 0) {
                        for (int slot = 3; slot <= 0x1a; ++slot) {
                            // ORIG +0x2e6-0x3b1：add_info/addInfo/characNo
                            // 三独立链求值（R2L），串无句号
                            if (m_slotsData[trader][slot].m_item
                                    .m_addInfo != 0) {
                                cMyTrace(__PRETTY_FUNCTION__, 0x9f8, 5)(
                                    "Remained Item %d : %d x %d",
                                    m_pTraders[trader]->get_charac_no(-1),
                                    m_slotsData[trader][slot]
                                        .m_item.m_addInfo,
                                    m_slotsData[trader][slot]
                                        .m_item.get_add_info());
                            }
                        }
                    }
                }
            }
        }
        break;
    case 0:  // 解除锁定
        if (IsLocked()) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
        } else {
            m_nState[me] = 0;
            SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_0);
        }
        break;
    case 1:  // 锁定
        {
            unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x36);
            if (err != 0) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, (unsigned char)err);
                // P0：ORIG +0x2c0 `call GetOppositeErr; movzbl %al,%eax` 无槽往返
                // ⇒ 原码无 oppErr 具名局部，实参内联。
                target->SendCmdErrorPacket(
                    (ENUM_CMDPACKET)0x1a,
                    GlobalData::s_pSecuProtectionField->GetOppositeErr(err));
                if (err == 0x7b || err == 0x89) {
                    // 安全异常：给对端补发 0xb0 强退提示包
                    ((InterfacePacketBuf*)&packet)->clear();
                    ((InterfacePacketBuf*)&packet)->put_header(0, 0xb0);
                    ((InterfacePacketBuf*)&packet)->put_byte(1);
                    ((InterfacePacketBuf*)&packet)->finalize(true);
                    target->Send(packet);
                }
                return 0;
            }
            if (IsLocked()) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
                break;
            }
            // 对方背包空间校验（ORIG 内联：QuickEmptyCount + 分类型空格统计，
            // 非 checkTrade 的模拟插入）：
            // 遍历 me 的槽位，装备总是占格；非装备仅当对方背包中不存在该物品时占格。
            // 按 ITEM_TYPE(1..10) 累计需求，与对方各类型空格比较，缺口从快捷栏空格抵扣。
            int needCnt[0xb] = {0};
            int emptyCnt[0xb] = {0};
            // P0：ORIG 各处按 `m_pTraders[opp]->…` 表达式逐点重算索引
            // （+0x5ef 段 `mov -0x70(%ebp),%edx; add $0x8,%edx; mov (%eax,%edx,4)`），
            // 非读 target 具名槽 ⇒ 以表达式直写。
            int quickEmpty = m_pTraders[opp]->getCurCharacInvenR()->QuickEmptyCount();
            for (int slot = 3; slot <= 0x1a; ++slot) {
                // ORIG +0x5ef-0x6d7：链上直读；isEquipable 正判 je→占格块
                if (m_slotsData[me][slot].m_item.m_addInfo != 0) {
                    // P222：ORIG +0x5f1 为 `call; xor $1; test; je`（负极性）
                    // ⇒ 原码把否定写在条件里（非装备走 check_item_exist 支）。
                    if (!m_slotsData[me][slot].m_item.isEquipableItemType()) {
                        if (m_pTraders[opp]->getCurCharacInvenR()->check_item_exist(
                                m_slotsData[me][slot].m_item.m_addInfo) < 0)
                            needCnt[m_slotsData[me][slot].m_item.m_field1]++;
                    } else {
                        needCnt[m_slotsData[me][slot].m_item.m_field1]++;
                    }
                }
            }
            for (int type = 1; type <= 10; ++type) {
                emptyCnt[type] = m_pTraders[opp]->getCurCharacInvenR()->InvenEmptycount(
                    (Inven_Item::ITEM_TYPE)type);
                if (needCnt[type] > emptyCnt[type]) {
                    quickEmpty += emptyCnt[type] - needCnt[type];
                    if (quickEmpty < 0) {
                        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 4, packet);
                        return 0;
                    }
                }
            }
            m_nState[me] = 1;
            SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_1);
            if (m_nState[opp] == 1)
                _SetLock(1);
        }
        break;
    case 3:  // 完成交易
        {
            int goldMe = m_nMoney[me];
            int goldOpp = m_nMoney[opp];
            // P222：ORIG 逐条件 `xor $1; test; je` 落共享出口（+0x92b），
            // 4.4 对 `&&` 链会折叠首个条件的 xor ⇒ 拆嵌套 if。
            if (user != 0) {
                if (!user->GetReliablePerson()) {
                    if (!user->IsAvailableCurCharacTradeGoldDaily(goldMe)) {
                        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 0x46,
                                                 packet);
                        return 0;
                    }
                }
            }
            if (target != 0) {
                if (!target->GetReliablePerson()) {
                    if (!target->IsAvailableCurCharacTradeGoldDaily(goldOpp)) {
                        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 0x47,
                                                 packet);
                        return 0;
                    }
                }
            }
            if (m_nState[me] != 1) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
                return 0;
            }
            m_nState[me] = 3;
            if (m_nState[opp] == 3) {
                bool bOppHasLow = false;  // 对方提交了低稀有度(<=1)非头像物品
                bool bMeHasLow = false;   // 己方提交了低稀有度(<=1)非头像物品
                if (user != 0) {
                    for (int slot = 3; slot <= 0x1a; ++slot) {
                        if (m_slotsData[opp][slot].m_item.m_addInfo != 0) {
                            CItem* item = G_CDataManager()->find_item(
                                m_slotsData[opp][slot].m_item.m_addInfo);
                            // P282 vtable 实证：ORIG vptr+0x10 = CStackableItem::IsAvatarItem
                            // （_ZTV14CStackableItem @0x08c96ea0 word[6]=0x08514a92）、
                            // vptr+0x14 = IsCreatureItem（word[7]=0x08514a9c）
                            // ⇒ ORIG 此处调 IsAvatarItem，旧重建误调 IsCreatureItem。
                            if (item != 0 && !item->IsAvatarItem() &&
                                item->get_rarity() <= 1) {
                                bOppHasLow = true;
                                break;
                            }
                        }
                    }
                }
                if (target != 0) {
                    for (int slot = 3; slot <= 0x1a; ++slot) {
                        if (m_slotsData[me][slot].m_item.m_addInfo != 0) {
                            CItem* item = G_CDataManager()->find_item(
                                m_slotsData[me][slot].m_item.m_addInfo);
                            if (item != 0 && !item->IsAvatarItem() &&
                                item->get_rarity() <= 1) {
                                bMeHasLow = true;
                                break;
                            }
                        }
                    }
                }
                if (proceed_trade()) {
                    G_CGameManager()->PutTradeSpace(this);
                    if (user != 0)
                        user->IncrementTradeGold(goldMe, bOppHasLow);
                    if (target != 0)
                        target->IncrementTradeGold(goldOpp, bMeHasLow);
                } else {
                    cMyTrace(__PRETTY_FUNCTION__, 0xaed, 5)(
                        "change_trade_state, proceed_trade Failed2! %d %d",
                        user->getCurCharacNo(), target->getCurCharacNo());
                    m_nState[me] = 0;
                    m_nState[opp] = 0;
                    SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_0);
                    SendChangeTradeState(packet, target, user, ENUM_TRADESTATE_0);
                }
                // 防 hack：金币倒卖概率检测（ORIG 使用“金额大于浮点阈值”，不是相等判断）。
                unsigned short fatigueMe = user->getCurCharacUsedFatigue();
                unsigned short fatigueOpp = target->getCurCharacUsedFatigue();
                int r1 = get_rand_int(0x19);
                int r2 = get_rand_int(0x14);
                // ORIG 常量：_DAT_08c98630=0.01、_DAT_08c98638=0.7；
                // fucompp + 条件码表示 goldMe > threshold。
                int totalMoney =
                    user->getCurCharacInvenR()->get_money() + goldMe;
                // ORIG Ghidra：float 精度比较 + bool 汇聚岛（mov$1/$0;test al）
                bool hack;
                if ((float)goldMe <=
                        (0.7f + 0.01f * (float)r1) * (float)totalMoney ||
                    goldMe < 10000)
                    hack = false;
                else
                    hack = true;
                if (hack) {
                    int etc = user->SetETC(0x2c0, target->get_acc_id());
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0x2c0, 1,
                        (unsigned)etc, 0);
                    if (fatigueMe >= r2 + 0x78) {
                        etc = user->SetETC(0x2bd, target->get_acc_id());
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0x2bd, 1,
                            (unsigned)etc, 0);
                    }
                    etc = target->SetETC(0x2c1, user->get_acc_id());
                    target->getHackAnalyzer()->addServerHackCnt(
                        target, (WongWork::ENUM_HACKTYPE)0x2c1, 1,
                        (unsigned)etc, 0);
                }
                // ORIG 常量：_DAT_08c98640=0.9；同样是严格大于阈值。
                totalMoney =
                    target->getCurCharacInvenR()->get_money() + goldOpp;
                bool hack2;
                if ((double)goldOpp <=
                        0.9 * (double)totalMoney ||
                    goldOpp < 10000)
                    hack2 = false;
                else
                    hack2 = true;
                if (hack2) {
                    int etc = target->SetETC(0x2c0, user->get_acc_id());
                    target->getHackAnalyzer()->addServerHackCnt(
                        target, (WongWork::ENUM_HACKTYPE)0x2c0, 1,
                        (unsigned)etc, 0);
                    if (fatigueOpp > 0x77) {
                        etc = target->SetETC(0x2bd, user->get_acc_id());
                        target->getHackAnalyzer()->addServerHackCnt(
                            target, (WongWork::ENUM_HACKTYPE)0x2bd, 1,
                            (unsigned)etc, 0);
                    }
                    etc = user->SetETC(0x2c1, target->get_acc_id());
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0x2c1, 1,
                        (unsigned)etc, 0);
                }
            }
        }
        break;
    case 4:  // 双方确认后最终完成：解锁并清状态
        if (IsLocked()) {
            m_nState[me] = 0;
            m_nState[opp] = 0;
            SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_0);
            SendChangeTradeState(packet, target, user, ENUM_TRADESTATE_0);
            _SetLock(false);
        } else {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
        }
        break;
    default:
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1, packet);
        return 1;
    }
    return 0;
}