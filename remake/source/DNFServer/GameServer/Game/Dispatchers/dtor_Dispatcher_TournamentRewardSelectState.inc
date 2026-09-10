Dispatcher_TournamentRewardSelectState::~Dispatcher_TournamentRewardSelectState() {}
// ORIG 0x081dff32（报告 Dispatcher_TournamentRewardSelectState/check_error.md）：
// state!=5 → 0x7fffffff；GetParty(GetPartyIndex)==0 → -1；
// TournamentDungeonClearState!=1 → 0x7fffffff；!CheckRecvFromAllMember → 0x7fffffff。
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081dff32
//   [N82→P200] 根因=具名 int partyIdx 落 -0x10 槽（帧 0x24→0x28）+ 缺 user==0 判空；
//   改 GetPartyIndex() 内联实参（movswl→%ebx 跨 call）+ 补判空 ⇒ 归零。
