// ORIG 0x83ffe0e：GetDBHandle(3,0)；组装 0xc000 SQL 缓冲（两份模板：
// 普通 expire_date 与 from_unixtime 版，m_period 为 0 用普通版）。
// m_period ∈ (6,0x1e] 时 expire_date = getCurSec() + period*86400，否则直接 period。
// set_query 变参 13 个（SQL 模板仅 11 个占位，末两参为 ORIG 遗留多余实参）。
// [SweepW8LT b28d] DIFF：gap 14（our 220 / ORIG 234）封顶。p167 17/19：
// EXTRA memcpy×2 —— ORIG 4.1 对 `memcpy(sql,"...",0xce/0xdd)` 内联为
// `mov $imm,%esi; mov $0x33/%0x37,%ecx; rep movsl` + 尾 word/byte（各 13 条），
// 4.4 -O0 恒 call memcpy（各 5 条）；memset 实参装载 ORIG 多 1 条/处。
// 结构体整体赋值已试劣化（N84③），长度/语义逐处核对一致。ORIG 083ffe0e
bool DB_AvatarItemInsert::InsertAvatarItem(SIG_AVATAR_INSERT* sig)
