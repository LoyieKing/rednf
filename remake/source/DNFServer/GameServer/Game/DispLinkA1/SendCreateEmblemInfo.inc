// ============================================================================
// 11. Dispatcher_DisJointAvatar::SendCreateEmblemInfo  ORIG 0x08218a36
//     条数<6 才发；每对 (itemIdx, value)：find_item 失败即整体放弃；
//     slots[i+1]=素材 grade、slots[i+6]=附加值；末写 slots[0]=条数。
// [b28d-solo-DispLinkA1] verdict=DIFF N 封顶（98 vs 97，ndiff=13；p167 14/14 CLEAR）。
//   已修：P28 it++（operator++(int)）、P167 内联 GetServerProxy（删匿名 helper）、
//   P0 具名 itemIdx/value 槽、P13 end 缓存、P196/P218 负条件 setne/jne、
//   P239 begin→end→count=0 发射序、P311 具名数组成员视图（m_grades 已归正）。
//   残余 N：①N7 实参先升格 %ebx 跨 receiver call（P226 语句表达式/裸形均不翻，
//   4.4 恒先 call 后装载）；②N94 m_values 常量拆分（ORIG add $8+disp -0x58，
//   4.4 lea 0x4+disp -0x48，地址等价）；③N18b 尾比较（ORIG mov count,%ebx;
//   call size; cmp %eax,%ebx，4.4 恒 cmp mem,%eax 内存直比）。
// ============================================================================
void Dispatcher_DisJointAvatar::SendCreateEmblemInfo(
    std::vector<std::pair<int, int> > const& results)
{
    Packet_Emblem_Create_Statistic packet;
    if (results.size() >= 6)
        return;

    std::vector<std::pair<int, int> >::const_iterator it = results.begin();
    std::vector<std::pair<int, int> >::const_iterator end = results.end();
    // [b28d-solo-DispLinkA1 P239] ORIG 发射序：begin→end→count=0。
    int count = 0;
    while (it != end) {
        // [b28d-solo-DispLinkA1 P0] ORIG 具名槽 -0x14/-0x10（各一次 operator->）。
        const int itemIdx = it->first;
        const int value = it->second;
        // [b28d-solo-DispLinkA1 N] ORIG 实参先升格 %ebx 跨 receiver call
        // （G_CDataManager）保活；P226 语句表达式/裸形均不翻（4.4 恒先 call 后装载）。
        const CItem* item = G_CDataManager()->find_item(itemIdx);
        if (item == 0)
            return;
        packet.m_grades[count] = item->get_grade();
        packet.m_values[count] = value;
        ++count;
        if (count > 5)
            break;
        it++;   // [b28d-solo-DispLinkA1 P28] ORIG 调 operator++(int)（sret -0x1c）
    }
    // [b28d-solo-DispLinkA1 P196/P218] ORIG 为负条件（setne/jne）。
    // [M1S3EmblemLink 2026-09-11 负结论] 操作数交换（size() != count）实测无效：
    //   ORIG 0x08218b3c 形为 `mov -0x18(%ebp),%ebx; …; call size; cmp %eax,%ebx`
    //   （102 条），4.4 两序均产 `call size; cmp -0x18(%ebp),%eax`（101 条）⇒
    //   N18b 编译器比较装载序，非操作数序可解。回滚。
    if (count != (int)results.size())
        return;
    packet.m_count = count;
    // [b28d-solo-DispLinkA1 P167] ORIG 直调 GetServerProxy(0)（无匿名 helper）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}