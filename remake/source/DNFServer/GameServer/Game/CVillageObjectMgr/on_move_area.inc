void CVillageObjectMgr::on_move_area(CUser* user)
{
    // [M5E round6 判据 ① 已改 · 旧判词 N82/N93 被推翻] ORIG 0x086b71da 直证：
    //  · +0x095 ptEv 后 `add $0x4,%eax; mov %eax,-0x14(%ebp)`（具名 `list* pList
    //    = &it->second;`），begin/end 由 +0x0f6/+0x11d `mov -0x14(%ebp),%edx`
    //    取用 ⇒ 旧码 `it->second.begin()` 内联 ptEv 两次；
    //  · -0x10 = count 具名（`movl $0x0,-0x10` @+0x0da），+0x0e1 `mov -0x10(%ebp),%eax`
    //    ⇒ ORIG 的占位写下的是**变量 count** 而非字面 0（旧码 `put_byte(0)`）；
    //  · -0xc = obj 具名且声明即 0（`movl $0x0,-0xc` @+0x141）：先 `obj = *iter;`
    //    再 `if (!obj->put_object_info(packet)) continue;`（+0x173 `xor $0x1,%eax`
    //    ⇒ 取反形态），旧码 `if (...put...) count += 1;`；
    //  · +0x0f3/+0x11a begin/end 各自先出裸 iterator（-0x1c/-0x18）再转成
    //    `_List_const_iterator`（-0x3c/-0x40）⇒ ORIG 用两个具名 const_iterator；
    //  · get_index 结果 -0x38 **无前置零初始化**（旧码 `int countIdx = 0;` 多余 1 条）；
    //  · 帧 `sub $0x50,%esp`（旧码 0x40）⇒ 补 3 个具名局部后同 ORIG。
    // [M5E round6 复验] 改后 157/157、字节长 0x215=0x215，DIFF→NEAR；残差仅 1 条
    //   寄存器对：循环体内虚调用 `obj->put_object_info(packet)` 的调用目标
    //   ORIG 落 %edx（+0x162 `mov (%eax),%edx`）而我方落 %eax（+0x162
    //   `mov (%eax),%eax`），实参随之 %eax/%edx 互换（各 5 条、同长）。属 4.4
    //   local-alloc 伪号序不可控（P-batch 无对应旋钮，已试形 B/C 同形）⇒ 封顶 NEAR。
    // ORIG 0x86b71da：Zone(curVill, curArea) 查找，命中则向该玩家发送 0xca 视图包。
    Zone zone(user->getCurCharacVill(), (unsigned char)user->get_area(false));
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it == m_objects.end())
        return;

    std::list<IObject*>* pList = &it->second;
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xca);
    int countIdx = packet.get_index();
    int count = 0;
    packet.put_byte(count);
    std::list<IObject*>::const_iterator iter = pList->begin();
    std::list<IObject*>::const_iterator iterEnd = pList->end();
    IObject* obj = 0;
    for (; iter != iterEnd; ++iter)
    {
        obj = *iter;
        if (!obj->put_object_info(packet))
            continue;
        count += 1;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    user->Send(packet);
}