// df_game_r Game/ EventClassify::CEventScriptMng。
// ORIG 布局（CEventScriptMng::CEventScriptMng 0x810b136）：
//   +0x00 InGameEventInfoList（基类 1，map<int,InGameEventInfo>，0x18）
//   +0x18 InGameEventScriptLoader（基类 2，map<string,InGameEventScript*>，0x18）
//   +0x30 map<int,CEventEntity*>（0x18）| +0x48 int | +0x4c byte | +0x50 vector<int>，
//   sizeof = 0x5c。
// 本工具链 std::map 为 0x18、std::vector 为 0xc（已用 sizeof 探针确认）。
#ifndef DNF_CEVENTSCRIPTMNG_H
#define DNF_CEVENTSCRIPTMNG_H

#include <bitset>
#include <map>
#include <string>
#include <vector>

#include "InGameEventScripts.h"   // 具体事件脚本类型（PurchaseCashItemBonusScript 等）

class CUser;
class CUser;
class PacketGuard;
class InGameEventScript;
class CItem;
class Inven_Item;
struct stCeraShopIPGRequest_t;

// ---- InGameEventInfo / InGameEventInfoList：ORIG 为全局作用域类型
// （_Z25importInGameEventInfoListP19InGameEventInfoListPKc 实证），
// 自 namespace EventClassify 内上移收敛（CDataManager::init 依赖该全局型）。
class InGameEventInfo
{
public:
    InGameEventInfo();
    ~InGameEventInfo();
    InGameEventInfo(const InGameEventInfo&);
    InGameEventInfo& operator=(const InGameEventInfo&);

    int m_eventId;               // +0x00
    int m_field4;                // +0x04
    std::string m_name;          // +0x08
    int m_startTime;             // +0x0c
    int m_endTime;               // +0x10
    unsigned char m_isEventing;  // +0x14
};

class InGameEventInfoList
{
public:
    InGameEventInfoList();
    void insertEventInfo(int id, InGameEventInfo& info);   // ORIG 0x0897eb30 T

    std::map<int, InGameEventInfo> m_info;   // +0x00，sizeof 0x18
};

// InGameEventScriptLoader —— 基类 2。+0x00 map<string,InGameEventScript*>（std::map=0x18）。
class InGameEventScriptLoader
{
public:
    InGameEventScriptLoader();
    // ORIG 0x08982f10 T：非空则 m_scripts[name] = script；返回 script。
    InGameEventScript* insertEventScript(const std::string& name, InGameEventScript* script);

    // 本工具链 std::map 为 0x18，即基类 2 整体 0x18。
    std::map<std::string, InGameEventScript*> m_scripts;  // +0x00，sizeof 0x18
    // ORIG _ZN23InGameEventScriptLoader14getEventScriptEiRKSs 0x08982f38 T
    // （id→脚本查找；initScriptlist 被调。声明先行，定义归脚本装载批次）
    InGameEventScript* getEventScript(int id, const std::string& name);
    // ORIG _ZN23InGameEventScriptLoader23importInGameEventScriptEiRKSs 0x08982a4a T
    // （按 id 导入 .txt 脚本；声明先行，定义归 InGameEventScript 装载批次，
    //   需先落地 InGameEventScript 完整类型）
    InGameEventScript* importInGameEventScript(int id, const std::string& name);
};

