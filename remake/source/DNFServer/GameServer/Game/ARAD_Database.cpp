// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ARAD_Database/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "ARAD_Database.h"

#include "MySQL.h"
#include "Stream.h"
#include "SigTypes.h"
#include "GlobalData.h"          // GlobalData::s_db_mgr / s_systemTime_
#include "DBMgr.h"               // DBMgr::GetDBHandle / addUserDBInfo / stUserDBInfo_t
#include "CEnvironment.h"        // G_CEnvironment / STDBConnInfo
#include "CEventMgr.h"           // WongWork::CEventMgr（声明经 Main 收口）
#include "AdvanceAltarZip.h"     // compress_zip
#include "GameTypes.h"           // cMyTrace
#include "CSystemTime.h"
#include "GameWorld.h"           // G_GameWorld / GameWorld::GetChannelType
#include "LogManager.h"

extern int DEFAULT_MAX_FATIGUE;  // CUserCharacInfo.h 声明的全局（ORIG B）

namespace OS_API
{
#include "ARAD_Database/snprintf.inc"
}

namespace
{

// 【ABI 镜像】SIG_CREATE_CHARAC 尾部两个 ORIG 实证字段（错误码/charac_no），
// W6TB_DbCreateCharac 正在 SigTypes.h 具名化为 m_resultCode(+0x5344)/m_characNo(+0x5348)。
inline unsigned int& create_result_code(SIG_CREATE_CHARAC* sig)
{
    return sig->m_resultCode;    // +0x5344（W6TB_DbCreateCharac 已具名化）
}
// 【ABI 镜像】POD 区未建模单值字段直访（SIG_LOGIN_DATA 在 SigTypes.h 仅 char m_pad[0x4358]；
// SIG_CREATE_CHARAC 内散列属性同样适用）。
// 宏而非模板函数：ORIG(4.1) 对该访问无外联 call 且无形参槽往返
//（mov 0x3914(%eax),%eax 单条实证 0x0818c6c4），函数形（含 always_inline）
// 在 4.4-O0 下均发射 call 或 base/off 槽。指针加法生 add+mov 两步，须用
// 类型化数组下标折叠单条 disp。
#define LOGIN_FIELD(T, base, off) \
    (reinterpret_cast<T*>(base)[(off) / sizeof(T)])

// 【ABI 镜像】SIG_CREATE_CHARAC 主属性区具名视图（ORIG 0x0818af9a 各
//   `mov off(%eax)` disp 单条实证；pack(1) 使 +0x26 等非对齐 int 折叠 disp，
//   替代 LOGIN_FIELD 指针加法在 4.4 下多出的 add+mov 两步形）。POD 无符号发射。
#pragma pack(push, 1)
struct SigCreateCharacAttrView
{
    char m_pad00[0x26];
    unsigned int   m_maxHP;      // +0x26
    unsigned int   m_maxMP;      // +0x2a
    unsigned short m_phyAtk;     // +0x2e
    unsigned short m_phyDef;     // +0x30
    unsigned short m_magAtk;     // +0x32
    unsigned short m_magDef;     // +0x34
    char m_pad36[0x60 - 0x36];   // +0x36..+0x5f（spec_property 8B / element 0x22B 区）
    unsigned int   m_invenWt;    // +0x60
    unsigned short m_hpRegen;    // +0x64
    unsigned short m_mpRegen;    // +0x66
    unsigned int   m_moveSpd;    // +0x68
    unsigned short m_atkSpd;     // +0x6c
    unsigned short m_castSpd;    // +0x6e
    unsigned short m_hitRecov;   // +0x70
    unsigned short m_jump;       // +0x72
    unsigned int   m_chaWt;      // +0x74
};
#pragma pack(pop)

#define SIG_ATTR(sig, field) \
    (reinterpret_cast<const SigCreateCharacAttrView*>(sig)->field)

} // namespace

