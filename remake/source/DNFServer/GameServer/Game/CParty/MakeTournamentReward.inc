// [H7PartyGlobal 2026-09-11 · N 封顶判词] 唯一实质残差 = x87 比较 lowering：
// ORIG 0x85bcb40 段 `fucompp/fnstsw/test $0x45,%ah/sete` vs 4.4 `fucomip/fstp/seta`（N85/N97 族）。
// 负结论证据（Main 全 TU A/B 四组，四数逐值相同 identical 146 / ae 331 / 合计 477 / near 0 / diff 30）：
//   ① 现行路由基线；② `-march=i486 -mtune=generic`；③ `-B cc1plus446bin/`（4.4.6 后端）；④ ②+③ 叠加
//   ⇒ 本形差在本 TU 不可由旗标消除（H46 头集臂已含本 TU，后端保持 4.4.7）。另全库 x87 族 26 TU A/B 仅 7 受益、
//   其中 6 为 weak/COMDAT 伪增益 ⇒ 停止追旗标。
// p167：余 C1/C2、D1/D2 同址别名（P93），无语义调用缺口；logFormat 立即数为 .o 显示伪差。
void CParty::MakeTournamentReward(CTournamentDungeonReward& reward)
