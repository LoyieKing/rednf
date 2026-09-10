bool CCreatureMgr::SetKeepRightItem(CUser* user, Inven_Item* item)
{
    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); it++)   // P167：ORIG 增量段为后置 ppEi
    {
        CCreatureItem* creatureItem = it->second;
        // ORIG 0x833c03f..0x833c061：`creatureItem!=0 && type==1` 短路物化
        // （mov$1/mov$0 + test;je）正向包裹，勿写 `==0 || !=1` 的 continue 形。
        // 同一对象按 CCreature 语义访问（ORIG 直呼 CCreature 访问器，单继承布局
        // 同址；每次重发 reinterpret_cast 表达式，无具名 creature 局部）。
        if (creatureItem != 0 && creatureItem->GetCreatureItemType() == 1)
        {
        if (reinterpret_cast<CCreature*>(creatureItem)->IsNoCharge())
        {
            // [P277 试形失败] 具名派生指针 156→158 条 + 槽基 -0x28→-0x2c（多 4B 局部）
            // ⇒ 回滚；残余 = P75/N-G 跨版本实参装载序（ORIG `movl $0,0x4(%esp)` 先）。
            reinterpret_cast<CCreature*>(creatureItem)->SetNoCharge(false);
            CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
            // P0/P183：ORIG 0x833c0xx `movzbl %al,%ebx` + `call GetUid` 均无槽往返
            // ⇒ 两个实参表达式内联（右起求值：noCharge 先入 %ebx）。
            if (!sender.UseKeepRight(
                    creatureItem->GetUid(),
                    (int)(unsigned char)reinterpret_cast<CCreature*>(creatureItem)
                        ->IsNoCharge()))
            {
                // P227：ORIG vararg 在 ctor 前求值并跨 ctor 保活 %ebx ⇒ 全表达式临时链。
                cMyTrace(__PRETTY_FUNCTION__, 0xdf0, 5)(
                    "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
                return false;
            }
            sender.Send();
            CPacketRespondent pkt(m_user, 0, 0);
            pkt.MakeNotipacketCreatureItemList(&m_creatureItems,
                                               creatureItem->GetSlotNo());
        }
        }
    }
    item->reset();
    return true;
}
