// §6.2 巨件核销：P167 77/77 [0缺0多]；gap 515→515/522；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x52 + add $0xd/0xe/0x10 两步，4.4 折单 lea；语义集合全等  (ORIG 0x081f2d0c insn 515->522)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 287（ORIG 515 / ours 522），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
int DisPatcher_SellItem::dispatch_sig(CUser* user, PacketBuf& packet)

{
    char invenType;                     // -0x45
    short slot;                         // -0x48
    unsigned short count;               // -0x46
    unsigned long money;                // -0x64
    int sum;                            // -0x3c
    int unusedGuard;                    // -0x38（ORIG 死存储 movl $0x6）
    int sellResult;                     // -0x34
    int sellTimeRet;                    // -0x30
    CParty* party;                      // -0x2c
    CMap* map;                          // -0x24
    Village* village;                   // -0x20
    Area* area;                         // -0x1c
    Inven_Item* invenItem;              // GetInvenItemPtr（寄存器传递）
    CItem* item;                        // GetItemPtr（寄存器传递）
    int rarity;                         // -0x40

    // [R3a16 第三轮 SEMANTIC-DIFF 修复] ORIG 真值方向：命中 {3,8,5,10,12} ⇒ **进入处理块**，
    // 未命中 ⇒ return 0（ORIG 0x81f2d23/33/43/53/63 五条 `je 0x81f2d6c`=`mov $0x0,%eax`
    // → `test; je 处理块`）。原写法 `if (state==3||..) return 0;` 与之互为反，已翻转。
    if (!(user->get_state() == 3 || user->get_state() == 8 ||
          user->get_state() == 5 || user->get_state() == 10 ||
          user->get_state() == 0xc))
        return 0;

    if (packet.get_byte(invenType) != 1)
        return LineFunc(0x7e15, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_short(slot) != 1)
        return LineFunc(0x7e16, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_short(count) != 1)
        return LineFunc(0x7e17, __PRETTY_FUNCTION__, 0, 0);

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 3))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0xd1);
        return 0;
    }

    // P48：GetInvenItemPtr / GetItemPtr 为两次独立调用（各自求值实参）
    invenItem = GlobalData::s_pSecuProtectionField->GetInvenItemPtr(
        user, invenType, slot);
    item = GlobalData::s_pSecuProtectionField->GetItemPtr(user, invenType, slot);
    rarity = GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x23, item, invenItem);
    if (rarity != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, (unsigned char)rarity);
        return 0;
    }

    if (user->CheckItemLockSlotSpace(invenType, slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0xd5);
        return 0;
    }

    // [K6-DpcSigBig L5] P225/P133：ORIG 0x081f2fa1..0x081f2fd4 两条条件**各自
    // 独立物化**为 `cmp $imm; sete %al; test %al,%al; je 081f3010`（双跳同一
    // 失败目标，中间夹 getInDungeonItemSellCount 的完整实参装载）⇒ 原码是
    // `if (state==5) { if (count==7) {...} }` 嵌套形；旧 `&&` 合并形 4.4 折成
    // `jne` + `mov $1/jmp/mov $0/test` 岛（少 2 处 sete/test 对）。
    if (user->get_state() == 5)
    {
        if (user->getInDungeonItemSellCount(invenType, slot) == 7)
        {
            user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x139,
                                           1, 0, 0);
        }
    }

    PacketGuard packetGuard;            // -0x60
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        packetGuard.clear();
        packetGuard.put_header(1, 0x18);
        packetGuard.put_byte(0);
        packetGuard.put_byte(0x3c);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        return 0;
    }

    money = 0;
    if (packet.get_int(money) != 1)
        return LineFunc(0x7e71, __PRETTY_FUNCTION__, 0, 0);

    sum = 0;
    if ((int)(signed char)invenType + (int)slot + (unsigned short)count + sum !=
        (int)money)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x335,
                                       1, 0, 0);
    }

    unusedGuard = 6;                    // ORIG -0x38 死存储
    // [P5-DPC 2026-09-11] P251/P261 真 switch 轴：ORIG 0x081f315e..0x081f31cc 为 -O0
    // 决策树（`movl $0x6,-0x38`；`movzbl -0x45,%eax; movsbl %al,%eax`；
    // `cmp $0x1,%eax; je <case1>`；`cmp $0x3,%eax; je <case3>`；`jmp <0x4c0>`）——
    // case1 直接 `SendCmdErrorPacket(0x18,0x17); return 0`；case3 内再
    // `movzwl -0x44; cmp $0x9,%ax; jg <0x4c0>` 后同样错误返回。我方旧 if/else-if
    // 令 4.4 出 `movzbl; cmp $0x1,%al; jne` 字节比较链（diff at orig[226:228]）。
    switch (invenType)
    {
    case 1:
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0x17);
        return 0;
    case 3:
        if (slot <= 9)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0x17);
            return 0;
        }
        break;
    default:
        break;
    }

    if (user->get_state() == 5)
    {
        party = user->GetParty();
        if (party != 0)
        {
            // [G5-SigA] P183：ORIG 0x081f31fd/0x081f323c 每次都重发完整寻址链
            // `mov -0x2c(%ebp),%eax; add $0xb24,%eax`（无 battleField 指针缓存槽）。
            // 原具名 CBattle_Field* battleField 多出槽写入（我方 -0x20）+ 后续沿用
            // 单条 load，改回逐次重算表达式。
            // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）
            map = (CMap*)G_CDataManager()->find_map(
                (party)->m_battleField.get_cur_map());
            if (map != 0 && map->getAssignedNPCCount() == 0 &&
                !(party)->m_battleField.IsAssignNPCByPotion())
            {
                WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
                hackAnalyzer->addServerHackCnt(user,
                                               (WongWork::ENUM_HACKTYPE)0x323,
                                               1, 0, 0);
            }
        }
    }

    if (user->get_state() == 3)
    {
        if (G_GameWorld()->check_valid_area((int)user->getCurCharacVill(),
                                            user->get_area(false)))
        {
            village = G_GameWorld()->getUserVillage(user);
            if (village != 0)
            {
                area = village->getArea(user->get_area(false));
                // [R20a05 ④/§26.3] ORIG 0x081f3338 `je 0x081f338a`（area==0 直跳）
                //   与 0x081f334e `je 0x081f338a`（`call IsExistItemShopNPC;
                //   xor $1;test;je` 同靶）为**两条独立守卫同落同一出口** ⇒ 原码
                //   是嵌套独立 if；合并 `&&` 令 4.4 物化 bool 岛
                //   （test;jne;mov $1;jmp;mov $0;test;je；实测件级 582→580 条，P225/P133）。
                if (area != 0)
                {
                    if (!area->IsExistItemShopNPC())
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0x324, 1, 0, 0);
                    }
                }
            }
        }
    }

    sellResult = G_Store()->user_sell_item(user, invenType, slot, count);
    if (sellResult != 0)
    {
        packetGuard.put_header(1, 0x18);
        packetGuard.put_byte(0);
        packetGuard.put_byte(sellResult);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        if (sellResult == 0x16)
            user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)3, 0, 0);
    }

    sellTimeRet = 0;
    // [G5-SigA] P52/P183：ORIG 0x081f345a `movl $0x0,-0x30(%ebp)` 之后
    // 直接把 getCurTickCount 的返回值放进实参位（无 -0x40 具名槽往返；
    // fn_diff_tu 实测我方多 `mov %eax,-0x40(%ebp)` + `mov -0x40(%ebp),%eax` 2 条）。
    sellTimeRet = user->SellItemTimeChecker(
        GlobalData::s_systemTime_.getCurTickCount(), 1000);
    if (sellTimeRet == 2)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x138,
                                       1, 0, 0);
    }
    else if (sellTimeRet == 4)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x321,
                                       1, 0, 0);
    }

    return 0;
}
