// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _ZN10CEventBaseC2Ev ⇒ CEventBase
// ctor 定义须为本 TU 首个强符号（此前 stADDINFO::stADDINFO 定义在前 ⇒ 键名错为
// _GLOBAL__I__ZN9stADDINFOC2Ev）。纯前置函数定义，符号集合/构造序零变化。
// [b28d-30] 复核 NEAR：ORIG 帧 0x18 vs 4.4 0x14（P190 附注：带 call 帧 sub 恒 16k+4
// 量化，0x18 非量化值不可凑；dead int×4=0x14、+double 跳 0x24）⇒ N82/P190 封顶。
CEventBase::CEventBase()
    : m_addInfo()
{
    // ORIG：*(vptr) = &vtable_08b4bae0（编译器自动）；
    // stADDINFO ctor 0x0810af10：两个 ushort 置 0xffff；+0x08 flag 置 0。
    // [N5F 2026-09-11 修·帧 0x28→0x18] ORIG 帧 0x18 = P210 三分律③：stADDINFO::C1
    // 定义虽已在本 TU（CEventBase.inc 之后），但位于本 caller 之后 ⇒ 4.4 看不到体、
    // 按保守 0x18 outgoing 分配。旧 dead int×4（P31 垫帧）在此之上再撑 0x10 ⇒ 0x28。
    // 删之即回归 0x18；探针 /tmp/r5/n5/N5F_EventFair/probe_frame.cpp 实测：
    // 无 dead int = sub $0x18（13 条逐条同形 / 与 ORIG 一致）、有 dead int = sub $0x28。
    m_isEventing = false;
}