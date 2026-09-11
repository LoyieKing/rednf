// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Sanicova/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ Sanicova 命名空间类实现（独立 TU，从 GameStubs.cpp 拆出）。
// LinkMisc5 收口：CPad 全字段访问器真实化（ORIG 反汇编逐条核对），并承载
// WongWork::CSecurityCard 补充方法与 CUser::sendBackSecurityAuthRequest。
#include "CUser.h"

#include "Cipher.h"
#include "GameRand.h"    // get_rand_int
#include "GlobalData.h"  // s_systemTime_ / s_secu_cipher_keystring_manager_
#include "PacketGuard.h"

namespace Sanicova
{

#include "Sanicova/init.inc"


#include "Sanicova/reset.inc"


#include "Sanicova/isTimeAttackSeccess.inc"


#include "Sanicova/getCancelCnt.inc"

#include "Sanicova/incCancelCnt.inc"

#include "Sanicova/setCancelCnt.inc"

#include "Sanicova/getRequestState.inc"

#include "Sanicova/setRequestState.inc"


#include "Sanicova/isActivate.inc"

#include "Sanicova/setActivate.inc"

#include "Sanicova/isCertified.inc"

#include "Sanicova/setCertified.inc"

#include "Sanicova/getFailCnt.inc"

#include "Sanicova/setFailCnt.inc"

#include "Sanicova/getVersionInfo.inc"

#include "Sanicova/setVersionInfo.inc"

#include "Sanicova/getAdvideChangePwd.inc"


#include "Sanicova/SendChagePwdRewardMail.inc"


#include "Sanicova/setAdvideChangePwd.inc"


#include "Sanicova/getChangedPwd.inc"


#include "Sanicova/isValidKeyGenTime.inc"


#include "Sanicova/GenerateRandomKeyIdx.inc"


#include "Sanicova/GenerateRandomShakeData.inc"


#include "Sanicova/GenerateRandomData.inc"


#include "Sanicova/GetSelectedCipherIdx.inc"


#include "Sanicova/GetKeyIdx.inc"


#include "Sanicova/GetShakeData.inc"


#include "Sanicova/setChangedPwd.inc"



#include "Sanicova/SendChagePwdReward.inc"


}  // namespace Sanicova

// ===========================================================================
// WongWork::CSecurityCard 补充方法（LinkMisc5）
// ===========================================================================

#include "Sanicova/getRequestState_1.inc"


#include "Sanicova/generateQuestion.inc"


#include "Sanicova/sendBackSecurityAuthRequest.inc"

