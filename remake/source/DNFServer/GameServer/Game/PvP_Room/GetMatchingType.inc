// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：GetMatchingType（原 TU 第 104 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================
// ============================================================================
// 自由函数
// ============================================================================
// ORIG 0x85d4690：查 PvP map；onlyDeathMatch = info.m_playable；
// mapNo∈[0,0x13] 时返回 PVPMAP_DEATHMATCH_MODE[mapNo]
// 末段 ORIG `movzbl tbl(%eax),%eax` 全字直返（无 setne 归一）⇒ 原返回 int +
// (unsigned char) 提升（mangled 无返回类型，零 ABI 风险，技巧33）。
// [b28d-solo-PvP_Room] P248 档①/P292：ORIG 调用点（ChangePvPMode 0x085d8b8e /
// CheckPvPMapMode 0x085dd98e / DrawRandomMapIndex）均 `call; test %al,%al; je`
// 无 setne ⇒ callee 返回 bool；配合 PVPMAP_DEATHMATCH_MODE 改 bool 数组后本函数
// 体尾仍 `movzbl`（bool→bool 无需归一化），自身保持 identical。
// ORIG 0x855c8f4：按频道类型返回匹配类型（9→2, 10→3, 14→4, 15→5, 其余→0）
int GetMatchingType()
