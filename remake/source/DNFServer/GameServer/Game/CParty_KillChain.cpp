#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CParty_KillChain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>

#include <set>
#include <vector>

#include "QuickParty.h"        // 必须先于 CDungeon.h/CParty.h（守卫惯例）
#include "CParty.h"
#include "CKillMonsterInfo.h"  // 击杀/奖励链 CKillMonsterInfo 六谓词（桥清零）
#include "CDataManager.h"      // G_CDataManager/find_dungeon/getUltimateDungeonList
#include "CDungeon.h"          // get_min_level/get_index/get_dimension_possible/…
#include "CBattle_Field.h"     // bf 子对象视图
#include "CUser.h"             // CUser/CUserCharacInfo
#include "CInventory.h"        // tryInsertItemIntoInventory
#include "WarField.h"          // map_monster 完整定义
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "CQuickPartyRewardManager.h"  // QuickParty::RewardData_Param/Result

namespace
{

// P49：寻址助手用宏（inline 函数 -O0 产真 call；ORIG 0x0859ba98 域
// `mov this; add $0xb24,%eax; mov esp; call` 直取址形）。
#define kcbf(p) ((p)->m_battleField)

// calDeadUserDungeonPenalty 死亡计数表具名视图（ORIG 0x085ad9df 寻址实证：
// `mov slot,%edx; mov this,%eax; add $0xe0,%edx; mov 0x10(%eax,%edx,4),%eax`
// —— 表基址 this+0x10，索引 slot+0xe0；越界索引照抄 ORIG 无值域检查语义）。
struct KcDeadCountView
{
    char m_pad[0x10];
    int m_deadCounts[1];
};

// _makeRewardItemInfo 的尾随 Inven_Item 视图：ORIG 0x085ad0cc 每次字段访问都
// 重发 `mov 0x10(%ebp),%eax; add $0x3d,%eax`（无指针槽，P0）⇒ 用宏内联展开，
// 不落具名指针局部。+0x2 复用 Inven_Item::m_addInfo 作调用方扩展标志。
#define kcTailItem(p) ((p) + 1)

// addDungeonClear 的每日入场次数记录视图（ORIG 0x085bde31 寻址实证：
// `mov -0xc(%ebp),%ecx; add $0x300,%ecx; movzwl 0x8(%edx,%ecx,8),%edx` ——
// 记录基址 this+(i+0x300)*8+8，步长 8：short uniqueId @+0x0 / int 计数 @+0x4）。
// CDataManager 每日入场门槛（ORIG 0x085bde7e/0x085bdf11 实证 +0x512c/0x512e
// 两个 ushort；CDataManager.h 未建模该段，按 §4.3 具名视图访问）。
}  // namespace

#include "CParty_KillChain/hasPCRoomPlayer.inc"


#include "CParty_KillChain/GetPartyMemberForceDropFlag.inc"


#include "CParty_KillChain/existSamePCRoomUser.inc"


#include "CParty_KillChain/calDeadUserDungeonPenalty.inc"


#include "CParty_KillChain/_makeRewardItemInfo.inc"


#include "CParty_KillChain/increase_result_card_gold.inc"


#include "CParty_KillChain/MemberRegisterKillMonster.inc"


#include "CParty_KillChain/getDungeonTypeKind.inc"


#include "CParty_KillChain/addDungeonClear.inc"


#include "CParty_KillChain/DungeonPermission.inc"

