// ORIG T 0x08468614 (size 0x300) —— registRanker.md
// [b28d-solo-W6_RankMailDeps] DIFF(N) — 修法（ndiff 216→~125，insn 170→229/ORIG 224，gap 5）：
//   ① slot 指针缓存与 oldStage/oldTime 具名局部全删（P183），每处槽访问重发完整
//      二维链；② 槽寻址直用真二维下标 `m_towers[towerIdx].m_slots[slotIdx]`
//      （Main 代落头 m_slots[6] 后）复现 ORIG 双 imul 形
//      （探针 /tmp/b28d_w6_probe3.cpp f_2d 逐条同形；此前 TU-local 视图因 cast
//      多 this 中转 236→229）；③ byte lane 改
//      `uchar(*)[0x17]` 数组引用形（探针 probe4 l2：7→6 条/lane）；
//      ④ insert 保持 P178 双层临时链（make_pair+pair C1IjjE/C1IjS3_）。
//      P167 CLEAR(8/8)。残余 gap 5 = 结构性 N 族：a) 3 处 byte lane 源读 4.4
//      恒先载 members 寄存器 + lea（ORIG `add 0x14(%ebp),%eax` 内存操作数，
//      N4/P30，探针 probe4 l2）；b) m_playTime store LHS-first+`mov %eax,%edx`
//      中转 vs ORIG RHS-first（N76/P211）；c) erase 段 `add %edx,%eax` vs
//      ORIG `add 0x8(%ebp),%eax`（this 装载序，N7/N18a）。另槽序
//      -0x1c/-0x20/-0x28… vs ORIG -0x1c/-0x24/-0x2c…（N82/N98）。语义与调用点
//      全等（14 处槽访问、3 处 insert/erase 调用点逐条同形）。ORIG 0x08468614。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N4/P30）→ 试=N4a 轴反向（`uchar(*)[0x17]` 数组引用 → `uchar* + i*0x17+0x14` 混合形 R2：our 228→234 劣化、add 链变长，已回滚）；ORIG 0x084686bc `imul $0x17; add 0x14(%ebp),%eax`（mem-add）vs 4.4 `mov 0x14(%ebp),%edx; lea (%edx,%eax,1)` ⇒ 维持。
void WongWork::CDeathTowerRanking::registRanker(
