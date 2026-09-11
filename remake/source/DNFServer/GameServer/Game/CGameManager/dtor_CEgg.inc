// ============================================================================
// DynamicPool<CEgg/CCreature>::CNode —— 多态 T 的 CNode vptr 重写（显式特化）
// ============================================================================
// ORIG CNodeC1/D1（df_game_r）：基类 T() 之后先把 vptr 改写为 CNode 自身虚表
// （CEgg CNode `_ZTVN11DynamicPoolIN13user_creature4CEggEE5CNodeE@0x08c2f470`+8
//  =0x08c2f478；CCreature CNode `@0x08c2f488`+8=0x08c2f490），再写 magic/bUse。
// 真虚建模不可行（实证：-O0 true-virtual 的 vptr 写为 `movl $0x8`+R_386_32 重定位，
// 恒为小偏移而非硬编 0x8c2f478；AE 口径不归一立即数）→ 手动常量按 ORIG 槽序写回。
// 基类 dtor 外联定义必须出现在 CNode 特化之前（P210 帧继承实证：callee out-of-line
// 定义先可见 ⇒ caller sub $0x4=ORIG；仅声明 ⇒ 4.4 保守 sub $0x18；探针 /tmp/bm3/p1-p10）。
user_creature::CEgg::~CEgg()
{
    // [b28d-Main·检查点14] vptr 复位由编译器发射（ORIG 0x08338f74 同形，探针 vt_probe.cpp）。
}