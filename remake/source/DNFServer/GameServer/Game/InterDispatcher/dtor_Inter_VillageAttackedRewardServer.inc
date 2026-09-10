Inter_VillageAttackedRewardServer::~Inter_VillageAttackedRewardServer() {}
// ORIG 0x084e05c2 T dispatch_sig —— 村庄攻击奖励结束：组 0x1ec 包全网广播、清理复仇副本与角色状态。
// 报告 docs/class_func_reports/Inter_VillageAttackedRewardServer/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=119（ORIG 0x084e05c2）：槽序/帧差(N82/P212/P221)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e05c2）：本轮补 P253 memset(10) 显式形 + P28 后置 it++（p167 CLEAR）；余 N82/P112 帧槽平移。
