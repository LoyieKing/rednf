// ORIG T 0x08468914 (size 0x238) —— registBestRecord.md
// [b28d-solo-W6_RankMailDeps] DIFF(N) — 修法（ndiff 133→~93，insn 152→153/ORIG 150）：
//   ① 删 src 指针缓存（P183），每 lane 重发 `imul $0x17 + add members` 链；
//   ② byte lane 改 `uchar(*)[0x17]` 数组引用形复现 ORIG `movzbl 0x14(%edx)`
//      disp 读（探针 /tmp/b28d_w6_probe4.cpp l2）；③ insert 保持 P178
//      value_type(make_pair(key,rec)) 双层临时链（pair C1IjS3_）。P167 CLEAR(14/14)。
//   残余 = 结构性 N 族：a) 每 lane 多 1 条 `mov 0x14(%ebp),%ecx` + lea（ORIG
//      `add 0x14(%ebp),%edx` 内存操作数，N4/P30 基址恒先物化，探针 l2 实测）；
//   b) 局部槽序 i/rt/bt = -0x20/-0x1c/-0x24 vs ORIG -0x1c/-0x24/-0x20
//      （N82/N98 首次使用序分配，声明序不可翻）；c) cur 槽 -0xd0 vs ORIG -0xcc
//      连带 stage/playTime -0x48/-0x44 vs -0x44/-0x40（同族）。语义全等。
//      ORIG 0x08468914。
// [S2W6Deps 复核] a) 项 = **gcc 中端 lowering 固有差**（fdiff.py reloc 归一口径
//   实测：3 处 lane 各 1 条，`insert orig[52..] N| mov 0x14(%ebp),%edx` 等）：
//   同址表达式 `members + i*0x17`，原厂发 `imul; add 0x14(%ebp),%edx`（mem 直加），
//   gcc11 恒先物化基址再 `lea (%edx,%eax,1),%edx` ⇒ 每 lane +1 条，gap -3 全额由
//   此解释；属 N4/P30 族（同 registRanker），源码不可造。
void WongWork::CDeathTowerRanking::registBestRecord(
