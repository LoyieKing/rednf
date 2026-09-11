// [S6 2026-09-11] 本 TU 三件伴生/假缺口判词（均零改动、无源码旋钮）：
//  ① `StaticPool<CAssaultPlace,128>::CNode::~CNode`（ORIG 0x082f13e8，19B）：ORIG
//     `sub $0x18,%esp`（4.1 保守帧基）vs 4.4 `sub $0x4,%esp`；助记符序与条数全等
//     （NEAR 8=8）⇒ 帧大小不受源侧控制（加 dead 局部会多 store，反劣）⇒ 判 N82。
//  ② `deque<CNode*>::_M_push_back_aux<CNode* const&>`（ORIG 0x082f3cc0，129B=41 条）
//     **无** EH 段；我方 61 条含 `__cxa_begin_catch`/`_Unwind_Resume` 清尾（+20，gap −20）
//     ⇒ 4.1 out-of-line STL 实例差异，判 N89 封顶。
//  ③ `_Z41__static_initialization_and_destruction_0ii`：工具**按符号名跨 TU 错配**
//     （ORIG 该符号有 493 个实例，工具取首个 0x0807ee76 属 PacketTypes）⇒ 逐 TU 读数
//     不可比、我方 ioinit 16 条形态正确 ⇒ 零改动跳过。
CAssaultPlace* CAssaultPlaceFactory::AcquireAssaultPlace()   // ORIG 0x82f0f54
{
    return m_pool.Acquire();
}