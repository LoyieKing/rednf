// ---------------------------------------------------------------------------
// ORIG W 0x82f0614 (size 0x6c)：CAssaultPlace::CAssaultPlace()（默认 ctor）
// [SweepCAssaultMgr 迁移自 CAssaultMgr.cpp，Main 2026-09-09 批准；仅搬位置，
//  函数体逐字节不变]
// 依据：ORIG StaticPool<pvp_assault::CAssaultPlace,128>::C1 @0x82f1462（128 insn）
// 含完整 new[] EH cleanup（0x82f14f9..0x82f153d：CNodeD1 循环 + _ZdaPv +
// _Unwind_Resume）；4.4.7/4.1.2 探针 /tmp/p_sp1..p_sp8 实证：T 的默认 ctor 体在
// 池实例化 TU 内可见 ⇒ 判 nothrow ⇒ 省该 cleanup（我方 103 insn）；仅声明（定义
// 在他 TU）⇒ 布 cleanup（128 insn）。CAssaultPlace 池实例化于 CAssaultMgr.cpp
// （CAssaultPlaceFactory::m_pool）⇒ 本定义必须不在该 TU（P284）。
// ---------------------------------------------------------------------------
CAssaultPlace::CAssaultPlace()
{
}