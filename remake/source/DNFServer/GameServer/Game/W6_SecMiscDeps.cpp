// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_SecMiscDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GlobalData.h"
#include "CInventory.h"
#include "CUser.h"
#include "CEventMgr.h"
#include "CGenUniqueNo.h"
#include "DNFFunctionLib.h"         // DNFFLib::Binary2Hex
#include "CGuildServerProxy.h"      // CGMAccounts / stGMInfo_t
#include "CMCAPManager.h"
#include "CCeraShop.h"
#include "CItemList.h"              // get_rand_int
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "CSystemTime.h"
#include "CRijndael.h"
#include "CEnvironment.h"           // IMethod::GetBlockSize / Pad、G_CEnvironment
#include "TaiwanCash.h"             // WongWork::IPG::CIPGHelper 权威类（b22 IPGInput 真调用）
#include "Inven_Item.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <list>

// ORIG 0x0858c81c _ZN6OS_API8snprintfEPcjPKcz（printf 家族封装）。
namespace OS_API
{
    int snprintf(char* buf, unsigned int size, char const* fmt, ...);
}

using namespace GlobalData;

// ----------------------------------------------------------------------------
// b22 桥清零（P201 + 真修，Model22）：原「handle-of-handle」§4.5 例外经 ORIG
// 0x086c1383..13f2 逐指令复核**推翻**——`mov eax,ds:0x941f734; mov eax,[eax]`
// 取 [s_pIPGHelper] 所指对象 vptr 的槽 0，`mov edx,ds:0x941f734; mov [esp],edx`
// 首参传 **s_pIPGHelper 值（对象 this）**，非旧注所称 &变量地址；旧桥
// `fn(&s_pIPGHelper, ...)` 系首参错位真 bug（IPGInput 以 this 解引用对象字段，
// 传 handle 槽地址=越界语义）。正解：TaiwanCash.h:436 权威虚槽 0
// IPGInput(CUser*, const char*×5, void*)（T 0x080ffb0a，定义 W6_IPGDeps.cpp:515）
// 真成员调用，与 GlobalData.cpp:3465 IPGOutputConfirm 真调用先例同族。
// ----------------------------------------------------------------------------

// [L32 GI 调序] ORIG `_GLOBAL__I__ZN8WongWork9CWebEvent14m_strEventNameE`（CU=WebEvent.cpp）
//   ⇒ 本 TU 首个强符号须为 WongWork::CWebEvent::m_strEventName。定义前移到所有函数
//   定义之前；该表为静态初始化 char 数组，不影响任何动态初始化对象的相对构造序（P327）。
//   数据取自 df_game_r D 0x093ace60（size 0xb4），槽长 0x12。R12 G 轴实测
//   ORIG raw 逐槽：0=""、1="Yegam"、2="ATI Coupon"、3="DNF Guide book"、
//   4="Everland"、5="Muz"、6="King of Alttle"、7="Free Charge Combo"、
//   8=""、9=""（槽内其余字节为 0）⇒ 照抄，不额外补前导空串。
char WongWork::CWebEvent::m_strEventName[10][0x12] = {
    "", "Yegam", "ATI Coupon", "DNF Guide book", "Everland",
    "Muz", "King of Alttle", "Free Charge Combo", "", "" };

// ============================ WongWork::CSecurityCardCenter =================

#include "W6_SecMiscDeps/CSecurityCardCenter.inc"


#include "W6_SecMiscDeps/init.inc"


#include "W6_SecMiscDeps/encryptString.inc"


#include "W6_SecMiscDeps/generateCertKey.inc"


#include "W6_SecMiscDeps/_updateTimeCount.inc"


#include "W6_SecMiscDeps/generateServerKey.inc"


#include "W6_SecMiscDeps/checkPhoneNumber.inc"


#include "W6_SecMiscDeps/generateCardData.inc"


// ============================ WongWork::CSecurityCard =======================

namespace
{
// CSecurityCard 答案表布局视图（ORIG 表基址 this+0x08，`movzbl 0x8(%eax,%edx,1)`
// 实证；仅 TU 局部 POD 布局视图，无成员函数/无符号发射）。
struct CSecurityCardAnswerView
{
    char m_pad08[8];
    unsigned char m_table[0x64];
};
}

#include "W6_SecMiscDeps/checkAnswer.inc"


// ============================ WongWork::CGMAccounts =========================

#include "W6_SecMiscDeps/getGMInfo.inc"


// ============================ WongWork::CMCAPManager ========================

#include "W6_SecMiscDeps/getExposedCount.inc"


// ============================ WongWork::CUserPremium ========================

#include "W6_SecMiscDeps/getCeraShopDiscountRate.inc"


#include "W6_SecMiscDeps/GetPremiumInfoList.inc"


// ============================ WongWork::CEventMgr ===========================

#include "W6_SecMiscDeps/GetCoinPerDay.inc"


#include "W6_SecMiscDeps/GetMoneyCharacCreate.inc"


#include "W6_SecMiscDeps/IsPCRoomEventing.inc"


// W7_BuildHarmonize 去重：IsChildrensDayEventing 唯一 owner 为 CEventMgr.cpp。

// ============================ WongWork::CWebEvent ===========================

// [L32 GI 调序] CWebEvent::m_strEventName 定义已前移到文件首（须保持 ORIG 的
// GI 键名），此处不再重复定义。

// 一维化（Main 批准）：R12 G 轴 raw 实测 ORIG 0x093acf20 非空槽 = **槽 2/3**
//（22 个 '\0'，即槽 0..1 空置）；"IP00000001"+'\0' 占槽 2（偏移 22..32），
// "IP00000002"+'\0' 占槽 3（33..43），余（槽 4..9）自动补 0。旧值 33 个 '\0'
// 使两非空槽整体后移 1 槽（=0xb 字节），已按 ORIG 校正。
char WongWork::CWebEvent::m_strEventIPGCode[10 * 0xb] =
    "\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0"
    "IP00000001\0IP00000002";

#include "W6_SecMiscDeps/_GetEventString.inc"


#include "W6_SecMiscDeps/_GetEventIPGCode.inc"


#include "W6_SecMiscDeps/_GetATIAvatarAbility.inc"

 
#include "W6_SecMiscDeps/ReqSaveEventItem.inc"


#include "W6_SecMiscDeps/RecvInfoFromWeb.inc"


#include "W6_SecMiscDeps/RecvInfoFromDB.inc"

