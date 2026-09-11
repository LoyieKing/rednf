// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <ctime>
#include <memory>

#include "CDataManager.h"
#include "LogManager.h"
#include "CSystemTime.h"
#include "NpcScript.h"

#include "CItemShop/ScanType.inc"
#include "CItemShop/ScanInt.inc"
#include "CItemShop/ScanStr.inc"

namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ---- 跨类符号（ORIG 真实符号） ----

// [L32 GI 调序] ORIG `_GLOBAL__I_g_itemShopScriptBaseDirectory`（CU=RDARScriptItemShop.cpp）
//   ⇒ 本 TU 首个强符号须为 g_itemShopScriptBaseDirectory。仅交换两块定义的先后：
//   零语义/零布局改动（g_itemShopScriptFileList 仍是本 TU 唯一动态初始化对象，
//   相对构造序不变，P327）。
// ORIG g_itemShopScriptBaseDirectory（B 0x094fe160[0x100]）：脚本目录全局
// 缓冲（initItemShopScript 0x89dc4f8 填充；本 TU 提供符号，空目录等价
// loadRDARScriptFile("", path)）。nm 权威名见 InitScriptProducers_5.cpp。
char g_itemShopScriptBaseDirectory[0x100];

STScriptFileList g_itemShopScriptFileList;  // ORIG 全局（0x94fe260），原 GameStubs.cpp 定义迁移

#include "CItemShop/importItemShopScript.inc"


#include "CItemShop/loadItemShopFiles.inc"


#include "CItemShop/checkValidShopItem.inc"


#include "CItemShop/checkLimitNpcBuyItemList.inc"


#include "CItemShop/IsOneADayShop.inc"


#include "CItemShop/GetOneADayItemList.inc"


#include "CItemShop/setOneADayItemListNo.inc"


#include "CItemShop/updateOneADayItemList.inc"


#include "CItemShop/testupdateOneADayItemList.inc"

