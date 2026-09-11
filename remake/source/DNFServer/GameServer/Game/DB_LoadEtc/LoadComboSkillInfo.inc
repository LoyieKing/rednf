// ============================================================================
// DB_LoadEtc::LoadComboSkillInfo @0x0844a3b2 —— combo 区 job==9 才拉双树。
// ============================================================================
// b28d-31: N 封顶（P223 边界）——ORIG 0x0844a3b2；4.4 尾部多 [jmp;nop]
// epilogue-threading（`!call()` 等三形实测不消），语义等价。
// [b28d-solo-DB_LoadEtc] AE ndiff=0 (orig=43/ours=43)：P257 正向包裹 `if (job==9){...}`
// 消掉 4.4 的 epilogue-threading [jmp;nop] 尾岛（旧早返三形均留 1 岛）。
void DB_LoadEtc::LoadComboSkillInfo(SIG_LOAD_ETC* sig)
