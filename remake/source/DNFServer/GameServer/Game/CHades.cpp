// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHades/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— XNuclear::CHades（Hades 反外挂客户端成员类，内嵌于 CUser +0x8e954）。
// 参照 docs/class_func_reports/XNuclear__CHades/：
//   Send_Login           @ ORIG 0x084b9200
//   needLogin            @ ORIG 0x084b9718
//   GetIP                @ ORIG 0x084b9774
//   Send_UserStatus      @ ORIG 0x084b97fe
//   Send_ReturnToVillage @ ORIG 0x084b9386（纯空函数）
#include "CUser.h"
#include "CMCAPManager.h"

#include <string.h>

#include "CEnvironment.h"
#include "CHadesServerProxy.h"
#include "DB_InterDeps.h"   // DB_HadesPunish::makeRequest（ORIG T 0x08445dd4）
#include "GlobalData.h"
#include "Packet_Hades_Login.h"
#include "Packet_Hades_StdUpdate.h"
#include "Packet_Hades_GoldTracking.h"
#include "Packet_Hades_UserStatus.h"
#include "Packet_Hades_Logout.h"
#include "Packet_Hades_AddHackType.h"
#include "Packet_Hades_Gold_Plus.h"
#include "Packet_Hades_Gold_Minus.h"

namespace XNuclear
{

#include "CHades/Init.inc"


#include "CHades/Send_Logout.inc"


#include "CHades/Send_HackType.inc"


#include "CHades/AddHackTypeCnt.inc"


#include "CHades/ChangeSecurity.inc"


#include "CHades/Send_ChangeSecurity.inc"


#include "CHades/GoldPlus.inc"


#include "CHades/GoldMinus.inc"


#include "CHades/Send_ReturnToVillage.inc"


#include "CHades/ExpUp.inc"


#include "CHades/needLogin.inc"


#include "CHades/StdCateUpdate.inc"


#include "CHades/Send_Login.inc"


#include "CHades/GetIP.inc"


#include "CHades/Send_UserStatus.inc"


#include "CHades/UseFatigue.inc"


#include "CHades/GoldTracking.inc"


#include "CHades/Send_ChangeGold.inc"


#include "CHades/RunHadesAction.inc"


// ---- CHades_GameSvr（ORIG 空标记类型）----
// ORIG C1/C2 @ 0x082a5a68、D1/D2 @ 0x082a5a6e（各 5 字节 = push ebp/mov esp/pop
// ebp/ret 空体）。GameSvr 侧配置占位；无成员、无基类、无虚函数。
// 全局对象 s_hades_（_ZN10GlobalData8s_hades_E @ 0941f728）ctor 由 ORIG
// __static_initialization_and_destruction_0（0x082a3a5a）调用并注册 atexit。
class CHades_GameSvr
{
public:
    CHades_GameSvr();
    ~CHades_GameSvr();
};

#include "CHades/CHades_GameSvr.inc"


#include "CHades/dtor_CHades_GameSvr.inc"


}  // namespace XNuclear
// GlobalData 静态成员（namespace 形态，见 GlobalData.h 注释）；完整类型见上。
XNuclear::CHades_GameSvr GlobalData::s_hades_;
