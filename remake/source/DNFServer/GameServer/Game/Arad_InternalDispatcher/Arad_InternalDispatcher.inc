// [A31-GI · P327] Arad_InternalDispatcher::C1 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN4ARAD10DISPATCHER23Arad_InternalDispatcherC2Ev`（ORIG 0x819a31a）。
// 零语义/机器码变化，仅发射序。
ARAD::DISPATCHER::Arad_InternalDispatcher::Arad_InternalDispatcher()
{
    // ORIG 0x081984d4：map C1 后跟一次显式 map::clear。
    m_dispatchers.clear();
}