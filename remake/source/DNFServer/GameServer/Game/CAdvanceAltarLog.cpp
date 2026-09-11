// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAdvanceAltarLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [b28d-E03 2026-09-10 本波收口] 四数 68/102/0/5 → **68/107/0/2**：
//   · incrementEntranceData DIFF→AE：if(it==end){装配}else{累加} 块序翻转
//     （ORIG 0x08156c95 `je +0xc1`：未命中块 fall-through 在前）。
//   · updateDatabase DIFF→AE（**258=258**）：P259 删 raw/msgq 具名槽 + P132
//     `(*stream)->operator<<` + P328⑩ cMyTrace 单语句临时链 + P13 begin/end 缓存
//     + P96 非 const iterator + P0 size 具名 int + 自拷贝守卫 memcpy 0xe。
//   · incrementLayerStatData 111 vs 112 → N18a/N7 封顶（c5root 4.1.2 探针：具名/
//     const/register 三形亦必 spill；试形读数 113/115/112/113，保留 112 最优）。
//   · getIndex 16 vs 12 → N18a 封顶（9 形探针 12~21 条）。
//   · 真 bug 修正：装配写 m_field2(+0x2) → **m_itemLevel(+0x8)**（ORIG 0x08156e1d
//     写 pair+0x8）；删 SigTypes_E.h 显式 m_padE ⇒ W 模板实例
//     `_ZNSt4pairIKx27STAdvanceAltarLayerStatDataEC1IRS0_S1_EEOT_OT0_` 复现 ORIG
//     0x081583e2 的 3×mov + 尾 `movzwl 0xc(%eax); mov %ax,0x14(%edx)`（25=25 归零）。
#include "CAdvanceAltarLog.h"

#include <cstring>

#include "CStreamGuard.h"
#include "GlobalData.h"      // GlobalData::s_stream_pool / s_msgq_mgr
#include "MsgQueueMgr.h"
#include "StreamPool.h"

#include "CAdvanceAltarLog/makeKey.inc"



#include "CAdvanceAltarLog/getType.inc"


#include "CAdvanceAltarLog/getIndex.inc"


#include "CAdvanceAltarLog/getLevel.inc"


#include "CAdvanceAltarLog/CAdvanceAltarLog.inc"


#include "CAdvanceAltarLog/dtor_CAdvanceAltarLog.inc"


#include "CAdvanceAltarLog/initStatistics.inc"


#include "CAdvanceAltarLog/incrementEntranceData.inc"


#include "CAdvanceAltarLog/incrementLayerStatData.inc"


#include "CAdvanceAltarLog/updateDatabase.inc"

