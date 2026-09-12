// ----------------------------------------------------------------------------
// CUser::increase_status
// ----------------------------------------------------------------------------
// CUser::increase_status —— ORIG T 0x086657fc _ZN5CUser15increase_statusEs
// 报告 docs/class_func_reports/CUser/increase_status.md 全文转写（汇编+C）：
//   GetInvenSlot 取槽物品 → itemIdx 判定表设定 (type,value) → 公共门槛段 →
//   find_item/GetUsablePvPRank/GetItemType/delete_item → switch(type) 动作 →
//   公共回包 + m_chState 5/8 队伍战房广播。
// ----------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x086657fc：p167=M0/E5:_ZN15CUserCharacInfo23getCurCharacAddInfoRefWEv，gap=69；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086657fc：N82 封顶——帧/槽差（p167 M0/E5，gap 69）。
// [b28d-BigCUserStat 2026-09-09] 判定表 if-else 链 → switch（P251/P261/P286/P324）：
// 4.4.7 对 switch(unsigned 局部) 复现 ORIG 4.1 的 `mov itemIdx; cmp $K,%eax; je/ja`
// 二分决策树（探针 /tmp/sw_probe.cpp 逐条同形；if-else 链恒退化 cmpl $K,mem+jne，
// 旧判词的「跨版本 lowering 不可翻」据此作废）。同时按 ORIG 汇编补 4 处体尾
// goto L08666990（0x08665d83 / 0x08665fce / 0x08666420 / 0x0866698c 实证；原链式
// else 会误落 L08666751 默认体 = 语义偏差）。itemIdx 用纯 register 存储类提示
// （P231 合法，非 asm 绑定）省 3 条 store/load。
// 读数：2757 vs ORIG 2703（gap -54，与改前持平未劣化）；structdiff 2068→1911、
// ndiff 2388→2287；TU 四数 427/862/0/59 不变；p167 CLEAR（418/418，C1↔C2/D1↔D2 镜像豁免）。
// 残余：mov +46 / jmp +15（尾部 switch(type) 段具名局部与跳转岛）、帧 0x20c vs 0x39c
// （N82/N126 族，P321 T5 不再动类型）。
// [A06 2026-09-10 必配对律消费侧获益 + 归因1分诊] 本件内 2 处 `get_give_skill` 消费点
// （9380/9462）在 D06 的 `CCharacter::get_give_skill` char→bool 落地并与我方 `bool got`
// 成对后：2757→**2753** 条（gap −54→**−50**），源码零改动、identical 净账 0。
// 另：本件 9 处 ORIG `setne` 经分诊（Main C05④ 四步）全为「call 结果 → 比较 → setne」
// = **归因 1（callee 返回类型差）**，非前端折叠；相关 callee 非本 TU，未单侧动（spec 已报 Main）。
// [CoBatchB 2026-09-11 thunk 岛结论] 本件 gap 主项 = 4.4 为「多 case 共享同一 goto 目标」
// 的**每个 case** 各发 `jmp body; nop` 两指令 thunk，4.1 则把空 case 并入目标 BB 直发
// `je body`（SubB 实测定量：我方 ≥38 个 thunk 对、ORIG 仅 5 个）。已落地 1 组
// （`case 0x4b0/0x4b1/0x4b2/0x2920915c` 体移入 switch、标签贴体）实测 **2790→2787**；
// 其余组（最大 = `case 3/0x1c/0x6d/0xa1/0xa2/0x394/0x3c0/0x3c3/0x4b6/0x4e5/0x28982c
// → L0866607b` 11 case，预期 ≈ −9）需逐组同法搬体，未做。另 [SubB] 候选实证：
// `spBefore/spAfter` 去初值 **−2 有效（已落地）**；default 段三处 `vtmp`→`&value` 直传
// 与 `ok54` 去具名 **中性（已回滚）**。帧 0x20c vs ORIG 0x39c（N82/N126）与 9 处
// `setne`（归因1，callee 非本 TU）仍为封顶项。
// [M8BigTURest 2026-09-11 03:31] thunk 岛搬体续做（承 SubB ① 4-case 组，本轮再搬 5 组）：
//   4.4 对「case 组: goto L08xxxxx」为**每个 case** 各发 `jmp body; nop`（thunk 对），
//   4.1 则把空 case 并入目标 BB 直发 `je body`。按 SubB 手法把共享体搬入 switch
//   （体贴 case 标签、删 goto 与远标签）：L0866607b(11 case)/L08665c97(6)/L08665d24(2)/
//   L08665e38(2)/L08666320(2)。读数 our **2787→2566**（ORIG 2717），strict ndiff
//   （autojunk=False、clean 口径）**2317→2162**；TU 四数不变（仍 DIFF 45）。
//   default 体（L08666751）同法搬入 = **零效果**（2566/2162 均不变）⇒ 已回滚。
//   残余：4.1 的 `sub $0xa1` 范围归约 + 二分树（orig[61:64] 仅 3 条）vs 4.4 顺序比较链
//   （our[59:158] 99 条）；另有 280/307 条级块置换（orig[1410:1692] vs our[2087:2394]）。
void CUser::increase_status(short slotNo)
{
    PacketGuard packet;

    if (CheckInTrade() != 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
        return;                                              // L086686d4
    }

    // ORIG 0x086657fc 区内无 _ZN10Inven_ItemC1Ev 调用点，仅 1×GetInvenSlot：
    // 按值返回 sret 直写拷贝初始化，无默认构造。
    Inven_Item slotItem = getCurCharacInvenR()->GetInvenSlot(1, slotNo);
    char type = -1;                                          // -0xd1
    unsigned int value = 0;                                  // -0xd8
    register unsigned int itemIdx = (unsigned int)slotItem.m_addInfo;

    // ---- itemIdx → (type,value) 判定表 ----
    // ORIG 0x0866589d 起是 4.1 的 switch 二分决策树（单次载入 itemIdx 后
    // `cmp $K,%eax` + je/ja）；4.4 同源形（/tmp/sw_probe.cpp 4.4.7 探针实证），
    // if-else 链恒退化为 `cmpl $K,mem`+jne（P251/P261/P286）。
    // case 书写序 = ORIG 叶块地址序（P324）；体尾 goto L08666990 按 ORIG 汇编补齐。
    switch (itemIdx)
    {
    case 0x292091ad: type = 0x19; value = 1; goto L08666990;      // 0x08665ba5
    case 0x407:  type = 0; value = 5; goto L08666990;             // 0x08665bbb
    case 0x40e:  type = 0; value = 0x14; goto L08666990;          // 0x08665bd1
    case 0x4b4:  type = 0x11; value = 1; goto L08666990;          // 0x08665be7
    case 0x4b5:  type = 0x11; value = 5; goto L08666990;          // 0x08665bfd
    case 0x40a:  type = 1; value = 100; goto L08666990;           // 0x08665c13
    case 0x40b:  type = 1; value = 1000; goto L08666990;          // 0x08665c29
    case 0x40c:  type = 1; value = 10000; goto L08666990;         // 0x08665c3f
    case 0x40d:  type = 1; value = 100000; goto L08666990;        // 0x08665c55
    case 0x4cf:  type = 1; value = 1000000; goto L08666990;       // 0x08665c6b
    case 0x4d0:  type = 1; value = 10000000; goto L08666990;      // 0x08665c81
    // [CUserOnlyOrig-Lane 2026-09-11 P167] ORIG 0x08665b78 `je 08665c97` = case 0x28d2bc
    // （与 0x1f71/0x204b..0x204f 共享同体）——旧形漏此 case ⇒ 该值落 default（真语义缺口）。
    case 0x1f71: case 0x204b: case 0x204c: case 0x204e: case 0x204f: case 0x28d2bc:
    {
            bool reject = false;
            if (!CalLevelUpItemCheck(itemIdx))
        {
                reject = false;                              // asm: 不报错，走公共段
        }
        else
        {
                int valueTmp = (int)value;
                reject = (CalLevelUpItemState(type, valueTmp, 0x13, 0x3b) != 1);
                value = (unsigned int)valueTmp;
        }
            if (reject)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
    }
                    goto L08666990;
    // [CUserOnlyOrig-Lane 2026-09-11 P167 真缺口修复] ORIG 0x08665b5b (`je 08665d24`,
    // case 0x989a13) / 0x08665b9a (同目标, case 0x292090e1) ⇒ 本块是二者共享体
    // （CalLevelUpItemState(type,value,1,0x54) 失败 → SendCmdErrorPacket(0x20,0x13)）；
    // thunk 搬体后标签丢失 ⇒ DCE。
    case 0x989a13: case 0x292090e1:
    {
        // [SubB ①-d′ 2026-09-11 二分测试·暂回滚] 直传 &value + 去 ok54 具名槽
        int valueTmp = (int)value;
        bool ok54 = (CalLevelUpItemState(type, valueTmp, 1, 0x54) == 1);
        value = (unsigned int)valueTmp;
        if (!ok54)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
    }
                    goto L08666990;
    case 0x40f:  type = 4; value = 0x32; goto L08666990;          // 0x08665d88
    case 0x410:  type = 6; value = 0x32; goto L08666990;          // 0x08665d9e
    case 0x411:  type = 5; value = 0x32; goto L08666990;          // 0x08665db4
    case 0x412:  type = 7; value = 0x32; goto L08666990;          // 0x08665dca
    case 0x413:  type = 2; value = 0xfa; goto L08666990;          // 0x08665de0
    case 0x414:  type = 3; value = 0xfa; goto L08666990;          // 0x08665df6
    case 0x415:  type = 8; value = 10; goto L08666990;            // 0x08665e0c
    case 0x416:  type = 9; value = 10; goto L08666990;            // 0x08665e22
    // [CUserOnlyOrig-Lane 2026-09-11 P167 真缺口修复] ORIG 0x086658d4 (`je 08665e38`,
    // case 0xc9) / 0x08665afe (同目标, case 0x28bdb8) ⇒ 本块是二者共享体；
    // M8BigTURest「thunk 搬体」后标签丢失 ⇒ 块不可达被 4.4 前端 DCE
    // （p167 MISSING: GetRepeatEvent×1 / clear×2 / Send×2 / getCurCharacTotalFatigue×1）。
    case 0xc9: case 0x28bdb8:
    {
        CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0xf);
        if ((repeatEvent == 0) || (!repeatEvent->IsEventing(0)))
        {
            packet.clear();
            packet.put_header(1, 0x20);
            packet.put_byte(0);
            packet.put_byte(0x13);
            packet.finalize(true);
            Send(packet);
            return;
        }
        short totalFatigue = (short)getCurCharacTotalFatigue();
        if (totalFatigue == 0)
        {
            packet.clear();
            packet.put_header(1, 0x20);
            packet.put_byte(0);
            packet.put_byte(0x43);
            packet.finalize(true);
            Send(packet);
            return;
        }
        type = 0x0e; value = 0;
    }
                goto L08666990;
    case 0xc84:                                                   // 0x08665fd3
                        if (get_charac_guildkey() == 0)
        {
                            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x40, packet);
            return;
        }
                        type = 0xb; value = 10;
        goto L08666990;
    case 0x1bc1:                                                  // 0x08666027
                    if (get_charac_guildkey() == 0)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x40, packet);
            return;
        }
                    type = 0xb; value = 0x32;
        goto L08666990;
    case 3: case 0x1c: case 0x6d: case 0xa1: case 0xa2:
    case 0x394: case 0x3c0: case 0x3c3: case 0x4b6: case 0x4e5: case 0x28982c:
    {
                    unsigned char errSecu =
                        (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                            this, (SECURITY_PROTCTION)0x2c);
                    if (errSecu != 0)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
                        return;  // L086686d4
        }
                    if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
            return;
        }
                    if (CheckItemLock(1, (int)slotNo) != 0)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd5, packet);
            return;
        }
                    if (!WongWork::CSkillChanger::CheckCondition(this))
        {
                        setCurCharacExp(G_CDataManager()->get_level_exp(get_charac_level()));
        }
                    if ((itemIdx == 0x4b6) || (itemIdx == 0x4e5))
        {
                        if (get_charac_level() < 0x32) return;    // asm 直跳 L086686d4
                        if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd8, packet);
                return;
            }
                        value = 1; type = 0x12;
        }
                    else if (itemIdx == 0x394) { value = 1; type = 0xa; }
                    else if (itemIdx == 0x3c0)
        {
                        value = 1; type = 0x18;
                        CQuestShop* questShop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
                        if (questShop != 0)
            {
                            questShop->clearQP(this);
                            questShop->sendCharacQp(this, '\0');
            }
        }
                    else if (itemIdx == 0x3c3) { value = 1; type = 0xa; }
                    else if (itemIdx == 0x28982c) { value = 1; type = 0xa; }
                    else { value = 0; type = 0xa; }
        goto L08666990;
    }
    // [CUserOnlyOrig-Lane 2026-09-11 P167 真缺口修复] ORIG 0x08665b10 (`je 08666320`,
    // case 0x28974b) / 0x08665b35 (同目标, case 0x289788) ⇒ 本块是二者共享体
    // （IsPvPSkilTreeChannel()!=1 → 0x17；Check → errSecu；CheckCondition → 1；
    // value=0/type=0x16，0x289788 时 value=1）；thunk 搬体后标签丢失 ⇒ DCE。
    case 0x28974b: case 0x289788:
    {
                if (G_GameWorld()->IsPvPSkilTreeChannel() != 1)
        {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
            return;
        }
            unsigned char errSecu =
                (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                    this, (SECURITY_PROTCTION)0x2c);
            if (errSecu != 0)
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
            return;
        }
            if (!WongWork::CSkillChanger::CheckCondition(this))
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 1, packet);
            return;
        }
            value = 0; type = 0x16;
            if (itemIdx == 0x289788) value = 1;
    }
        goto L08666990;
    case 0x28974c:                                                // 0x08666425
            if (G_GameWorld()->IsPvPSkilTreeChannel() != 1)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
            return;
        }
        {
            unsigned char errSecu =
                (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                    this, (SECURITY_PROTCTION)0x2c);
            if (errSecu != 0)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
                return;
            }
        }
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
        {
                            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd8, packet);
            return;
        }
                value = 0; type = 0x17;
        goto L08666990;
    case 0x1bbd:                                                  // 0x08666517
                if (get_state() != 3)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
                type = 0xd; value = 8;
        goto L08666990;
    case 0x1c82:                                                  // 0x0866656c
                if (get_state() != 3)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
                type = 0xd; value = 2;
        goto L08666990;
    case 0x1c0c: type = 0xd; value = 1; goto L08666990;           // 0x086665c1
    case 0x1c0d: type = 0xd; value = 5; goto L08666990;           // 0x086665d7
    case 0x2a: case 0x28d14d: type = 0x15; value = 1; goto L08666990;  // 0x086665ed
    case 0x4df:  type = 0x13; value = 10; goto L08666990;         // 0x08666603
    case 0x1f16: type = 0x14; value = 0; goto L08666990;          // 0x08666619
    // [SubB ③ 2026-09-11 thunk 岛实验] 4 个 case 的共享体直接贴 case（旧 `goto L0866662f`
    // 令 4.4 为每个 case 各发 `jmp body; nop` = 8 条；ORIG 0x0866662f 处 4.1 把空 case
    // 并入目标 BB、直接 `je body`）。
    case 0x4b0: case 0x4b1: case 0x4b2: case 0x2920915c:
    {
            // [CUserOnlyOrig-Lane 2026-09-11 真语义修复] ORIG 0x08666635-0x0866663a：
            // `cmp $0x2920915c,%eax; jne 08666678` ⇒ 块内**再查一次 itemIdx**，
            // 仅 0x2920915c 走 IsGuildMaster 错误包；0x4b0/0x4b1/0x4b2 非会长时**继续**
            // 后续流程（旧形令三者误报 0xdb，真语义 bug）。
            if ((itemIdx == 0x2920915c) && (!IsGuildMaster()))
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xdb, packet);
            return;
        }
            STGuildDBInfoOnly* guildDB = (STGuildDBInfoOnly*)GetGuildDBInfo();
            // [R7a08 2026-09-12 §0.1 真语义修复] ORIG 0x08666e5 `mov -0x94(%ebp),%eax;
            // mov 0x4(%eax),%eax` ⇒ 原码把返回值当**指针**读 *(p+4) 再比较；ORIG 实现
            // 0x08979648 尾为 `lea 0x5c(%edx,%eax,1),%eax`（返回 m_levelUpCost+(n-1)*8
            // 的地址）或 0。旧形 `int needFund` 直接比指针值(低 32 位)，运行时错误路径
            // 与 ORIG 不同（同 TU CheckGuildLevelUp.inc 已用 (unsigned int*)+视图口径）。
            struct GuildLevelUpParamView { unsigned int m_head; unsigned int m_value; };
            GuildLevelUpParamView* needFund = (GuildLevelUpParamView*)
                G_CDataManager()->m_guildParameter.getGuildLevelUpParam(guildDB->m_fundLevel + 1);
            if (needFund == 0)
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd2, packet);
            return;
        }
            if (guildDB->m_fundExp >= needFund->m_value)  // ORIG +0x29 vs *(p+4)，无符号
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd2, packet);
            return;
        }
            type = 0x10;
            value = G_CDataManager()->m_guildParameter.getGuildExpBook(guildDB->m_fundLevel + 1);
        goto L08666990;
    }
    default:
            goto L08666751;
    }







