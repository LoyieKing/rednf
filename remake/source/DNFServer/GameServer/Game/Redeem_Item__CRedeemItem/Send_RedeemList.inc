// ORIG T 0x085f73ee _ZN11Redeem_Item11CRedeemItem15Send_RedeemListEP5CUser：
// header(0,0x121)/byte(list size)/逐项（≤0x15 项）：+0x40 计数、Inven_Item
// 字段展开、附魔通道分支、增幅属性、3rd Chronicle 子包与 put_packet，最后
// finalize + Send 并返回 1。
int CRedeemItem::Send_RedeemList(CUser* user)
{
    // [b28d-46 判词·N/N82 翻正] ORIG 0x085f73ee 全程**无 Inven_Item 局部副本**：
    // 循环首 `call _ZNKSt14_List_iterator...ptEv; mov %eax,-0xc(%ebp)` 后逐处
    // `mov -0xc(%ebp),%eax; mov 0x2(%eax),%eax`（具名 entry 指针 + 每处重载，
    // P0/P167 `it->` 形）⇒ 删 memcpy/局部对象即消 `call memcpy` 与 0x3d 字节
    // 栈副本（帧 0x80→ORIG 0x30）。
    PacketGuard packet;
    packet.put_header(0, 0x121);
    packet.put_byte((int)m_redeemList.size());

    // [R13a01 形态] ORIG 0x085f7476 段顺序为 `call list::begin(); sub $0x4,%esp;
    //   movl $0x0,-0x10(%ebp)` —— **count=0 在 begin() 之后**；且循环尾
    //   0x085f75b2 为 `addl $0x1,-0x10(%ebp)`（count）**先于** 0x085f75bc 的
    //   `call _List_iterator::ppEv`（it）⇒ 原码是「先声明 it = begin()、再声明
    //   count、for 空 init + 增量 `++count, ++it`」。我方旧形把 count=0 提前到
    //   begin() 之前、增量写作 `++it, ++count`（顺序相反）。
    std::list<Inven_Item_Expand_RedeemInfo>::iterator it = m_redeemList.begin();
    int count = 0;
    for (;
         // ORIG 0x085f75c1/0x085f75c4/0x085f75c7：`mov -0x10(%ebp),%eax;
         // cmp $0x14,%eax; ja` = **无符号序 + 两段寄存器形**（count > 20 退出）。
         // P196 前置：ORIG 无符号 ⇒ 禁改 int；按 P310 翻案形保留 int 局部 +
         // 比较处显式 (unsigned int) 转换即复现两段 + ja（直写 unsigned 局部
         // 被 4.4 折成单段 mem 直比 + jg，即改前态）。
         (unsigned int)count < (unsigned int)MAX_COUNT &&
             it != m_redeemList.end();
         ++count, ++it)
    {
        // ORIG 0x085f7452 单次 ptEv 后存具名指针 -0xc；其后每处字段访问
        // 都从该槽重载（禁缓存到 callee-saved）。
        Inven_Item_Expand_RedeemInfo* entry = it.operator->();

        // [R5a16 ⑤] ORIG 0x085f746b..0x085f7476：+0x40 处**第二次独立 ptEv**
        //   （`lea -0x24(%ebp); call ptEv; mov 0x40(%eax),%eax`）⇒ 原码该实参
        //   写的是 `it.operator->()->m_count40`（不复用 entry 指针）；
        //   首处 ptEv 结果存 -0xc 只服务 +0x02/+0x07 等后续字段（Ghidra C 双 op-> 同证）。
        packet.put_int(it.operator->()->m_count40);          // +0x40 兑换计数
        packet.put_int(entry->m_item.m_addInfo);             // +0x02 物品 idx
        packet.put_int(entry->m_item.m_addInfo2);            // +0x07 数量/强化值
        packet.put_byte((int)entry->m_item.GetItemAttr());   // 属性字节
        packet.put_short((int)entry->m_item.m_fieldb);       // +0x0b 耐久
        packet.put_byte((int)(unsigned char)entry->m_item.m_field0); // +0x00 绑定标记

        // [SemA00 2026-09-12] P354：ORIG 0x085f7508 段为**正极性直判**
        //   `call IsEnchantRevisionChannel; test %al,%al; je <put_int(item->0xd)>`
        //   ⇒ then 块 = put_int(0) 直落、else 块 = put_int(m_fieldd) 挂 je 目标。
        //   我方旧形 `if (!f())` 产 `call; xor $0x1,%eax; test; je`（多 1 条 xor 且块序反）。
        if (G_GameWorld()->IsEnchantRevisionChannel())
            packet.put_int(0);
        else
            packet.put_int(entry->m_item.m_fieldd);          // +0x0d 强化信息

        packet.put_byte((int)entry->m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)entry->m_item.m_amp.getAbilityValue() & 0xffff);

        // [R13a01 形态] ORIG 0x085f757b 段为 `call G_CDataManager; mov 0x20(%eax),%eax;
        //   lea -0x20(%ebp),%edx; mov %edx,0xc(%esp); …; mov %eax,(%esp);
        //   call make3rdChroniclePacket` —— **无 receiver 具名槽**（P356/P183：
        //   ORIG 无 `mov %eax,-0xN; mov -0xN,%eax` 往返）⇒ 原码是链式调用；
        //   我方旧形多 1 对 store/load 并使全帧槽号深 4（guard -0x24 vs ORIG -0x20）。
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
            user, entry->m_item, &packet);
        packet.put_packet(entry->m_item);
    }

    packet.finalize(true);
    user->Send(packet);
    return 1;
}