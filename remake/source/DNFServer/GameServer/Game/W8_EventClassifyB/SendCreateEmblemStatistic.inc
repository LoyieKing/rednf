// SendCreateEmblemStatistic（ORIG 0x0810f742）
// ORIG 写入序：先 put_grade（slots[i+1]）后 put_value（slots[i]）——get_grade
// 调用在 value 读取之前（+0x09c vs +0x0ae）。size>5 判断（cmp $5;seta）。
// [b28d-L14 P30/N82附] ORIG 0x0810f742；p167 CLEAR（13/13）；数组下标 add-vs-lea
// 组合差（ORIG `mov n,%ebx` 直用 vs 4.4 `lea 1(%eax),%ebx`）+ 基址 -0x40/-0x44。
// [b28d-solo-W8_EventClassifyB N94 封顶] 改用具名视图 m_grades[n]/m_values[n] 后
// grade 侧已逐条全等（`mov n,%ebx; mov %eax,-0x40(%ebp,%ebx,4)`）；value 侧
// ORIG `lea 0x8(%ebx),%edx; mov %eax,-0x4c(%ebp,%edx,4)`（disp=packet+0x2、lea+8）
// vs 4.4 `lea 0x4(%ebx),%edx; mov %eax,-0x3c(%ebp,%edx,4)`（disp=packet+0x12、lea+4）
// —— 4.4 对成员数组偏移 0x18 恒拆 disp=偏移&0xf=8 / lea=4，ORIG 4.1 拆 disp=2/lea=8；
// 地址与语义完全等价（均 = packet+0x22+4n），助记符序列全等（NEAR）。
// 合法 C++ 无法把 union 基址移到 +0x2（PacketHeader 非平凡 ctor 不可作 union 成员）⇒ N94。
// [b28d-W8EventB 复核] jcc 归一化 11/11 逐位一致；`call *` 虚槽 vptr+0xc
// （`mov (%eax),%eax; add $0xc,%eax; mov (%eax),%edx`）两侧逐条全等。
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N6/P30，gap=+0/blocks=1）→ 试=①prett/q3_gap2 缺口 0、
//   strall CLEAR ②**槽视图旋钮实测**：值写点改 `m_slots[n+6]` 视图劣化（99→101，多
//   `mov -0x10,%eax; lea 0x6(%eax),%ebx` 两条）⇒ 回退保持 `m_values[n]`；③残差定性：
//   两侧最终地址全等（我方 `lea 0x4(%ebx),%edx; [%ebp+%edx*4-0x3c]` = ORIG
//   `lea 0x8(%ebx),%edx; [%ebp+%edx*4-0x4c]` = ebp+4n-0x2c），仅**常量折叠拆分**不同
//   （4.4 折 +1 元素入索引、基址 +8；4.1 折 +2 元素、基址 −8）⇒ 同源表达式跨版本折叠差
//   ⇒ 维持 N（99/99，唯一 hunk）。
void CAvatarDisjointRandomReward::SendCreateEmblemStatistic(
    std::vector<std::pair<int, int> >& rewards)
{
    // [b28c-L0 N6/P30] ORIG 两个独立数组基址（grade @-0x40 直接索引 n；
    // value @-0x4c+`lea 0x8(%ebx)`），我方单数组 m_slots 恒 `lea 1/lea 6`
    // 折同一地址。值索引已按 ORIG 槽语义修为 m_slots[n+6]（越界尾随照抄 P171）。
    Packet_Emblem_Create_Statistic packet;
    if (rewards.size() > 5)
        return;

    int n = 0;
    for (std::vector<std::pair<int, int> >::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        CItem* citem = G_CDataManager()->find_item(it->first);
        if (citem == 0)
            return;
        if (citem->GetItemType() == 0x14)
        {
            packet.m_grades[n] = citem->get_grade();
            packet.m_values[n] = it->second;
            ++n;
            if (n > 5)
                break;
        }
    }
    if (n <= 0)
        return;
    packet.m_slots[0] = n;
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}