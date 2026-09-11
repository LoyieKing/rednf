#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EmblemCompoundServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EmblemCompoundServer + ARAD::SCRIPT::EmblemCompound 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__EmblemCompound/*.md 与
// docs/class_func_reports/EmblemCompoundServer/{EmblemCompoundServer,~EmblemCompoundServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstdio>
#include <string>

#include "EmblemCompoundServer.h"
#include "GameTypes.h"      // cMyTrace
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证）----
typedef char emblem_compound_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound) == 0x84 ? 1 : -1];
typedef char emblem_compound_result_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound::Result) == 0xa ? 1 : -1];
typedef char emblem_compound_server_size_assert[sizeof(EmblemCompoundServer) == 0x88 ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

#include "EmblemCompoundServer/EmblemCompound.inc"


#include "EmblemCompoundServer/dtor_EmblemCompound.inc"


#include "EmblemCompoundServer/loadScript.inc"


#include "EmblemCompoundServer/clearScript.inc"


#include "EmblemCompoundServer/getCost.inc"


#include "EmblemCompoundServer/getResult.inc"


#include "EmblemCompoundServer/getList.inc"


}  // namespace SCRIPT
}  // namespace ARAD

#include "EmblemCompoundServer/EmblemCompoundServer.inc"


#include "EmblemCompoundServer/dtor_EmblemCompoundServer.inc"


#include "EmblemCompoundServer/Load.inc"


#include "EmblemCompoundServer/Destroy.inc"


#include "EmblemCompoundServer/Test.inc"


// ============================================================================
// LinkClosureB.AradServers 追加段：
//   GetEmblemInfo        ORIG T 0x08191818（GetEmblemInfo.md 全文转写）
//   GetEmblemRandomIndex ORIG T 0x081919e8（GetEmblemRandomIndex.md 全文转写）
//   IsResultSelectable   ORIG T 0x08191ad6（IsResultSelectable.md 全文转写）
//   CheckCondition       ORIG T 0x08191636（CheckCondition.md 全文转写）
//   Process              ORIG T 0x08191afa（Process.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "MoreTypes.h"          // arad::SigAradEmblemCompound 完整定义
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item / ENUM_CMDPACKET
#include "LogManager.h"
#include "PacketGuard.h"
#include "CMailBoxHelper.h"     // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"
#include "CompoundEmblemDeps.h" // getCompoundEmblemStatistic
#include "AvatarConvertServer.h" // StatisticsEmblemCreate

#include "EmblemCompoundServer/GetEmblemInfo.inc"


#include "EmblemCompoundServer/GetEmblemRandomIndex.inc"


#include "EmblemCompoundServer/IsResultSelectable.inc"


#include "EmblemCompoundServer/CheckCondition.inc"


#include "EmblemCompoundServer/Process.inc"

