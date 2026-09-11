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
