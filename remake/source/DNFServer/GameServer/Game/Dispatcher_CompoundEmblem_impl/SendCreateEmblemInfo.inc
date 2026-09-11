// 布局（ORIG 汇编实证）：Packet_Emblem_Create_Statistic(0x17a4, 0x36)
// + count@slots[0](+0xa) + grades@slots[i+1](+0xe) + addInfo@slots[i+6](+0x22)，
// SendPacket(pkt, 0x36)。迭代器 while 循环（ORIG __gnu_cxx::operator!= 判续）。
void Dispatcher_CompoundEmblem::SendCreateEmblemInfo(
    const std::vector<std::pair<int, int> >& created)
{
    // b28d-40: ORIG 0x08221dde。
    // [M1S3EmblemLink 2026-09-11 落地] P0 删具名 `proxy` 局部（原占 -0x18，把
    //   count/迭代器/ppEi/packet 全部顶深 4）⇒ 槽全部归位（packet -0x5a、slots[0]
    //   -0x50、count -0x18、it/end -0x20/-0x24），指令数 104→102 = ORIG 102。
    // 残余（byte 层 N 封顶）：① N7 ORIG 0x08221e60 `mov -0x14(%ebp),%ebx; call
    //   G_CDataManager; mov %ebx,0x4(%esp)`（实参先升格跨 receiver call）vs 4.4
    //   先 call 后装载；② N18b 尾比较 ORIG 0x08221ee4 `mov -0x18(%ebp),%ebx; …
    //   call size; cmp %eax,%ebx` vs 4.4 `cmp -0x18(%ebp),%eax`（同 4.1/4.4 装载序差）；
    //   ③ N94 数组 disp 折叠（ORIG `add $0x8,%edx; mov -0x58(,%edx,4)` vs 4.4
    //   `lea 0x6(%eax),%edx; mov -0x50(,%edx,4)`，地址等价）。
    Packet_Emblem_Create_Statistic packet;

    // ORIG 0x08221dfb seta：size()>5 直接返回。
    if (created.size() > 5)
        return;
    // [b29 R20] 判据⑩/P28：ORIG 0x08221f05 `movl $0x0,-0x18`（count=0）出现在
    // begin(-0x20)/end(-0x24) 调用**之后** ⇒ count 声明后移至迭代器之后。
    std::vector<std::pair<int, int> >::const_iterator it = created.begin();
    std::vector<std::pair<int, int> >::const_iterator end = created.end();
    int count = 0;
    while (it != end)
    {
        // [b29 R20] ORIG 0x08221e3f/0x08221e4f 两次 operator->（P157 逐字段重读，
        // 每次落具名 int 槽 -0x14/-0x10）：key=it->first、value=it->second。
        int key = it->first;
        int value = it->second;
        const CItem* item = G_CDataManager()->find_item(key);
        if (item == 0)
            return;                       // ORIG 中途 return
        packet.m_slots[count + 1] = item->get_grade();
        packet.m_slots[count + 6] = value;
        ++count;
        if (count > 5)
            break;
        it++;                             // [b29 R20] P28 增量侧：ORIG 调 ppEi（后置）
    }
    // ORIG 0x08221ef4 setne：count != size 时返回。
    if (count != (int)created.size())
        return;
    packet.m_slots[0] = count;
    // [M1S3EmblemLink 2026-09-11 P0] ORIG 0x08221f01 起
    //   `lea -0x5a(%ebp),%ebx; mov s_statistic_proxy_mgr,%eax; …; call
    //   GetServerProxy; movl $0x36,0x8(%esp); mov %ebx,0x4(%esp); …`
    //   ⇒ 无具名 proxy 槽（原 proxy 占 -0x18；ORIG 该槽是 count，且 ORIG
    //   无 -0x18 之外的额外槽 ⇒ 我方全部槽被顶深 4）。接收者表达式内联。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}