namespace EventClassify
{

class CEventEntity
{
public:
    CEventEntity();                                   // 0x0810af40
    virtual ~CEventEntity();                          // D1 0x810afc4 / D0 0x810aff8
    // ORIG 访问器（偏移见成员声明）：
    virtual bool is_eventing();                       // 0x810b016：return +0x04 字节
    virtual void set_event_flag(bool flag);           // 0x810b022：写 +0x04 字节
    virtual void set_event_data(int key, InGameEventScript* script); // 0x810b03a：写 +0x18/+0x14
    virtual int get_event_record_type();              // 0x810b052：return +0x18
    virtual void set_event_record_type(int type);     // 0x810b05e：写 +0x18
    virtual bool send_init_data(CUser* user, PacketGuard& packet,
                                short key, unsigned char count,
                                bool flag);           // 0x810b0ac：基类 return 0
    // ORIG 权威虚表（_ZTVN13EventClassify12CEventEntityE @0x8b4d840，vptr=+0x8）：
    //   +0x00 D1 | +0x04 D0 | +0x08 is_eventing | +0x0c set_event_flag |
    //   +0x10 set_event_data | +0x14 get_event_record_type | +0x18 set_event_record_type |
    //   +0x1c send_init_data | +0x20 get_db_table_name。
    // send_init_data 位于 vptr+0x1c（与当前声明一致）；get_db_table_name 为
    // vptr+0x20 的虚函数（派生类 CAttendance/CLevelUpReward/CObjectBringUp/
    // CAddItemCountDB 均覆写），必须声明为 virtual。4 个时间访问器
    // （0x810b094/0x810b078/0x810b0a0/0x810b086）确认非虚，虚表无 process 槽。
    virtual std::string& get_db_table_name();                // 0x810b06c：return (char*)&m_dbTableName（+0x08）
    int   get_event_start_time();                     // return m_startTime（+0x0c）
    void  set_event_start_time(int t);                // m_startTime = t
    int   get_event_end_time();                       // return m_endTime（+0x10）
    void  set_event_end_time(int t);                  // m_endTime = t

protected:
    unsigned char       m_isEventing;     // +0x04
    std::string         m_dbTableName;    // +0x08
    int                 m_startTime;      // +0x0c
    int                 m_endTime;        // +0x10
    InGameEventScript*  m_script;         // +0x14
    int                 m_recordType;     // +0x18
};

// ============================================================================
// EventClassify 事件实体（CEventEntity 派生类）。ORIG 统一布局：
//   基类 0x1c + 私有 InGameEventScript*（+0x1c）= sizeof 0x20。
// create_event_entity（0x810b402）按事件记录类型 value-init 对应类：
//   operator new(0x20) → 整块清零循环 → C1 thunk。
// ORIG 各派生 ctor 符号均为 W（隐式生成：调基类 CEventEntity ctor + 设 vptr），
// 除 CAccountFirstLogin 外均不声明 ctor，由编译器同样产生清零循环 + 弱 C1。
// 脚本访问一律经 InGameEventScripts.h 具体脚本类型的具名字段，禁止裸偏移。
// ============================================================================

// CLevelUpReward ← 事件记录类型 0x194（脚本 LevelUpRewardScript）。
// get_db_table_name 返回 &m_dbTableName（调用方按 std::string& 解释），
// get_mail_data 返回 &m_mail。ORIG ctor W 隐式（C1 0x081110fc），不声明。
class CLevelUpReward : public CEventEntity
{
public:
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810e168
    virtual std::string& get_db_table_name();               // slot 8 0x810e2d6：&m_dbTableName
    char  is_recv_check_item();                      // 0x810e2f2：m_flag08
    char* get_mail_data();                           // 0x810e2e4：&m_mail
    int   set_reward_item(int, int, int);            // 0x810e302（定义 EventLevelupA2）
    int   check_recv_level(int, int, int, bool);     // 0x810e192
    void  do_action(CUser* user);                    // 0x810e4ea（非虚）
    int   send_reward_from_recv_level(CUser*, int, int);  // 0x810e51a
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CUsedFatigueGiveItem ← 事件记录类型 0x65（脚本 UsedFatigueGiveItemScript）。
// 虚表覆写 is_eventing(slot2)/set_event_data(slot4)/get_db_table_name(slot8)；
// get_db_table_name 返回 &m_dbTableName。ORIG ctor W 隐式（0x081110a0），不声明。
class CUsedFatigueGiveItem : public CEventEntity
{
public:
    virtual bool is_eventing();                      // slot 2 0x811056e
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x81105be
    virtual std::string& get_db_table_name();               // slot 8 0x81105e8：m_script+8
    bool  getIsCharacEvent(bool& out) const;         // 0x81107ea
    char  get_user_type() const;                     // 0x8110eec（ZNK，读脚本 +0xc）
    void  sendEventItemFromFatigue(CUser*, unsigned int);  // 0x81105f6
    int   verifyApplyUser(CUser*);                   // 0x8110756
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAttendance ← 事件记录类型 0x56（脚本 AttendanceScript）。虚表覆写 is_eventing
// (slot2)/set_event_data(slot4)/send_init_data(slot7)/get_db_table_name(slot8)；
// get_db_table_name 返回 &m_dbTableName，get_mail_data 返回 &m_mail。
// ORIG ctor W 隐式（0x08110fdc），不声明。
class CAttendance : public CEventEntity
{
public:
    virtual bool is_eventing();                      // slot 2 0x810e75c
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810eadc
    virtual bool send_init_data(CUser* user, PacketGuard& packet,
                                short key, unsigned char count, bool flag);  // slot 7 0x810e804
    virtual std::string& get_db_table_name();               // slot 8 0x810e7cc：m_script+8
    char* get_mail_data();                           // 0x810e7da：m_script+0x2c
    int   get_fatigue_condition();                   // 0x810e7e8：*(m_script+0xc)
    int   get_final_day_condition();                 // 0x810e7f6：*(m_script+0x24)
    void  send_attend_reward_item(CUser*, int);      // 0x810e878
    void  send_attend_final_rward_item(CUser*, int, int);  // 0x810e9c0
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CPcRoomPlayTime ← 事件记录类型 0x5c（脚本 PcBangPlayTimeScript）。虚表覆写
// is_eventing(slot2)/set_event_data(slot4)/get_db_table_name(slot8)；
// get_db_table_name 返回 &m_dbTableName。ORIG ctor W 隐式（0x08111084），不声明。
class CPcRoomPlayTime : public CEventEntity
{
public:
    virtual bool is_eventing();                      // slot 2 0x8110814
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x8110872
    virtual std::string& get_db_table_name();               // slot 8 0x8110864：m_script+8
    int   processEventPcRoomPlayTime(CUser*);        // 0x811089c
    int   verifyUserPcRoomPlayEvent(CUser*);         // 0x8110a38
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAccountFirstLogin ← 事件记录类型 0x191/0x192（脚本 AccountFirstLoginScript）。
// 虚表覆写 set_event_data(slot4)/get_db_table_name(slot8)，并新增虚槽 slot 9 =
// do_action。get_db_table_name 返回 &m_dbTableName，get_mail_data 返回 &m_mail。
// CAccountFirstLoginDaily 由此类派生。ORIG ctor 为 W 隐式（0x08110f88）；此处
// 保留显式声明仅因 entitiesB 的派生类需显式调用基类构造，ctor 内补 m_script1c=0
// 对齐 ORIG 工厂清零终态。
class CAccountFirstLogin : public CEventEntity
{
public:
    CAccountFirstLogin();                            // 0x08110f88
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810deda
    virtual std::string& get_db_table_name();               // slot 8 0x810debe：m_script+8
    virtual void do_action(int flags, CUser* user, std::bitset<32u> bs);  // slot 9 0x810df04
    char* get_mail_data();                           // 0x810decc：m_script+0xc
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAddItemCountDB ← 事件记录类型 0x193（脚本 AddItemCountDBScript）。虚表覆写
// set_event_data(slot4)/get_db_table_name(slot8)；get_db_table_name 返回
// &m_dbTableName。ORIG ctor W 隐式（0x08110fa4），不声明。
class CAddItemCountDB : public CEventEntity
{
public:
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810e064
    virtual std::string& get_db_table_name();               // slot 8 0x810e15a：m_script+0x20
    int is_event_item(int itemId);                   // 0x810e08e
    int is_event_quest(int questId);                 // 0x810e0f4
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// ============================================================================
// 第二组实体（FixEventEntitiesB）。脚本导入路径：set_event_data 绑定脚本至 +0x1c，
// get_db_table_name 返回绑定脚本的 db_table_name。各运行时奖励/校验方法依赖
// ObjectBringUpScript/PurchaseCashItemBonusScript/stCeraShopIPGRequest_t/
// CEventActionMng 等未建模类型，待 FixEventScriptDeps 建模后实现。
// ============================================================================

// CFatigueQuantity ← 疲劳量事件。虚表覆写 set_event_data(slot4)/get_db_table_name(slot8)。
class CFatigueQuantity : public CEventEntity
{
public:
    CFatigueQuantity();                              // 0x08111030
    virtual ~CFatigueQuantity();                     // D1 0x08114aac / D0 0x08114adc
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810f95e
    virtual std::string& get_db_table_name();               // slot 8 0x810f988：m_script+8
    char get_user_type() const;                       // 0x08110edc（ZNK const）
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CExchangeRandomItemReward ← 交换随机奖励事件。虚表覆写 set_event_data(slot4)。
class CExchangeRandomItemReward : public CEventEntity
{
public:
    CExchangeRandomItemReward();                     // 0x08110ff8
    virtual ~CExchangeRandomItemReward();            // D1 0x08114b48 / D0 0x08114b78
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810eb06
    int  getRewardItem(std::vector<std::pair<int, int> >& out);   // 0x0810ee40
    int  verifyMaterialItemByRule(CUser* user,
                                  std::vector<int>& slots,
                                  std::vector<std::pair<int, int> >& materials);  // 0x0810eb30
    int  deleteUsedMaterialItem(CUser* user,
                                std::vector<std::pair<int, int> >& materials,
                                std::vector<std::pair<int, int> >& returnItems);  // 0x0810eede
    int  sendRandomItemReward(CUser* user,
                              std::vector<std::pair<int, int> >& rewards,
                              std::vector<int>& recordVec,
                              unsigned char& flag);             // 0x0810f26e
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAvatarDisjointRandomReward ← 头像分解随机奖励事件。虚表覆写 set_event_data(slot4)。
class CAvatarDisjointRandomReward : public CEventEntity
{
public:
    CAvatarDisjointRandomReward();                   // 0x08111014
    virtual ~CAvatarDisjointRandomReward();          // D1 0x08114afa / D0 0x08114b2a
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810f37a
    int  getRewardItem(CItem const* item,
                       std::vector<std::pair<int, int> >& out);       // 0x0810f694
    int  checkAvatarDisjointError(CUser* user, unsigned short slot,
                                  Inven_Item& item, CItem const* citem);  // 0x0810f3a4
    int  deleteAvatarItem(CUser* user, unsigned short slot,
                          Inven_Item& item, CItem const* citem);      // 0x0810f4e2
    void SendDisjointAvatarStatistic(CItem const* citem);             // 0x0810f5f4
    void SendCreateEmblemStatistic(
        std::vector<std::pair<int, int> >& rewards);                  // 0x0810f742
    int  sendRandomItemReward(CUser* user,
                              std::vector<std::pair<int, int> >& rewards);  // 0x0810f88c
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CObjectBringUp ← 养成本体事件。虚表覆写 set_event_data(slot4)/get_db_table_name(slot8)。
class CObjectBringUp : public CEventEntity
{
public:
    CObjectBringUp();                                // 0x08111068
    virtual ~CObjectBringUp();                       // D1 0x08114a10 / D0 0x08114a40
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810fcb2
    virtual std::string& get_db_table_name();               // slot 8 0x810fcdc：m_script+8
    void  sendEventItemFromFatigue(CUser*, unsigned int);  // 0x8110386
    bool  verifyGiveBringUpReward(ObjectBringUpScript::ObjectBringUpRule& rule,
                                  int count);                       // 0x0810fe1a
    bool  verifyMaterialItemByRule(CUser* user, unsigned int itemIdx);  // 0x0810fcea
    bool  sendBringUpReward(CUser* user, std::vector<int>& rewardVec,
                            unsigned char& flag);                   // 0x0810fe90
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CPurchaseCashItemBonus ← 商城消费奖励事件。虚表覆写 set_event_data(slot4)/
// get_db_table_name(slot8)。
class CPurchaseCashItemBonus : public CEventEntity
{
public:
    CPurchaseCashItemBonus();                        // 0x0811104c
    virtual ~CPurchaseCashItemBonus();               // D1 0x08114a5e / D0 0x08114a8e
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810f996
    virtual std::string& get_db_table_name();               // slot 8 0x810f9c0：m_script+8
    void  updateTargetGoodsCount(CUser*, unsigned int,
                                 stCeraShopIPGRequest_t*);  // 0x810f9ce
    bool  sendPurchaseBonusReward(CUser*, int);             // 0x810fb34
    bool  verifyGiveBonusReward(
              PurchaseCashItemBonusScript::PurchaseBonusRule&, int);  // 0x810fae0
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAccountFirstLoginDaily ← 由 CAccountFirstLogin（含虚槽 slot 9 do_action）派生。
// set_event_data 直接委托基类。不新增成员。
class CAccountFirstLoginDaily : public CAccountFirstLogin
{
public:
    CAccountFirstLoginDaily();                       // 0x0811110a
    virtual ~CAccountFirstLoginDaily();              // D1 0x08114c80 / D0 0x08114cb0
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810dfc6 委托
    virtual void do_action(int flags, CUser* user, std::bitset<32u> bs);
};

class CEventScriptMng : public InGameEventInfoList, public InGameEventScriptLoader
{
public:
    CEventScriptMng();                              // ORIG 0x810b136 T
    ~CEventScriptMng();
    bool is_eventing(int key);                      // ORIG 0x810ba36 T
    void send_event_init_record_data(CUser* user);      // ORIG 0x0810cb2c T
    void process_update_ingame_history(CUser* user);    // ORIG 0x0810c82e T
    void send_event_init_data(CUser* user, bool flag);   // ORIG 0x810c40e T
    void process_level_up_reward(CUser* user, bool flag);   // ORIG 0x0810bf56（CUser::_onLevelUp 尾调用；声明先行）
    void process_add_item_count_db(CUser* user, int itemId, int count);   // ORIG T 0x0810bea8（事件 0x193 拾取计数）
        std::string& get_db_table_name(int key);
    void process_account_first_login(int key, CUser* user, std::bitset<32u> flags);
    void process_add_fatigue_count(CUser* user, int count);
    void process_add_quest_count_db(CUser* user, int questId);
    void process_level_up_reward_recv_level(CUser* user, int level, int growType);
    void inc_user_fatigue(CUser* user, unsigned int fatigue);
    bool send_gift_fatigue_item(CUser* user, unsigned int fatigue);
    void update_purchase_target_item_count(CUser* user, unsigned int goodsId, stCeraShopIPGRequest_t* request);
    bool send_purchase_bonus_mail(CUser* user, unsigned int goodsId);
    bool send_bring_up_item_fatigue(CUser* user, int fatigue);
    void send_pc_room_play_init_time(CUser* user);
    // ---- 已恢复的访问器/工具方法（各报告） ----
    int  get_convert_to_user_confirmflag(int key);      // ORIG 0x810b0cc
    CEventEntity* get_event_entity(int key);            // ORIG 0x810b96a
    bool is_event_entity(int key) const;                // ORIG 0x810b9d4
    int  get_record_type(int key);                      // ORIG 0x810c7b8
    int  get_today_time();                              // ORIG 0x810c6d6：return m_todayTime
    unsigned char get_today_time_day();                 // ORIG 0x810c6ca：return m_todayTimeDay
    bool is_current_ingame_event_record_type(int key);  // ORIG 0x810c75c
    void get_ingame_event_record_type_list(std::vector<int>& out); // ORIG 0x810c6e2
    int  get_pass_day(int yyyymmdd1, int yyyymmdd2);    // ORIG 0x810c974
    void update_time_values(int hour);                  // ORIG 0x810c5d2
    void init_event_nums();                             // ORIG 0x810cdb0

