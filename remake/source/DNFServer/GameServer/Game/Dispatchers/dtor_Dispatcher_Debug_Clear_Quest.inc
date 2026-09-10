Dispatcher_Debug_Clear_Quest::~Dispatcher_Debug_Clear_Quest() {}
// ORIG 0x081df91c：isGMUser 否 → LineFunc(0x51f3) 返回其值；
// res.m_index = msg（MsgClearQuestIndex::operator=，ORIG W 0x08235266）；
// 遍历 m_questIdxs：find_quest 失败 → return 1；exposeQuest &&
// check_possible → getCurCharacQuestW()->m_cleared.setClearedQuest(idx)、
// m_field138>0 → incQuestPoint(值,idx,true)、m_field13a>0 →
// incQuestPiece(值,idx,true)，计数；循环后 res.m_index.m_count=实际清空数，
// sendCharacQp() + sendCharacQuestPiece()，return 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081df91c orig/ours=151/127 ndiff-blocks=24
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN28Dispatcher_Debug_Clear_Quest7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x14(%ebp),%eax | mov    %ea
