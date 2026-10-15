// [K6 2026-09-11·P292 档①] int→bool（配合 WarRoom.h:89）：ORIG 调用点 0x081c3cac
// `call; test %al,%al; je 0x081c3cd2` 单字节判定；4.4 探针实测 bool 与 int 的
// `return 0/1` 形态全等（mov $0/%eax）⇒ 定义体机器码不变。
bool WarRoom::HandleGetItem(CUser* user, int idx)
{
    // [b28d-solo-WarRoom-3] ORIG 0x086bea2e：551/-4 → **546/+1**。本轮修正：
    //   ①删 `int result` 局部（ORIG 返回值走 %ebx 公共尾：086beaf0/086bef5f
    //     `mov $0x0,%ebx`、086bf2ea `mov $0x1,%ebx`、086bf317 `mov %ebx,%eax`）
    //     ⇒ 全部改直接 return（P0：ORIG 无 result 槽）；
    //   ②`else if (GetWaiterCount() < 1)` → 具名 `int count` + 失败块直落
    //     （ORIG 086beb05 `mov %eax,-0x24(%ebp); cmpl $0,-0x24; jg 主流程`）；
    //   ③删具名 bool randomOption → 内联 `IsRandomOption() && !getCheck…`
    //     （ORIG 086bee7b..086bee9d 常量岛 + `xor $1` ⇒ 无槽；P0/P214①）。
    //   残余 +1（086bec31 段尾块序）+ 版本槽基 8（N95/P332）⇒ 封顶。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N95）→ 试=槽集合比对：ORIG 38 槽 / OURS 39 槽，
    //   帧 ORIG 0x120 / OURS 0x110 但槽号非单调整移（`movb $1` -0x2d↔-0x31 深 4、
    //   PacketGuard -0x4c↔-0x40 浅 0xc，0x086bea2e 头部）；我方另有 -0x78/-0x70/-0x6c/-0x74
    //   一组 `mov %eax,X; mov X,%eax` 槽往返链（4.4 对具名局部恒写栈）⇒ 无源码可择项，维持 N95。
    char flag = 1;
    CUser* target = 0;
    // [R15a12 形态修复] ORIG 0x086bea3a..0x086bea54 构造序 = **PacketGuard 先、
    //   map_item 后**（`lea -0x4c; call PacketGuardC1Ev` 在前、`lea -0xcc; call
    //   map_itemC1Ev` 在后）⇒ 源码声明序须为 packet → fieldItem；原序相反令
    //   两条 ctor 调用次序与 ORIG 相反（构造序=声明序）。
    PacketGuard packet;
    map_item fieldItem;
    bool ok = m_field.GetFieldItem(idx, fieldItem) != 0;
    if (ok != 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else
    {
        // [b28d-solo-WarRoom-3] ORIG 086beb05 `mov %eax,-0x24(%ebp); cmpl $0x0,
        //   -0x24(%ebp); jg 086beb7e` ⇒ 原码是具名 int count + 正极性 `> 0`
        //   （失败块直落、主流程后置）；原重建写 `else if (GetWaiterCount() < 1)`
        //   使 4.4 折成无槽 `test;setle;test;je`（P0/条件极性）。
        int count = GetWaiterCount();
        if (count <= 0)
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(0x15);
            packet.finalize(true);
            user->Send(packet);
            return 0;
        }
        if (fieldItem.m_item.m_addInfo == 0)
    {
        int money = fieldItem.m_item.get_add_info();
        packet.put_header(0, 0x5d);
        packet.put_short(idx);
        packet.put_short(user->get_unique_id());
        std::map<int, map_item>::iterator iter;  // ORIG 显式 _Rb_tree_iterator C1Ev（置 0）
        // [b28d-L20r4 判据⑫/P198①] ORIG 0x086bebea `movl $0x0,-0x14(%ebp)` 在
        //   CheckPickupItem 调用**之前** ⇒ 原码是预清两句式 `int check = 0;
        //   check = …;`（Ghidra C：`local_24 = 0; local_24 = CheckPickupItem(…)`）。
        int check = 0;
        check = m_field.CheckPickupItem(
            idx, user->get_unique_id(), iter);
        if (check < 1)
        {
            int gained = user->getCurCharacInvenW()->gain_money(
                money, (eMoneyAddReason)0xf, 1, 0);
            user->saveTaxMoneyForUpperMember(money);
            if (gained < money)
            {
                if (gained < 0)
                    gained = 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, money, gained);
            }
            m_field.PickupItem(iter);
            packet.put_int(gained);
            packet.finalize(true);
            SendToRoom(packet);
            return 1;
        }
        else
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(check);
            packet.finalize(true);
            // [R5a13 2026-09-12·④] ORIG 0x086bec8a..0x086beca9：本分支（第一处
            //   CheckPickupItem 的 check>=1，即 m_addInfo==0 money 分支的 else）
            //   在同一分支内 **先 SendToRoom(packet) 再 user->Send(packet)**；
            //   旧实现把 SendToRoom 误写进第二处（cMyTrace）分支 ⇒ 广播错发/漏发。
            SendToRoom(packet);
            user->Send(packet);
            return 0;
        }
    }
    else
    {
        int itemIdx = fieldItem.m_item.m_addInfo;
        CItem* item = G_CDataManager()->find_item(itemIdx);
        if (item == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__,
                                  0x957, "G_CDataManager()->find_item(%d) fail", itemIdx);
            return 0;
        }
        else
        {
            target = user;
            if (user == 0)
            {
                packet.put_header(1, 0x2e);
                packet.put_byte(0);
                packet.put_byte(0x16);
                packet.finalize(true);
                SendToRoom(packet);
                return 0;
            }
            else
            {
                // [b28d-solo-WarRoom-3] ORIG 086bee7b..086bee9d 无 bool 槽：
                //   `test %al,%al; je <mov $0>`（IsRandomOption 直判）+ 086bee8a
                //   `xor $0x1,%eax; test %al,%al; je <mov $0>`（!getCheck…）+ 常量岛
                //   + `test %al,%al; je 主流程` ⇒ 原码是内联短路 `if (A && !B)`
                //   （P0/P214①），原重建的具名 bool randomOption 多占槽 -0x11。
                if (item->IsRandomOption() && !user->getCheckPickUpRandomOptionItem())
                {
                    user->setCheckPickUpRandomOptionItem(true);
                    DB_InsertRandomOptionItemInform::makeRequest(
                        user->GetUID(), user->get_acc_id(), 1);
                    PacketGuard notice;
                    notice.clear();
                    notice.put_header(0, 0x171);
                    notice.put_short(user->get_unique_id());
                    notice.put_byte(1);
                    notice.finalize(true);
                    user->Send(notice);
                }
                std::map<int, map_item>::iterator iter;  // ORIG 显式 _Rb_tree_iterator C1Ev（置 0）
                // [b28d-L20r4 同族] ORIG 0x086bf092 `movl $0x0,-0x1c(%ebp)` 预清。
                int check = 0;
                check = m_field.CheckPickupItem(
                    idx, user->get_unique_id(), iter);
                if (check < 1)
                {
                    // [b28d-L20r4 判据⑫] ORIG 0x086bef8f `movl $0x0,-0x20(%ebp)` 预清
                    //   （Ghidra C：`local_20 = 0;` 后 if/else 两臂各赋值 —— 两臂全覆盖，
                    //   与旧写的 `= -1` 语义等价，但与 ORIG 槽/指令形一致）。
                    int added = 0;
                    if (fieldItem.m_item.isAvatarItemType() == 0)
                    {
                        added = user->getCurCharacInvenW()->insertItemIntoInventory(
                            fieldItem.m_item, (eItemAddReason)0x16, 1, 1);
                    }
                    else
                    {
                        char ipg[24];
                        WongWork::CGenUniqueNo::genIPGNo(
                            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                            user->getCurCharacNo(), ipg);
                        added = user->addAvatarItem(
                            (unsigned long)fieldItem.m_item.m_addInfo, 0,
                            (char)fieldItem.m_item.m_endurance, ipg,
                            (eAvatarItemAddReason)0x11);
                    }
                    if (added == -1)
                    {
                        packet.put_header(1, 0x2e);
                        packet.put_byte(0);
                        packet.put_byte(4);
                        packet.finalize(true);
                        user->Send(packet);
                        return 0;
                    }
                    else
                    {
                        m_field.PickupItem(
                            iter);
                        packet.put_header(0, 0x5d);
                        packet.put_short(idx);
                        packet.put_short(user->get_unique_id());
                        packet.put_short(added);
                        packet.finalize(true);
                        SendToRoom(packet);
                        return 1;
                    }
                }
                else
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x982, 5);
                    trace("fieldData_.PickupItem( %d/%d ) ", idx, itemIdx);
                    packet.put_header(1, 0x2e);
                    packet.put_byte(0);
                    packet.put_byte(check);
                    packet.finalize(true);
                    // [R5a13 2026-09-12·④] 修正 b28d-lane25 的错位：ORIG 0x086bf076..
                    //   0x086bf083 本分支（第二处 CheckPickupItem 的 cMyTrace else）
                    //   **只有** user->Send(packet)，**无** SendToRoom（0x086bec97 的
                    //   SendToRoom 属另一分支，见上）。
                    user->Send(packet);
                    return 0;
                }
            }
        }
    }
    }
    return 1;
}