Dispatcher_Break_Away_Quest_Check::~Dispatcher_Break_Away_Quest_Check() {}
// ORIG 0x081d390a：msg.m_checkFlag==0（+0xd）→ setBreakAwayDungeonClearCnt(-1)、
// setBreakAwayQuestTime(0)、StreamPool::Acquire("PacketDispatcher_Impl_1.cpp",
// 0x31d9)+CStreamGuard(stream,true) 写 0x144/UID/acc_id 入 DB_Q；
// 否则 getBreakAwayDungeonClearCnt()==0 →
// GlobalData::s_BreakAwaySys->giveBreakAwayDungeonClearReward(user)，
// 非 0 → param.m_resultCode=1。
// 【blocked：giveBreakAwayDungeonClearReward 及其 selectReward/
// Inter_LoadEtc::giveBreakAwayReward 生产链尚未还原（报告齐备，独立批次），
// 已 hub 通知 Main；不写猜测实现。】
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081d390a orig/ours=98/98
