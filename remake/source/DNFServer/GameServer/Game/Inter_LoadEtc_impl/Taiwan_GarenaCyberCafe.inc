// ---- Garena 段 [blocked]：Taiwan::GarenaAuthData 权威视图在 TaiwanQueries.h，
// 但该头与 MoreTypes.h 存在五组 Sig 类 ODR 重定义，include 任一均冲突。
// dispatch_sig 尾部 Taiwan 段调用按 ORIG 保留语义登记为 blocked_dependencies：
//   garena->getGcaType() / getMobileAuth() / GarenaCyberCafe / GarenaAuthPCRoom
// [P327/N132 封顶] 本 TU 相对 ORIG 多 1 个
//   `_GLOBAL__I__ZN15CSkillTreeParamC2EP5CUserP12SIG_LOAD_ETCR11PacketGuard`：
//   根因（-H 实测链）CUser.h:85 → TaiwanQueries.h → TaiwanCash.h →
//   ThreadInterface.h:29 `#include <iostream>` ⇒ 本 TU 发射
//   std::ios_base::Init __ioinit（static_init 体仅 InitC1 + __cxa_atexit）。
//   ORIG 本区唯一 GI 为 _GLOBAL__I__Z7getUserj（属 InterDispatcher.cpp 的 TU）；
//   删直接 include 无效（CUser.h 仍传递）。唯一修法 = 删共享头
//   ThreadInterface.h 的 iostream（7 直接 + 12/56 间接消费）⇒ Main 2026-09-10
//   按 GmCmds_F 同判例裁决「不删」⇒ 结构性噪音封顶。
bool Taiwan_GarenaCyberCafe(unsigned short cafeFlag);      // T 0x0817313c [INFERENCE 归属名]
