// ORIG _ZN13user_creature12CCreatureMgr16SetKeepRightItemEP5CUserP10Inven_Item
// T 0x0833c00c（SetKeepRightItem.md）：遍历 m_creatureItems，对
// GetCreatureItemType()==1 且 IsNoCharge() 的宠物清免充标志并落库——
// CDBMsgSender((ENUM_INTERNALPACKET)0x8a, user).UseKeepRight(uid, flag)
// （flag 为清标志后重读的 IsNoCharge，恒 0）；失败打 cMyTrace
// ("...SetKeepRightItem(CUser*, Inven_Item*)",0xdf0,5) 返 0；成功 Send +
// CPacketRespondent(m_user,0,0).MakeNotipacketCreatureItemList(
// &m_creatureItems, slotNo)。循环尽 item->reset() 返 1。
// [b28d-solo-CCreatureMgr N] ndiff=2：已翻正 ①`it++` 后置（P167）、②`a!=0 && b==1`
// 短路物化正向包裹（P214①）、③去具名 creature 局部（P183）、④UseKeepRight 双实参
// 内联（P0）、⑤cMyTrace 全生命周期临时链（P227）。唯一残余 = SetNoCharge(false) 的
// 立即数与 this 装载序（ORIG `movl $0,0x4(%esp)` 先 / 4.4 先备 this；P75/N-G 跨版本
// 装载序，1 条位置差）。156=156。ORIG 0x0833c00c
// [b28d-solo-CCreatureMgr P277 试形失败] 具名派生指针（CCreature* creature = ...）
// 156→158 条 + 槽基 -0x28→-0x2c（多 4B 局部）⇒ 已回滚，封 N（P75/N-G）。
bool CCreatureMgr::SetKeepRightItem(CUser* user, Inven_Item* item)
{
    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); it++)   // P167：ORIG 增量段为后置 ppEi
    {
        // [MiscLane 2026-09-11] 局部声明为派生类型 CCreature*（替换原 CCreatureItem*，
        // 不新增局部、帧不变）：探针 /tmp/lane_MiscLane/probe_ord2.cpp g5/g8 实证
        // 「具名类型化指针接收者」使 4.4 对 `p->SetNoCharge(false)` 发 ORIG 的
        // const-first 实参装载序（`movl $0,0x4(%esp)` 先，后 `mov -0xc(%ebp),%eax`）；
        // 原 reinterpret_cast 表达式接收者恒 this-first。ORIG 0x0833c07c 直证。
        CCreature* creature = reinterpret_cast<CCreature*>(it->second);
        // ORIG 0x833c03f..0x833c061：`creature!=0 && type==1` 短路物化
        // （mov$1/mov$0 + test;je）正向包裹，勿写 `==0 || !=1` 的 continue 形。
        if (creature != 0 && creature->GetCreatureItemType() == 1)
        {
        if (creature->IsNoCharge())
        {
            // ORIG 0x833c07c 实参装载序 = `movl $0x0,0x4(%esp); mov -0xc(%ebp),%eax;
            // mov %eax,(%esp)`（常量先压、this 后载）。
            creature->SetNoCharge(false);
            CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
            // P0/P183：ORIG 0x833c0xx `movzbl %al,%ebx` + `call GetUid` 均无槽往返
            // ⇒ 两个实参表达式内联（右起求值：noCharge 先入 %ebx）。
            if (!sender.UseKeepRight(
                    creature->GetUid(),
                    (int)(unsigned char)creature->IsNoCharge()))
            {
                // P227：ORIG vararg 在 ctor 前求值并跨 ctor 保活 %ebx ⇒ 全表达式临时链。
                cMyTrace(__PRETTY_FUNCTION__, 0xdf0, 5)(
                    "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
                return false;
            }
            sender.Send();
            CPacketRespondent pkt(m_user, 0, 0);
            pkt.MakeNotipacketCreatureItemList(&m_creatureItems,
                                               creature->GetSlotNo());
        }
        }
    }
    item->reset();
    return true;
}
