Dispatcher_Req_Urgent_Quest::~Dispatcher_Req_Urgent_Quest() {}
// ORIG 0x081df42c：state>2 → param+4=0/param+8=-1；等级 27..69 且
// getCurCharacQuestR()->m_authenData.m_urgentQuest==-1 才继续；
// hasEpicQuest → err=1；否则静态 selected_quest 清空后
// getQuestNPCList(8)+allowable_questlist_as_npc(…,true) 取候选：
// 空 → err=4；非空 → 静态 CMTRand randInt(size-1) 抽一个写回
// m_urgentQuest 与 param+8，param+4=1。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081df42c orig/ours=214/206 ndiff-blocks=38
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x28(%ebp) | mov    -0x28(%ebp),%eax
