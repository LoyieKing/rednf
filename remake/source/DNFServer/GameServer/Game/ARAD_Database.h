// ============================================================================
// ARAD::DATABASE::CREATE_QUERY / LOGIN_QUERY 与 ARAD::Arad_DB_Charac_Dispatcher
// ORIG df_game_r 0x08184114..0x0818cc74+（Arad_Character.cpp / Arad_DB_Query.cpp）。
// 参照 docs/class_func_reports/ARAD__DATABASE__CREATE_QUERY/、
// ARAD__DATABASE__LOGIN_QUERY/、ARAD__Arad_DB_Charac_Dispatcher/ 全部报告转写。
// 实现：ARAD_Database.cpp（本批 W6_ARAD.DbQueries 独占 TU）。
//
// 附带（本批唯一调用链 producer，ORIG 弱符号无人认领时由本 TU 提供定义）：
//   WongWork::CEventMgr::GetCoinCharacCreate   T 0x0848f002（声明点仍在 CEventMgr.h，
//     若共享头尚缺声明，先于本头 extern 声明前向补齐——见 cpp 顶部说明）
//   CCoinEventOnCharCreate 全类 0x0810a914..0x0810aa9c（GetCoinCharacCreate 上游依赖）
// ============================================================================
#ifndef DNF_GAME_ARAD_DATABASE_H_
#define DNF_GAME_ARAD_DATABASE_H_

#include <vector>

class MySQL;
class Stream;
struct SIG_CREATE_CHARAC;
struct SIG_LOGIN_DATA;

#include "CGuildServerProxy.h"   // CEventBase / Word_Param / CEventManager
#include "DB_Login.h"            // DB_Login::tagDNFEventEntry / vecDNFEventEntry_t

namespace ARAD
{

namespace DATABASE
{
// ORIG 死常量（头文件 static，逐 TU 一份；值按 .rodata 实测 _ZN4ARAD8DATABASEL*E）：
static const unsigned int sourceInvenSize = 0x3b55;     // 角色背包快照长度
static const unsigned int sourceCargoSize = 0x2438;     // 仓库快照长度（0x98 槽 * 0x3d）
static const unsigned int sourceJewelSize = 0xf03;      // 珠宝快照长度
static const unsigned int sourceEquipSize = 0x2dc;      // 装备区快照长度
static const unsigned int sourceCreatureSize = 0x184e;  // 宠物区快照长度

// ---- CREATE_QUERY：角色创建 DB 写入族（全 static，无 ctor/dtor 符号）----
class CREATE_QUERY
{
public:
    // 返回 0 成功；-1 失败（同时写 SIG_CREATE_CHARAC +0x5344 错误码）
    static int arad_sp_charater_name_check(MySQL* db, SIG_CREATE_CHARAC* sig,
                                           char const* dbName);          // 0x0818ac54
    static int arad_sp_get_characer_index(MySQL* db, SIG_CREATE_CHARAC* sig);        // 0x0818ad8b
    static int arad_sp_create_character_check(MySQL* db, SIG_CREATE_CHARAC* sig);    // 0x0818aea2
    static int arad_sp_create_character(MySQL* db, SIG_CREATE_CHARAC* sig);          // 0x0818af9a
    static int arad_sp_create_charac_stat(MySQL* db, SIG_CREATE_CHARAC* sig);        // 0x0818b26d
    static int arad_sp_create_inven(MySQL* db, SIG_CREATE_CHARAC* sig);              // 0x0818b2b7
    static int arad_sp_create_expand_inven(MySQL* db, SIG_CREATE_CHARAC* sig);       // 0x0818b406
    static int arad_sp_create_equip(MySQL* db, SIG_CREATE_CHARAC* sig);              // 0x0818b443
    static int arad_sp_create_skill(MySQL* db, SIG_CREATE_CHARAC* sig);              // 0x0818b588
    static int arad_sp_create_pvp(MySQL* db, SIG_CREATE_CHARAC* sig);                // 0x0818b5f4
    static int arad_sp_create_dungeon_point(MySQL* db, SIG_CREATE_CHARAC* sig);      // 0x0818b638
    static int arad_sp_create_charac_dungeon(MySQL* db, SIG_CREATE_CHARAC* sig);     // 0x0818b642
    static int arad_sp_create_charac_quest(MySQL* db, SIG_CREATE_CHARAC* sig);       // 0x0818b68c
    static int arad_sp_create_npc_relationship(MySQL* db, SIG_CREATE_CHARAC* sig);   // 0x0818b6c9
};

// ---- LOGIN_QUERY：登录期账号/惩罚/GoblinPad 查询族（全 static）----
class LOGIN_QUERY
{
public:
    typedef std::vector<int> vecPremiumRemoveInfo_t;

