// df_game_r 还原 —— QuickParty 独立类声明（batch-5 迁移自 GameStubs.cpp）。
// 权威符号（ORIG df_game_r）：
//   QuickParty::CQuickParty::CQuickParty()                     T @0x0826952c
//   QuickParty::CQuickParty::~CQuickParty()                    T @0x08269540
//   QuickParty::CQuickParty::get_quick_party_index() const     W @0x0826b74e
//   QuickParty::CQuickParty::init()                            T @0x08269546
//   QuickParty::CQuickParty::set_quick_party_index(int)        W @0x082a69be
//   QuickParty::CQuickPartySystemManager::CQuickPartySystemManager()       T @0x08269bb6
//   QuickParty::CQuickPartySystemManager::~CQuickPartySystemManager()      T @0x08269bf8
//   QuickParty::CQuickPartySystemManager::init_quick_party_pool(CDungeon const*)  T @0x08269c0c
//   QuickParty::CQuickPartySystemManager::cancel_quick_party(CParty*,bool,CUser*) T @0x0826a386

#ifndef QUICKPARTY_H
#define QUICKPARTY_H

#include <list>
#include <map>

#include "CParty.h"   // TIMER_MESSAGE（CQuickParty::gen_timer_key 签名需要）

class CDungeon;
class CParty;
class CUser;

namespace QuickParty
{

// STQuickPartyPoolMap_Key（ORIG C1 @0x082344ca W：short@0=0、char@2=0xff；lt @0x0826b808）
struct STQuickPartyPoolMap_Key
{
    STQuickPartyPoolMap_Key()
        : m_dungeonIndex(0), m_dungeonDiff(0xff) {}

    bool operator<(const STQuickPartyPoolMap_Key& rhs) const
    {
        if (m_dungeonIndex < rhs.m_dungeonIndex) return true;
        if (rhs.m_dungeonIndex < m_dungeonIndex) return false;
        return (unsigned char)m_dungeonDiff < (unsigned char)rhs.m_dungeonDiff;
    }

    short m_dungeonIndex;        // +0x00
    unsigned char m_dungeonDiff; // +0x02
};                               // sizeof = 4

// STQuickPartyPoolMap_Data（ORIG C1 @0x0826b854 W / D1 @0x0826b9d2 W：
// 4 个 std::list<int>（各 8 字节），构造后全部 clear；sizeof = 32）
struct STQuickPartyPoolMap_Data
{
    STQuickPartyPoolMap_Data();
    ~STQuickPartyPoolMap_Data();

    std::list<int> m_lists[4];   // +0x00..0x1f（语义：按难度档位的 party index 池，推断）
};                               // sizeof = 32

class CQuickParty
{
public:
    CQuickParty();
    ~CQuickParty();
    // ---- push/remove_quick_party_in_pool 依赖访问器（ORIG 弱符号，字段直读） ----
    int get_party_index(int idx);                     // T @0x082697ec
    void set_dungeon_index(short idx);                // W @0x0826b77e (+0x16)
    void set_dungeon_diff(int diff);                  // W @0x0826b7a4 (+0x18)
    int get_pool_index();                             // W @0x0826b7c4 (+0x19)
    void set_pool_index(int idx);                     // W @0x0826b7c4 区
    char get_first_matching_cnt();                    // W @0x0826b75a (+0x14)
    bool is_change_quick_party_count();               // W @0x0826b7e4 (+0x24)
    CParty* get_party_object(int idx);                // T @0x082696d0
    unsigned short get_matching_quick_party_master_unique_id();  // T @0x08269936
    int gen_timer_key(TIMER_MESSAGE msg);             // T @0x08269680（计数数组 +0x1c 起）
    void send_matching_update_quick_party(char poolIdx,
                                          unsigned short masterUid,
                                          bool changed);          // T @0x08269a8e

    int get_quick_party_index() const;   // +0x10
    void init();
    void set_quick_party_index(int idx);

    short get_dungeon_index();           // W @0x0826b798 (+0x16)
    char get_dungeon_diff();             // W @0x0826b7b4 (+0x18)
    int get_quick_party_count();         // T @0x082698be
    void set_change_quick_party_count(bool flag);  // W @0x0826b7f0 (+0x24)
    void delete_party_index(int partyIdx);         // T @0x0826980e
    void destroy();                      // T @0x082695ca

private:
    int m_partyIndex[4];                 // +0x00（登记的 party index，-1=空）
    int m_quickPartyIndex;               // +0x10（StaticPool 槽位）
    unsigned char m_firstMatchingCnt;    // +0x14
    unsigned char m_pad15;
    short m_dungeonIndex;                // +0x16
    char m_dungeonDiff;                  // +0x18
    char m_poolIndex;                    // +0x19
    int m_field1c;                       // +0x1c（timer key 计数数组槽 0，gen_timer_key 实证）
    int m_field20;                       // +0x20（timer key 计数数组槽 1，msg 0x9e 用）
    unsigned char m_isChangeQuickPartyCount;  // +0x24
    unsigned char m_pad25[3];
};                                       // sizeof = 0x28

class CQuickPartySystemManager
{
public:
    CQuickPartySystemManager();
    ~CQuickPartySystemManager();

    void init_quick_party_pool(const CDungeon* dungeon);
    void insert_timer_waiting_party_matching(CQuickParty* qp);  // T @0x0826acea
    int cancel_quick_party(CParty* party, bool flag, CUser* user);  // ORIG 返回 0/1
    bool remove_quick_party_in_pool(CQuickParty* qp, const STQuickPartyPoolMap_Key& key);   // T @0x0826aa9a（后续批次实现）
    bool push_quick_party_in_pool(const STQuickPartyPoolMap_Key& key, STQuickPartyPoolMap_Data& data,
                                  CParty* party, int count, CQuickParty* qp, bool flag);    // T @0x0826afb4（后续批次实现）

private:
    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data> m_poolMap;  // +0x00
    unsigned char m_padTail[0xd0];       // 至 ORIG 对象大小 0x100（其余成员未还原）
};

// ---- RewardData_Param/Result、IRewardType 奖励族、QuickPartyData 权威定义
// ---- 已迁至 CQuickPartyRewardManager.h（经 CParty.h 引入；本头不再重复）。
#include "CQuickPartyRewardManager.h"

 // 完整 CQuickPartySystemManager 已定义（供 CDungeon.h 等本地最小声明守卫，
 // 与 include 顺序无关）。
 #define DNF_QUICKPARTY_SYSTEMMANAGER_FULL 1

}  // namespace QuickParty

#endif  // QUICKPARTY_H
