// ORIG T 0x08468c68。该塔 5 个榜首槽顺序序列化（基址 tower*0x390，stride 0x98）。
void CDeathTowerRanking::makeTopRankerRecordPacket(unsigned int towerIdx,
                                                   PacketGuard& packet)
{
    // ORIG 0x08468c68：i 从 1 起（movl $1,-0xc → cmpl $5,setle ⇒ i<=5），
    // 槽址 = m_towers + towerIdx*0x390 + i*0x98（双 imul 直用 m_towers 基址，
    // 不经 m_slots 具名 +0x98 偏移；具名 slots 局部会引入 add $0x90/+0x8
    // 常量折叠与 -0x10 基址槽）。
    // [b27] 残差：ORIG 先载 0xc(towerIdx)/-0xc(i) 后两 imul、我们 eax 预载
    // m_towers 且 lea 合并——N18a 族寄存器序不可定向，语义/循环界/槽序已全等。
    // 【b28d-solo-CompoundDeathLCB 定案】达成 **AE**：24/24 逐条全等。修法 =
    // 真二维下标 `m_towers[towerIdx][i]`（两级索引令 4.4 发
    // `mov 0xc(%ebp),%eax; mov -0xc,%edx; imul 0x98; imul 0x390; lea; add
    // 0x8(%ebp),%eax`，与 ORIG 全等；旧 char* 算术形 25 条且 this-first 不可翻）。
    // [b28d-Main·检查点14] stTowerBlock 包装类删除 ⇒ 成员访问改二维下标（同形）。
    for (int i = 1; i <= 5; ++i)
        _makeRecordPacket(m_towers[towerIdx][i], packet);
}