    static int arad_sp_select_account_info(SIG_LOGIN_DATA* data, char* passwd);      // 0x0818b706
    static int arad_sp_select_punish_user_info(SIG_LOGIN_DATA* data);                // 0x0818bd67
    static int arad_sp_select_punish_second_user(SIG_LOGIN_DATA* data);              // 0x0818bfb0
    static int arad_sp_convert_pass_pad_info(SIG_LOGIN_DATA* data);                  // 0x0818c0e0
    static int arad_sp_delete_member_mouspass(SIG_LOGIN_DATA* data);                 // 0x0818c609
    static int arad_sp_update_fail_record(SIG_LOGIN_DATA* data);                     // 0x0818c6c4
    static int arad_sp_update_history(SIG_LOGIN_DATA* data, int modifyType);         // 0x0818c76f
    static int arad_sp_check_goblin_reward(SIG_LOGIN_DATA* data, char* sinceTime);   // 0x0818c4f3
    static int arad_sp_load_event_entry(
        SIG_LOGIN_DATA* data,
        std::vector<DB_Login::tagDNFEventEntry>& out);                               // 0x0818c8b8
    static int arad_sp_select_member_brodcast(
        SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry, int userIdx);       // 0x0818ca69
    static int arad_sp_select_member_premium(
        SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry,
        std::vector<int>& removeList, std::vector<int> removeScratch);               // 0x0818cc74
};

} // namespace DATABASE

// ---- 角色创建 DB 主 dispatcher（vtable 仅 [dispatch]，ORIG 0x8b8d870 实证：
//      无虚析构槽；隐式 trivial 析构即可复现）----
class Arad_DB_Charac_Dispatcher
{
public:
    virtual int dispatch(int param1, int param2, Stream* stream);                    // 0x08184114

    int create_charac(SIG_CREATE_CHARAC* sig);                   // 0x08184290
    int create_charac_stat(SIG_CREATE_CHARAC* sig);              // 0x08184342
    int create_inventory(SIG_CREATE_CHARAC* sig);                // 0x081843d2
    int create_equip(SIG_CREATE_CHARAC* sig);                    // 0x08184484
    int create_skill(SIG_CREATE_CHARAC* sig);                    // 0x08184528
    int create_pvp(SIG_CREATE_CHARAC* sig);                      // 0x081845b8
    int create_charac_dungeon(SIG_CREATE_CHARAC* sig);           // 0x08184648
    int create_charac_quest(SIG_CREATE_CHARAC* sig);             // 0x081846d8
    int create_charac_npc_relationship(SIG_CREATE_CHARAC* sig);  // 0x08184768
};

} // namespace ARAD

// ============================================================================
// CCoinEventOnCharCreate —— 角色创建送币 repeat 事件（event idx 5；
// GetRepeatEvent(5) 命中后决定新角色初始 coin 数）。归属事件域，
// 本批按 objdump 全量转写以闭合 WongWork::CEventMgr::GetCoinCharacCreate 调用链；
// 若事件域后续建权威头/TU，应迁走并删除本 TU 对应段。
// ============================================================================
class CCoinEventOnCharCreate : public CEventBase
{
public:
    CCoinEventOnCharCreate();                       // C1/C2 T 0x0810a914
    ~CCoinEventOnCharCreate();                      // D1/D2 T 0x0810a93a

    virtual void StartEvent();                      // T 0x0810a988（vtable+0x10）
    virtual void StartEvent(Word_Param param);      // T 0x0810a9e4（vtable+0x18）
    virtual void EndEvent();                        // T 0x0810aa42（vtable+0x14）

    int GetCoinNoOnCreateCharac();                  // W 0x0848f200（读 this+0xc）

private:
    int m_coin;                                     // +0x0c（ctor 置 5）
};

#endif // DNF_GAME_ARAD_DATABASE_H_