    // ORIG _ZN13EventClassify15CEventScriptMng14initScriptlistER19InGameEventInfoList
    // （CDataManager::init 调用：以自身为实参加载事件脚本列表）
    bool initScriptlist(InGameEventInfoList& list);

    // ORIG _ZN13EventClassify15CEventScriptMng19create_event_entityEib 0x810b402 T
    // （按事件记录类型 new 对应实体；initScriptlist 被调。声明先行，定义归
    //   EventClassify 实体工厂批次）
    CEventEntity* create_event_entity(int recordType, bool flag);
    // ORIG _ZN13EventClassify15CEventScriptMng17init_evnet_entityEv 0x810bd14 T
    // （initScriptlist 尾部调用。声明先行，定义归同上批次）
    void init_evnet_entity();
    // ORIG _ZN13EventClassify15CEventScriptMng23test_process_change_dayEi 0x0810cc70 T
    void test_process_change_day(int timeVal);

private:
    std::map<int, CEventEntity*> m_entities;   // +0x30，sizeof 0x18 → 0x48
public:                                        // ORIG 直访成员
    int m_todayTime;                           // +0x48（yyyymmdd 当日时间值）
    unsigned char m_todayTimeDay;              // +0x4c（当日 dd）
private:
    std::vector<int> m_eventRecordTypes;       // +0x50，sizeof 0xc → 0x5c
};

}  // namespace EventClassify

#endif  // DNF_CEVENTSCRIPTMNG_H
