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
        // b28c-lane3：①P222 正向包裹——ORIG 0x0826b832 `jne <共享 false 尾>`
        // 与 0x0826b843 `jge <共享 false 尾>`，否定早返 `!=` 需改写为 `==` 包裹；
        // ②P196 真 bug 修复——ORIG 0x0826b843 是有符号 `jge`，m_dungeonDiff 值域
        // 含 0xff（C1 初值），无符号比较在 ≥0x80 处分叉，按 ORIG 用 signed char 比较。
        if (m_dungeonIndex == rhs.m_dungeonIndex)
        {
            if ((signed char)m_dungeonDiff < (signed char)rhs.m_dungeonDiff)
                return true;
        }
        return false;
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
    // ---- 本批（TailB）补齐的 ORIG-only 访问器 ----
    int get_timer_key(TIMER_MESSAGE msg);             // T @0x08269668
    bool check_timer_key(TIMER_MESSAGE msg, int key); // T @0x082696b0
    void set_party_index(int partyIdx);               // T @0x0826978c（首个空槽写入）
    void set_party_index(int idx, int partyIdx);      // T @0x082697c8（定位写）
    CParty* get_party_object();                       // T @0x0826972c（无参：首个有效成员 party）
    void delete_party_index();                        // T @0x08269856（无参：全部置 -1）
    bool is_empty_quick_party();                      // T @0x08269884
    void set_first_matching_cnt(char cnt);            // W @0x0826b75a

    unsigned short get_matching_quick_party_master_unique_id();  // T @0x08269936
    int gen_timer_key(TIMER_MESSAGE msg);             // T @0x08269680（计数数组 +0x1c 起）
    void send_matching_update_quick_party(char poolIdx,
                                          unsigned short masterUid,
                                          bool changed);          // T @0x08269a8e

    int get_quick_party_index() const;   // +0x10
    void init();
    void set_quick_party_index(int idx);

    short get_dungeon_index();           // W @0x0826b798 (+0x16)
    int get_dungeon_diff();              // W @0x0826b7b4 (+0x18, ORIG movsbl 符号扩展)
    int get_quick_party_count();         // T @0x082698be
    void set_change_quick_party_count(bool flag);  // W @0x0826b7f0 (+0x24)
    bool delete_party_index(int partyIdx);         // T @0x0826980e（命中置 -1 返 true，否则 false）
    void destroy();                      // T @0x082695ca

private:
    int m_partyIndex[4];                 // +0x00（登记的 party index，-1=空）
    int m_quickPartyIndex;               // +0x10（StaticPool 槽位）
    char m_firstMatchingCnt;             // +0x14（ORIG set_first_matching_cnt 0x826b766 截断→this→movzbl 序=P33 char 成员形，uchar 成员为截断→movzbl→this 形 DIFF）
    unsigned char m_pad15;
    short m_dungeonIndex;                // +0x16
    char m_dungeonDiff;                  // +0x18
    char m_poolIndex;                    // +0x19
    int m_timerKeyCnt[2];                // +0x1c（timer key 计数数组，下标 (int)msg-0x9d；get/check/gen_timer_key 实证）
    bool m_isChangeQuickPartyCount;          // +0x24（ORIG 改快速组队人数标记；movzbl 按 bool 读取）
    unsigned char m_pad25[3];
};                                       // sizeof = 0x28

class CQuickPartySystemManager
{
    // ---- 本批（TailB）补齐的匹配/开局内部链（ORIG 均为类成员） ----
    CQuickParty* find_quick_party_in_pool(std::list<int>& pool);   // T @0x0826a9fe
    CQuickParty* pop_quick_party_in_pool(int& outCount,            // T @0x0826ae64
                                         STQuickPartyPoolMap_Data& data,
                                         int slotIdx, int partyIdx,
                                         int firstMatchingCnt);
    bool regist_quick_party(const STQuickPartyPoolMap_Key& key,    // T @0x0826adb6
                            STQuickPartyPoolMap_Data& data, int slotIdx,
                            CParty* party, int firstMatchingCnt);
    int check_match_proper_party(const STQuickPartyPoolMap_Key& key,  // T @0x08269e34
                                 CParty* party);                      // ENUM_ERROR
    bool check_quick_party_min_count(const STQuickPartyPoolMap_Key& key,  // T @0x0826ab9e
                                     int count);
    CParty* make_quick_party(CQuickParty* qp);                     // T @0x0826b222
    void insert_timer_waiting_start_game(CParty* party);           // T @0x0826ad50

public:
    // DspD4 B1：Dispatcher_Register_Quick_Party/Direct_Entrance_Quick_Party
    // process 经 GetQuickPartySystemManager() 公开调用，需 public 可见。
    int match_proper_party(const STQuickPartyPoolMap_Key& key,     // T @0x08269f3c
                           CParty* party, CUser* user);            // ENUM_ERROR
    int direct_entrance_quick_party(CUser* user);                  // T @0x0826a762

public:
    CQuickPartySystemManager();
    ~CQuickPartySystemManager();

    void init_quick_party_pool(const CDungeon* dungeon);
    void insert_timer_waiting_party_matching(CQuickParty* qp);  // T @0x0826acea
    void start_game(CQuickParty* qp);                              // T @0x0826b58a
    bool start_game(CQuickParty* qp, const STQuickPartyPoolMap_Key& key);  // T @0x0826a972
    // [b23S3C] 返回 int→char：ORIG 0x0826a386 尾返 0/1 且调用点（direct_entrance
    // 0x0826a8a6、TimerDispatcher 等）一律 `test %al,%al` 直测低字节，无 int→bool
    // 物化（`test %eax,%eax; setne %al`）；mangled 不含返回类型 ⇒ 符号不变。
    // 实测 char 仍产 `test %al,%al; setne %al; test` 两级归一，bool 才出单条直测。
    bool cancel_quick_party(CParty* party, bool flag, CUser* user);  // ORIG 返回 0/1
    bool remove_quick_party_in_pool(CQuickParty* qp, const STQuickPartyPoolMap_Key& key);   // T @0x0826aa9a（后续批次实现）
    bool push_quick_party_in_pool(const STQuickPartyPoolMap_Key& key, STQuickPartyPoolMap_Data& data,
                                  CParty* party, int count, CQuickParty* qp, bool flag);    // T @0x0826afb4（后续批次实现）

private:
    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data> m_poolMap;  // +0x00
    // ORIG sizeof 0x18（GetQuickPartySystemManager _Znwj(0x18) 实证），仅 m_poolMap。
};

// ---- RewardData_Param/Result、IRewardType 奖励族、QuickPartyData 权威定义
// ---- 已迁至 CQuickPartyRewardManager.h（经 CParty.h 引入；本头不再重复）。
#include "CQuickPartyRewardManager.h"

 // 完整 CQuickPartySystemManager 已定义（供 CDungeon.h 等本地最小声明守卫，
 // 与 include 顺序无关）。
 #define DNF_QUICKPARTY_SYSTEMMANAGER_FULL 1

}  // namespace QuickParty

#endif  // QUICKPARTY_H