L08666751:
    // ORIG:每次使用都重新调用 G_CDataManager()（无缓存，4 个静态站点）。
    {
        if (itemIdx == (unsigned int)G_CDataManager()->m_growthCapsule.m_capsuleId)
        {
            CItem* itemA = G_CDataManager()->find_item(
                G_CDataManager()->m_growthCapsule.m_capsuleId);
            if (itemA == 0)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (!itemA->is_stackable())
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (get_charac_level() < itemA->getUsableLevel())
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (get_charac_level() > 0x47)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            type = 1; value = 0;
            int vtmp = 0;
            // [SubB ①-b 2026-09-11 二分测试·暂回滚]
            if (!itemA->GetIncreaseStatusIntData(0, vtmp))
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            value = (unsigned int)vtmp;
        }
        else
        {
            CItem* itemB = G_CDataManager()->find_item(itemIdx);
            if ((itemB != 0) && (itemB->GetIncreaseStatusType() != 0) &&
                (itemB->GetIncreaseStatusType() == 1))
            {
                // [SubB ①-c 2026-09-11 二分测试·暂回滚]
                int vtmp = 0;
                itemB->GetIncreaseStatusIntData(0, vtmp);
                value = (unsigned int)vtmp;
                type = 1;
            }
        }
    }


L08666990:
    if (type == -1)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
        return;                                              // L086686d4
    }
    if (type == 0x0f)
    {
        if (get_state() != 3)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
        if ((int)(getCurCharacFatigue() & 0xffff) < (int)value)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x5f, packet);
            return;
        }
    }
    if (((int)type == 0) || ((int)type == 0x11) || ((int)type == 1))
    {
        if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
    }

    CItem* actionItem = G_CDataManager()->find_item(itemIdx);
    if (actionItem == 0) return;                             // L086686d4
    if (actionItem->GetUsablePvPRank() > get_pvp_grade())
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 1, packet);
        return;
    }
    if (actionItem->GetItemType() != 0x15)
    {
        CInventory* invenW = getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, slotNo, 1, (eItemDelReason)10, true))
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
            return;
        }
    }

    // [AuditFix 2026-09-12] ORIG 0x08666bc1..0x08666bc8：`movzbl -0xd1(%ebp); movsbl %al,%eax`
    // ⇒ 原码是 `switch (type)`（char 为 signed，T1 直证）；旧 `(unsigned char)` cast 使
    // 我方发 `movzbl %al`（unsigned）⇒ 符号性不符。type 值域 {0..0x19, -1}，两形语义等价。
    switch (type)
    {
    case 0:                                                  // SP（asm +0x1435）
    {
        gain_sp(value);
        history_log_sp(-1, value, (eSPAddReason)1);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(0);
        packet.put_short(0);
        packet.finalize(true);
        Send(packet);
        return;                                              // asm jmp L086686d4
    }
    case 1:                                                  // EXP 类（asm +0x167f）
    {
        int bonusA = 0;                                      // -0xdc
        int bonusB = 0;                                      // -0xe0
        // [SubB ①-a 2026-09-11] ORIG 0x08666e7b/0x08666e85 只初始化 -0xdc/-0xe0（2 条
        // movl $0x0）；-0xe4/-0xe8 首现即 0x08666ee8/0x08666ef2 的 lea（纯出参，无初值）。
        int spBefore;                                        // -0xe4
        int spAfter;                                         // -0xe8
        if (get_charac_level() > 0x54) value = 0;
        if (itemIdx == (unsigned int)G_CDataManager()->m_growthCapsule.m_capsuleId)
            rewardExp(value, bonusA, bonusB, spBefore, spAfter, (::eExpAddReason)3, true);
        else
            rewardExp(value, bonusA, bonusB, spBefore, spAfter, (::eExpAddReason)0, true);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(spAfter - spBefore);
        packet.put_short(bonusB);
        packet.finalize(true);
        Send(packet);
        return;
    }
    case 2:                                                  // AddInfoRef +0x00（int）
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field00 += value;
        break;
    }
    case 3:                                                  // +0x04（int）
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field04 += value;
        break;
    }
    case 4:                                                  // +0x08（short）
    {
        // [b28d-solo-CUserOnlyOrig P167 真形] ORIG 每 case 仅 1 次 getCurCharacAddInfoRefW
        // （8667065..866714d 共 8 处）；`f = f + v` 两读形多 5 个调用点 ⇒ 改 `+=`。
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field08 += static_cast<short>(value);
        break;
    }
    case 5:                                                  // +0x0a
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field0a += static_cast<short>(value);
        break;
    }
    case 6:                                                  // +0x0c
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field0c += static_cast<short>(value);
        break;
    }
    case 7:                                                  // +0x0e
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field0e += static_cast<short>(value);
        break;
    }
    case 8:                                                  // +0x42（int）
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field42 += value;
        break;
    }
    case 9:                                                  // 四属性循环 (+8)*2
    {
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field10[i] += static_cast<short>(value);
        }
        break;
    }
    case 0xa:
    case 0x18:                                               // 转职/成长洗点重建技能树
    {
        bool ok = false;
        if (value == 1)
        {
            SkillSlot* skillSlot = (SkillSlot*)getCurCharacSkillW();
            skillSlot->clear_all_skills_both();
            skillSlot = (SkillSlot*)getCurCharacSkillW();
            skillSlot->set_parent(this);
            addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot,
                                      get_charac_job());
            addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot2,
                                      get_charac_job());
            std::vector<std::pair<int,int> > giveVec;
            std::pair<int,int> pr;
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int job2 = get_charac_job();
            bool got = G_CDataManager()->m_characList->m_characs[job2].
                           get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
            if (got)
            {
                // [A06 2026-09-10 轴B 试行(P196 已前置：ORIG 对应循环尾为 setg/setl 有符号序)]
                // 旧 `unsigned int i` ⇒ 4.4 `cmp -0xb4(%ebp),%eax; seta`（无符号）；
                // 改 int + `(int)size()` 复现 ORIG 的有符号序。
                for (int i = 0; i < static_cast<int>(giveVec.size()); ++i)
                {
                    int j = get_charac_job();
                    pr = giveVec[i];
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second,
                                                                       (ENUM_SKILL_TREE_KIND)0);
                    j = get_charac_job();
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second,
                                                                       (ENUM_SKILL_TREE_KIND)1);
                }
                STExpertJobScript* expert = (STExpertJobScript*)
                    G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
                if (expert != 0)
                {
                    // [A06 2026-09-10 轴B 试行记录·已回退] 本处（含下面 giveVec/第二 expert 循环共 3 处）
                    // 改 int 实测 delta 2282→2287（+5）反劣 ⇒ 该 3 处 ORIG 侧非 setg（仅 9391 处命中）⇒ 维持 unsigned。
                    for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
                    {
                        int lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        int j = get_charac_job();
                        std::pair<int,int>& pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)0);
                        lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        j = get_charac_job();
                        pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                WongWork::CSkillChanger changer;
                changer.SkillInitialize(this, 0, false);
                send_skill_info();
                CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    if (lethe != 0)
                    {
                        lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)0);
                        lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)0);
                    lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)1);
                }
                ok = true;
            }
            else
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x4aee,
                    "User %s - CUser::set_grow_type %d", getCurCharacName(), (int)type);
                ok = false;
            }
        }
        else
        {
            ENUM_SKILL_TREE_KIND treeKind =
                (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(treeKind);
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            bool firstTree = ((GetCurCharacSkillTreeIndex() == -1) ||
                              (GetCurCharacSkillTreeIndex() == 0));
            if (firstTree)
            {
                addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot,
                                          get_charac_job());
            }
            else
            {
                addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot2,
                                          get_charac_job());
            }
            std::vector<std::pair<int,int> > giveVec;
            std::pair<int,int> pr;
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int job2 = get_charac_job();
            bool got = G_CDataManager()->m_characList->m_characs[job2].
                           get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
            if (got)
            {
                // [R7a08 2026-09-12 §0.1 真语义修复·#27 符号性] ORIG 0x08667d6c
                // `cmp -0x2c(%ebp),%eax; setg`（有符号）⇒ 原码为 int 计数的有符号序。
                // 旧 `unsigned int i` 生 `seta`（无符号）⇒ 索引/长度 ≥0x80000000 时分叉。
                for (int i = 0; i < static_cast<int>(giveVec.size()); ++i)
                {
                    ENUM_SKILL_TREE_KIND kindI =
                        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
                    int j = get_charac_job();
                    pr = giveVec[i];
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second, kindI);
                }
                STExpertJobScript* expert = (STExpertJobScript*)
                    G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
                if (expert != 0)
                {
                    // [A06 2026-09-10 轴B 试行记录·已回退]（同上：本处反劣）
                    for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
                    {
                        int lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        int j = get_charac_job();
                        std::pair<int,int>& pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)0);
                        lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        j = get_charac_job();
                        pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                WongWork::CSkillChanger changer;
                int initKind = ((GetCurCharacSkillTreeIndex() == -1) ||
                                (GetCurCharacSkillTreeIndex() == 0)) ? 1 : 2;
                changer.SkillInitialize(this, initKind, false);
                send_skill_info();
                CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    if (lethe != 0)
                    {
                        lethe->UpdateBackupSkillFlag(
                            this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                    }
                }
                else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    lethe->InitLetheSkill(
                        this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                }
                ok = true;
            }
            else
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x4b48,
                    "User %s - CUser::set_grow_type %d", getCurCharacName(), (int)type);
                ok = false;
            }
        }
        if (!ok) return;                                     // 失败：仅析构 packet 返回
        break;
    }
    case 0xb:                                                // 公会经验（+0xc92）
    {
        if (get_charac_guildkey() != 0) add_guild_exp(value);
        break;
    }
    case 0xc:                                                // stamina（+0xcb3）
    {
        setCurCharacStamina((unsigned char)value);
        break;
    }
    case 0xd:                                                // event coin（+0xcd9）
    {
        int coinNow = getCurCharacInvenR()->GetEventCoin();
        getCurCharacInvenW()->SetEventCoin(coinNow + value);
        m_historyLog.EventCoinAdd(getCurCharacInvenR()->GetEventCoin(), value,
                                  (eCoinAddReason)2);
        SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 1);
        break;
    }
    case 0x0e:                                               // fatigue 回复（+0xd43）
    {
        RecoverFatigue(value);
        SendFatigue();
        break;
    }
    case 0x0f:                                               // fatigue 扣减（+0xd55）
    {
        int remain = (int)(getCurCharacFatigue() & 0xffff) - (int)value;
        // ORIG:显式 std::max<int>(remain, 0) 调用（_ZSt3maxIiERKT_S2_S2_ 0x8668143）。
        remain = std::max(remain, 0);
        setCurCharacFatigue(remain);
        SendFatigue();
        break;
    }
    case 0x10:                                               // 公会经验上报（+0xdd8）
    {
        if (get_charac_guildkey() != 0)
        {
            unsigned int characNo = get_charac_no(-1);
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
            proxy->SendIncreaseGuildExp((unsigned int)get_charac_guildkey(), characNo,
                                        value, 1);
        }
        break;
    }
    case 0x11:                                               // SFP（+0xe64）
    {
        gain_sfp(value);
        history_log_sfp(-1, value, (eSPAddReason)1);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(0);
        packet.put_short(0);
        packet.finalize(true);
        Send(packet);
        return;
    }
    case 0x12:                                               // SFP 技能树重建（+0xf12 区）
    {
        ((SkillSlot*)getCurCharacSkillW())->clear_sfp_skills(
            (char)get_charac_job(),
            (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
        ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
        // ORIG:initKind 判定重新调用两次（不缓存 treeKind）。
        int initKind =
            ((GetCurCharacSkillTreeIndex() == (ENUM_SKILL_TREE_KIND)-1) ||
             (GetCurCharacSkillTreeIndex() == (ENUM_SKILL_TREE_KIND)0))
                ? 3
                : 4;
        WongWork::CSkillChanger changer;
        changer.SkillInitialize(this, initKind, false);
        send_skill_info();
        break;
    }
    case 0x13:                                               // power war point（+0x1180）
    {
        IncreasePowerWarPoint((short)value);
        break;
    }
    case 0x14:                                               // GM 状态校正（+0x119c）
    {
        adjust_charac_stat();
        if (m_chState == 3)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            Send(packet);
        }
        break;
    }
    case 0x15:                                               // coin（+0x125b）
    {
        int coinNow = getCurCharacInvenR()->GetCoin();
        getCurCharacInvenW()->SetCoin(coinNow + value);
        m_historyLog.CoinAdd(getCurCharacInvenR()->GetCoin(), value, (eCoinAddReason)3);
        SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 1);
        break;
    }
    case 0x16:                                               // PvP 技能树（+0x1303）
    {
        if (value == 1)
        {
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            givePvPSkillTree(0, true, 3);
            int pvpGrade = get_pvp_grade();
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                get_charac_job(), (int)firstGrow, (int)secondGrow, pvpGrade, false);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(point,
                                                                       (ENUM_SKILL_TREE_KIND)0);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(point,
                                                                       (ENUM_SKILL_TREE_KIND)1);
            send_skill_info();
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                if (lethe != 0)
                {
                    lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)0);
                    lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)1);
                }
            }
            else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)0);
                lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)1);
            }
        }
        else
        {
            ENUM_SKILL_TREE_KIND treeKind =
                (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(treeKind);
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            int initKind = ((GetCurCharacSkillTreeIndex() == -1) ||
                            (GetCurCharacSkillTreeIndex() == 0)) ? 1 : 2;
            givePvPSkillTree(0, true, initKind);
            int pvpGrade = get_pvp_grade();
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                get_charac_job(), (int)firstGrow, (int)secondGrow, pvpGrade, false);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
                point, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
            send_skill_info();
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                if (lethe != 0)
                {
                    lethe->UpdateBackupSkillFlag(
                        this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                }
            }
            else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                lethe->InitLetheSkill(
                    this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
            }
        }
        break;
    }
    case 0x17:                                               // one day lethe（+0x1800）
    {
        CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
        // ORIG:单表达式链(实参 treeIdx/charNo/name 先于 cMyTrace 构造求值)。
        cMyTrace(__PRETTY_FUNCTION__, 0x4c0a, 0)(
            "ONE_DAY_LETHE : BUY_SKILL_CONFIRM_TICKET, char(%s), char_no(%d), style(%d)",
            getCurCharacName(), getCurCharacNo(), GetCurCharacSkillTreeIndex());
        if (lethe != 0)
        {
            lethe->ConfirmSkillReq(this);
        }
        break;
    }
    default:
        break;
    case 0x19:                                               // avatar coin（+0x13e1）
    {
        AvatarCoin* avatarCoin =
            &reinterpret_cast<CInventory*>(getCurCharacInvenW())->m_avatarCoin;  // +0x658
        avatarCoin->Add(value);
        AvatarCoin::SaveToDB(this);
        AvatarCoin::SendSyncPacket(this);
        AvatarCoin::HistoryLog::AddLog(this, value);
        break;
    }
    }

L_common_tail_6751_end_return:
    // ---- 公共回包与队伍/战房广播（ORIG +0x2c39 尾段）----
    packet.clear();
    packet.put_header(1, 0x20);
    packet.put_byte(1);
    packet.put_short(slotNo);
    packet.put_byte(type);
    packet.put_int(value);
    packet.put_short(0);
    packet.put_short(0);
    packet.finalize(true);
    Send(packet);

    if (m_chState == 5)
    {
        CParty* party = (CParty*)GetParty();
        if (party != 0)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            party->send_to_party(packet);
        }
    }
    else if (m_chState == 8)
    {
        WarRoom* warRoom = (WarRoom*)GetWarRoom();
        if (warRoom != 0)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            warRoom->SendToRoom(packet);
        }
    }
}