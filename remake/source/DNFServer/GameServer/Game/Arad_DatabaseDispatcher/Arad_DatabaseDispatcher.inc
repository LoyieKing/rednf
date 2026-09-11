// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN4ARAD10DISPATCHER23Arad_DatabaseDispatcherC2Ev`
// （P327：GI 名 = TU 内第一个「强符号·外部链接·本 TU 定义」实体；全限定定义
//  可写在命名空间块之外，发射序因此提前）。**语义与机器码零变化，仅发射序。**
// ---------------------------------------------------------------------------
// ORIG ctor 0x08184c7c：map C1 后跟一次显式 map::clear。
ARAD::DISPATCHER::Arad_DatabaseDispatcher::Arad_DatabaseDispatcher()
{
    m_dispatchers.clear();
}