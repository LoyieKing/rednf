// [round2-L30 复核·N82/P200] 残余：ORIG `sub $0x38` 无 callee-saved vs 我方
//   `push %esi; push %ebx; sub $0x30`（我方多 2 push，跨 find 保活 c/d）+ 槽整体浅 4
//   （ORIG -0x18/-0x12/-0x10 vs 我方 -0x14/-0xe/-0xc）⇒ N82 帧/槽分区族；ORIG 0x086b4964。
// [M2S4 2026-09-11 推翻旧 N82 判词] 56/56（ORIG 56，逐条同形含 `sub $0x38`/`leave`；
//   p167 6/6，余 Zone C1↔C2 同址别名）。真因=链式 `it->second.OnKillVillageMonster(...)`
//   让 4.4 把 c/d 提前装 callee-saved（2 push + lea/add 收尾）；ORIG 0x086b49f5/49f8
//   `add $0x4,%eax; mov %eax,-0xc(%ebp)` 直证具名 area 局部 → 补 `CVillageMonsterArea*
//   area = &it->second;` 即归零（P0 槽-局部双向对应律）。
int CVillageMonsterMgr::OnKillVillageMonster(int a, int b, unsigned short c, bool d)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    Zone zone((unsigned char)a, (unsigned char)b);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    if (it == m_areas.end())
        return 0x15;
    // [P0] ORIG 0x086b49f5/0x086b49f8 `add $0x4,%eax; mov %eax,-0xc(%ebp)` —— 具名
    // area 局部有真槽 ⇒ 原码有该局部；写成链式调用会让 4.4 把 c/d 提前装入
    // callee-saved（多 push %esi/%ebx + lea/add 收尾，our 60 vs ORIG 56）。
    CVillageMonsterArea* area = &it->second;
    return area->OnKillVillageMonster(0, c, d);
}