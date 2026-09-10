DisPatcher_DieMob::~DisPatcher_DieMob() {}
// ============================================================================
// ORIG T 0x081eb0c4 _ZN17DisPatcher_DieMob12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DieMob/dispatch_sig.md，1564 insn）
// setMonsterMoveHack 门禁（moveCheckFlag && state==5）后按 state 分派：
//   5=普通副本 kill_monster 全反作弊链 / 10=死亡塔 handleKillMonster /
//   0xc=BossTower Global::CBossTowerFunc::DieMob / 0xd=advancealtar dieMob /
// [b28d-29] N78/N82：ORIG 无 0xa3f 字节 memset 而 4.4 发 call + 帧 0xb10 vs 0xb40；调用点全等  (ORIG 0x081eb0c4 insn 1564->1585)
//   8=WarRoom HandleMonsterKill。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1582（ORIG 1564 / ours 1578），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] 真语义缺口核销（实测 ndiff 824→759；P167 MISSING 8→2，仅剩变体）：
//  ① ORIG 0x081eb9b7/0x081ec396/0x081ec7a2/0x081ecac5/0x081ecb10 是逐层重新 call
//     get_state() 的 if-else 链（==5/0xa/0xc/0xd/8），原码折成 switch 少 4 次调用；
//  ② 4 处 `GetPartyIndex();`/`GetWarRoomIndex();` 丢弃直呼是我方凑数，ORIG
//     0x081eb635/0x081eb9d0/0x081ec629/0x081ecb29 各只 1 次（movswl 缓存），已删；
//  ③ ORIG 0x081ec355/0x081ec761 在 static ValidateFixMobDiePos 前确实 call
//     getHackAnalyzer 后丢弃返回值，已补。
// 残差封 N：1 处 memset(N78) + PacketGuard/std::pair/cMyTrace 的 C1↔C2、D1↔D2 变体。
