// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 QuickParty/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— QuickParty 独立实现（batch-5 迁移自 GameStubs.cpp）。
// 本批（S4）：CQuickParty ctor/init、CQuickPartySystemManager ctor/
// init_quick_party_pool/cancel_quick_party 按权威报告实现。
// remove_quick_party_in_pool / push_quick_party_in_pool / destroy 等仍由后续批次实现。

#include "QuickParty.h"
#include "CGameManager.h"

#include <vector>

#include "LogManager.h"

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CDungeon.h"
#include "GameWorld.h"   // send_party_info_to_all / G_GameWorld


// CGameManager.h 目前与 CBattle_Field.h 组合存在重定义冲突（其它批次修复中），
// 此处按 ORIG 符号做最小声明（_ZN12CGameManager13GetQuickPartyEi /
// _ZN12CGameManager26GetQuickPartyRewardManagerEv，非虚函数，链接解析到真实定义）。
#include "CQuickPartyRewardManager.h"
// [dedup] CGameManager 权威定义见 CGameManager.h（namespace WongWork），本 TU 本地桩已删。
CGameManager* G_CGameManager();   // CDungeon.h 同声明

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以
// m_battleField[0x158] 等 pad 占位、未引入 CBattle_Field 类型；Main 裁定
// 本组 TU 局部视图，与 CParty.cpp 口径一致）。布局：pad 0xb24 + BF 0xcd0。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

namespace QuickParty
{

#include "QuickParty/CQuickParty.inc"
// ===================== STQuickPartyPoolMap_Data =====================

#include "QuickParty/STQuickPartyPoolMap_Data.inc"


#include "QuickParty/dtor_STQuickPartyPoolMap_Data.inc"

// ===================== CQuickParty =====================
// 布局依据 ORIG init @0x8269546 与内联访问器 0x826b74e..0x826b807。

#include "QuickParty/dtor_CQuickParty.inc"

#include "QuickParty/get_quick_party_index.inc"

#include "QuickParty/init.inc"


#include "QuickParty/set_quick_party_index.inc"

#include "QuickParty/get_dungeon_index.inc"
#include "QuickParty/get_dungeon_diff.inc"

#include "QuickParty/set_change_quick_party_count.inc"


#include "QuickParty/delete_party_index.inc"


#include "QuickParty/get_quick_party_count.inc"


#include "QuickParty/destroy.inc"


// [moved] GetActiveQuickPartyCnt/GetOccupiedQuickPartyPoolCnt 定义移至 CGameManager.cpp（避免陷入 namespace QuickParty）。

// ===================== CQuickPartySystemManager =====================

#include "QuickParty/CQuickPartySystemManager.inc"


#include "QuickParty/dtor_CQuickPartySystemManager.inc"

#include "QuickParty/init_quick_party_pool.inc"


#include "QuickParty/cancel_quick_party.inc"


}  // namespace QuickParty

// ===================== push/remove_quick_party_in_pool 批次 =====================
// 依赖头：TimerQueue / PacketGuard（ORIG 弱访问器均按反汇编直读字段）。
#include "TimerQueue.h"
#include "PacketGuard.h"

namespace QuickParty
{

// ---- 简单弱访问器（ORIG W 符号，objdump 直读实证） ----

#include "QuickParty/get_party_index.inc"


#include "QuickParty/set_dungeon_index.inc"
#include "QuickParty/set_dungeon_diff.inc"

#include "QuickParty/get_pool_index.inc"


#include "QuickParty/set_pool_index.inc"

#include "QuickParty/get_first_matching_cnt.inc"


#include "QuickParty/is_change_quick_party_count.inc"


#include "QuickParty/get_party_object.inc"


#include "QuickParty/gen_timer_key.inc"


#include "QuickParty/get_matching_quick_party_master_unique_id.inc"


#include "QuickParty/send_matching_update_quick_party.inc"


#include "QuickParty/insert_timer_waiting_party_matching.inc"


#include "QuickParty/remove_quick_party_in_pool.inc"


#include "QuickParty/push_quick_party_in_pool.inc"


}  // namespace QuickParty

// ===================== TailB 批：CQuickParty 访问器 + 系统管理器匹配/开局链 =====================
// 报告：docs/class_func_reports/QuickParty__CQuickParty{,SystemManager}/ 各同名 md。

namespace QuickParty
{

#include "QuickParty/get_timer_key.inc"


#include "QuickParty/check_timer_key.inc"


#include "QuickParty/set_party_index.inc"


#include "QuickParty/set_party_index_1.inc"


#include "QuickParty/get_party_object_1.inc"


#include "QuickParty/delete_party_index_1.inc"


#include "QuickParty/is_empty_quick_party.inc"


#include "QuickParty/set_first_matching_cnt.inc"


#include "QuickParty/find_quick_party_in_pool.inc"


#include "QuickParty/pop_quick_party_in_pool.inc"


#include "QuickParty/regist_quick_party.inc"


#include "QuickParty/check_match_proper_party.inc"


#include "QuickParty/check_quick_party_min_count.inc"


#include "QuickParty/match_proper_party.inc"


#include "QuickParty/make_quick_party.inc"


#include "QuickParty/insert_timer_waiting_start_game.inc"


#include "QuickParty/start_game.inc"


#include "QuickParty/start_game_1.inc"


#include "QuickParty/direct_entrance_quick_party.inc"


}  // namespace QuickParty
