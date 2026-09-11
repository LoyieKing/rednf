// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CQuestShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CQuestShop ORIG-only 符号恢复（Wave8 W8_questshop）。
// 反汇编依据 df_game_r objdump（0x85eed2a..0x85efd73）；ctor/dtor 见
// docs/class_func_reports/CQuestShop/ 三份报告。
// df_game_r CQuestShop::clearQP 恢复。
// 依据 docs/class_func_reports/CQuestShop.md 的反汇编与反编译报告。
#include "CUser.h"
#include "CQuestShop.h"
#include "PacketGuard.h"
#include "GlobalData.h"

// CQuestShop 消费侧 statusCounts 视图（CHARAC_QUEST_SHOP_DATA 0xaa 的
// 计数视角：+0x0c..+0xa5 为 0x4d 个 ushort 状态已购计数；DB 侧 _LoadQuestShop
// 同一区按属性具名填充，两视角布局一致）。
struct QuestShopStatusView
{
    unsigned int m_pad0;              // +0x00
    unsigned int m_qp;                // +0x04
    unsigned int m_initCount;         // +0x08
    unsigned short m_statusCounts[0x4d];  // +0x0c..+0xa5
    unsigned int m_questPiece;        // +0xa6
};

#include "CInventory.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CQuestShop/CQuestShop.inc"


#include "CQuestShop/dtor_CQuestShop.inc"


#include "CQuestShop/getQp.inc"
#include "CQuestShop/getInitQp.inc"
#include "CQuestShop/getInitCount.inc"
#include "CQuestShop/getPiece.inc"

#include "CQuestShop/setQp.inc"
#include "CQuestShop/setInitQp.inc"
#include "CQuestShop/setInitCount.inc"
#include "CQuestShop/setPiece.inc"
#include "CQuestShop/addQp.inc"
#include "CQuestShop/subQp.inc"
#include "CQuestShop/incInitQp.inc"
#include "CQuestShop/incInitCount.inc"
#include "CQuestShop/addPiece.inc"

#include "CQuestShop/clearBuyingStatuslist.inc"


#include "CQuestShop/sendCharacQp.inc"


#include "CQuestShop/sendCharacQuestPiece.inc"


#include "CQuestShop/clearQP.inc"


#include "CQuestShop/makePacket_BuyingStatusList.inc"


#include "CQuestShop/subQp_1.inc"


#include "CQuestShop/findStatusQp.inc"


#include "CQuestShop/findStatusType.inc"


#include "CQuestShop/getStatusBuyingCount.inc"


#include "CQuestShop/incStatusBuyingCount.inc"


#include "CQuestShop/getMaxCount.inc"


#include "CQuestShop/getNextLevel.inc"


#include "CQuestShop/clear.inc"


#include "CQuestShop/stQuestShopStatus.inc"


#include "CQuestShop/stQuestShopStatus_1.inc"


#include "CQuestShop/dtor_stQuestShopStatus.inc"


#include "CQuestShop/checkBuyingStatus_LimitLevel.inc"


#include "CQuestShop/getInitCost.inc"


#include "CQuestShop/calUsedQp.inc"


#include "CQuestShop/buyStatus.inc"


#include "CQuestShop/clearUsedQp.inc"


#include "CQuestShop/_reset.inc"


#include "CQuestShop/SetTagCharacQuestShopData.inc"


#include "CQuestShop/loadData.inc"


#include "CQuestShop/_saveData.inc"


#include "CQuestShop/getData.inc"


// ---- ORIG W 0x0891b8b4：顶层 stStatusLevelInfo C1（+0x01 清零后拷至 +0x00）
// 合法 force-use 发射（vector<stStatusLevelInfo> 元素构造点）。 ----
namespace {
#include "CQuestShop/emitStStatusLevelInfoWeakSymbols.inc"

}  // namespace
