// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NPCScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "NPCScript.h"

#include <algorithm>
#include <cstring>
#include <string>

#include "NpcScript.h"
#include "CCubeStatistic.h"   // getItemGroupNameTable()（ORIG 0x088c0409）
#include "CDataManager.h"     // G_CDataManager()/getItemList()（giveGiftItem 族）
#include "CItem.h"            // CItem::get_index/getItemGroupName
#include "DNFLexWrapper.h"    // ScanType/ScanInt/ScanStr
#include "DNFFunctionLib.h"   // get_rand_int(int)
// class SpinLock / struct less_TCHAR（[random message balloon] 静态查找表用）。
#include "CEnvironment.h"

// ============================================================================
// CNPCScript / NpcScript / importNpcScript
//   NpcScript ctor/clear：ORIG 0x089fb942 / 0x089fbf22（NpcScript/ 报告逐字段）
//   importNpcScript：     ORIG 0x089fc264（_lt_global_gt_/importNpcScript.md
//                         反编译 C 逐分支转写，§9 主分支级还原）
//   CNPCScript 访问器：    ORIG 0x85809a6 / 0x86d2736 / 0x8580fd0 /
//                         0x8580e6a / 0x85811a4
// ============================================================================

// ---- 外部符号（ORIG 真实 T/B；定义在各自 TU，链接期解析）----
// getNpcRoleByString/getNpcMoodFromString/getDirectoryPath/g_npcScript*/
// STScriptFileList::FindIndexByFullScan 声明见 NpcScript.h。

// [favor rate per mood] 的除数（ORIG _DAT_08e1dbf4 = 100.0f，匿名 .rodata 常量）。
// [OurOnly33 符号名轴 2026-09-11] ORIG 全 ELF 无该 local 符号（我方发射
// `_ZL22NPC_FAVOR_RATE_DIVISOR` r 段）⇒ 删命名 float，使用点内联字面量：4.4 仍发
// `fdivs <匿名 .rodata 常量>`，与 ORIG 的 _DAT_08e1dbf4 装载形一致。

#include "NPCScript/CNPCScript.inc"


#include "NPCScript/dtor_CNPCScript.inc"



#include "NPCScript/PresentResponceScript.inc"


#include "NPCScript/clear.inc"


#include "NPCScript/PresentResponceScriptException.inc"


#include "NPCScript/clear_1.inc"


#include "NPCScript/InteractiveDialogScript.inc"


#include "NPCScript/clear_2.inc"


#include "NPCScript/PresentResponceScriptEvent.inc"


#include "NPCScript/clear_3.inc"


#include "NPCScript/clear_4.inc"


#include "NPCScript/NpcScript.inc"

#include "NPCScript/dtor_NpcScript.inc"


#include "NPCScript/clear_5.inc"


namespace {

// 解析失败统一出口：ORIG 在各 scan 失败处 return 0。
// [OurOnly33 全局符号轴 2026-09-11] ORIG 全 ELF 无 IMPORT_OK/IMPORT_FAIL：我方曾发射
// `_ZN12_GLOBAL__N_1L11IMPORT_FAILE` / `_ZN12_GLOBAL__N_1L9IMPORT_OKE`（r 段各 4B）
// ⇒ 改 enum 常量：纯编译期、零符号、零布局、使用点零改动。
enum { IMPORT_OK = 1, IMPORT_FAIL = 0 };

}  // namespace

#include "NPCScript/importNpcScript.inc"


// ORIG W 0x08582374/0x085823a4：isKeyItem 的具名 functor（nm 三符号
//   isSameItem C1/C2/cl，W 弱符号；ctor 以 pair<unsigned long,unsigned int>
//   转发构造 m_pair，operator() 非 const 且语义为
//   item.first==m_pair.first && m_pair.second>=item.second）。
struct isSameItem
{
    std::pair<unsigned long, unsigned int> m_pair;

    isSameItem(unsigned long m, unsigned int j)
        : m_pair(m, j) {}

    bool operator()(const std::pair<unsigned long, unsigned int>& item)
    {
        if (m_pair.first == item.first && m_pair.second >= item.second)
        {
            return true;
        }
        return false;
    }
};

#include "NPCScript/load.inc"


#include "NPCScript/isExistRole.inc"


#include "NPCScript/isRewardLevel.inc"


#include "NPCScript/isKeyItem.inc"


#include "NPCScript/getPeriodRewardType.inc"


// ============================================================================
// CNPCScript 礼物/心情/奖励 6 函数 —— ORIG 0x85809e4 / 0x8580d30 / 0x8580f28 /
// 0x8581076 / 0x85811f8（T）；getFavorRatePerMood 为头内联（W 0x085819c6）。
// ============================================================================

#include "NPCScript/giveGiftItem.inc"


#include "NPCScript/getGiftRewardItem.inc"


#include "NPCScript/getNextMood.inc"


#include "NPCScript/getLevelRewardInfo.inc"


#include "NPCScript/getBuffRewardInfo.inc"

