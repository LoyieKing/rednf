// ============================================================================
// importTitleBookEtcScript —— ORIG 0x08aa8110
// 报告 _lt_global_gt_/importTitleBookEtcScript.md；调用点 init.md +0x1f23：
//   importTitleBookEtcScript("Etc/TitleBook.etc", this+0xa788)。
// 双遍结构（ORIG __lookup/__init_lock/__inited 三个函数级静态）：
//   首令牌以 id=0 进入 switch，case 0 与首个 case 0x39 合并，之后逐 case
//   **贯穿**登记 6 个 (键,id)（0x39/0x4d/0x7f/0xc6/0xe4/0xf4），落到 default
//   后释放锁、置位 __inited 并 continue，以当前令牌重新查表派发——
//   因此首令牌同样会被正常处理。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 650/650、P167 CLEAR(94/94)；残差 =
// 4.1 switch 叶块 nop 占位岛 ×6（4.4 恒不发射；详见文件头判词表）。
// [2026-09-10 D06 · N 封顶复核] 本轮三形实测（读 `fnx.py` 归一化口径）：
//  ① 原形（内层 `for(;;)` + `continue`）：643 vs ORIG 642（我方多 1 条 jmp）；
//  ② 内层+外层全 goto 形（`titlebook_retry:`/`outer_retry:`）：642 = 642（gap 0）
//     但残差变为「入口 `jmp <outer_head>`（我方多）↔ 置位处 `jmp +0x61`（我方少）」
//     两处位置互换；③ 单 goto 出口形：同 ②。⇒ 属 4.4 循环头/出口块布局（P223/A28
//     epilogue-threading 族），源码层不可分裂；保留 ② 形（gap 0 优于 ① 的 +1）。
bool importTitleBookEtcScript(const char* path, stTitleBookInfo* out)