namespace ARAD
{

namespace DATABASE
{

// ===========================================================================
// CREATE_QUERY（Arad_DB_Query.cpp）
// ===========================================================================

#include "ARAD_Database/arad_sp_charater_name_check.inc"


#include "ARAD_Database/arad_sp_get_characer_index.inc"


#include "ARAD_Database/arad_sp_create_character_check.inc"


#include "ARAD_Database/arad_sp_create_character.inc"

#include "ARAD_Database/arad_sp_create_charac_stat.inc"


#include "ARAD_Database/arad_sp_create_inven.inc"


#include "ARAD_Database/arad_sp_create_expand_inven.inc"


#include "ARAD_Database/arad_sp_create_equip.inc"


#include "ARAD_Database/arad_sp_create_skill.inc"


#include "ARAD_Database/arad_sp_create_pvp.inc"


#include "ARAD_Database/arad_sp_create_dungeon_point.inc"


#include "ARAD_Database/arad_sp_create_charac_dungeon.inc"


#include "ARAD_Database/arad_sp_create_charac_quest.inc"


#include "ARAD_Database/arad_sp_create_npc_relationship.inc"

// ===========================================================================
// LOGIN_QUERY（Arad_DB_Query.cpp）。ORIG 各查询经 DBMgr::GetDBHandle 取句柄。
// ===========================================================================

#include "ARAD_Database/arad_sp_select_account_info.inc"


#include "ARAD_Database/arad_sp_select_punish_user_info.inc"


#include "ARAD_Database/arad_sp_select_punish_second_user.inc"


#include "ARAD_Database/arad_sp_convert_pass_pad_info.inc"


#include "ARAD_Database/arad_sp_delete_member_mouspass.inc"


#include "ARAD_Database/arad_sp_update_fail_record.inc"


#include "ARAD_Database/arad_sp_update_history.inc"


#include "ARAD_Database/arad_sp_check_goblin_reward.inc"


#include "ARAD_Database/arad_sp_load_event_entry.inc"


#include "ARAD_Database/arad_sp_select_member_brodcast.inc"


#include "ARAD_Database/arad_sp_select_member_premium.inc"

} // namespace DATABASE

// ===========================================================================
// Arad_DB_Charac_Dispatcher（Arad_Character.cpp）
// ===========================================================================

// 各 create_* 共通形态：GetDBHandle(idx) → CREATE_QUERY::arad_sp_xxx。
// ORIG 行号：0x4e/0x67/0x7b/0x95/0xb0/0xbf/0xcd/0xdb/0xe9。

// [SplitC 2026-09-11 batch3 · TU 迁移] dispatch 已迁出至 Arad_Character.cpp（ORIG GI 键对齐）。



#include "ARAD_Database/create_charac.inc"


#include "ARAD_Database/create_charac_stat.inc"


#include "ARAD_Database/create_inventory.inc"


#include "ARAD_Database/create_equip.inc"


#include "ARAD_Database/create_skill.inc"


#include "ARAD_Database/create_pvp.inc"


#include "ARAD_Database/create_charac_dungeon.inc"


#include "ARAD_Database/create_charac_quest.inc"


#include "ARAD_Database/create_charac_npc_relationship.inc"


} // namespace ARAD

// ===========================================================================
// WongWork::CEventMgr::GetCoinCharacCreate —— ORIG T 0x0848f002 objdump 转写。
// GetRepeatEvent(5) 且事件 IsEventing → CCoinEventOnCoinCharCreate::GetCoinNoOnCreateCharac；
// 否则默认 5。
// b28d-solo-ARAD_Database N7 封顶：ORIG 0x0848f045 `mov (%eax),%edx; …; call *%edx`（目标 %edx、this
//   %eax），4.4.7 恒目标 %eax、this %edx（间接 call 族，P230补：4.4.6 后端才出
//   %edx 形）。本 TU 未路由 446（改路由表属 Main 权限，禁自改）；源形不可择 ⇒ N7
//   封顶，语义等价（虚槽 +0x34 = IsEventing 已核）。
// ===========================================================================
namespace WongWork
{

#include "ARAD_Database/GetCoinCharacCreate.inc"


} // namespace WongWork

// ===========================================================================
// CCoinEventOnCharCreate —— ORIG 0x0810a914..aa9c objdump 全量转写。
// ===========================================================================

// [SplitC 2026-09-11 batch2 · TU 迁移] CCoinEventOnCharCreate 已迁出至 CoinEventOnCharCreate.cpp（ORIG GI 键对齐）。



#include "ARAD_Database/dtor_CCoinEventOnCharCreate.inc"
                                                    // ORIG 仅恢复 vptr + 调基类析构

#include "ARAD_Database/StartEvent.inc"


#include "ARAD_Database/StartEvent_1.inc"


#include "ARAD_Database/EndEvent.inc"


#include "ARAD_Database/GetCoinNoOnCreateCharac.inc"

