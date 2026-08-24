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

class CUser;
class PacketGuard;
class InGameEventScript;

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
    ~InGameEventInfoList();
    void insertEventInfo(int id, InGameEventInfo& info);   // ORIG 0x0897eb30 T

    std::map<int, InGameEventInfo> m_info;   // +0x00，sizeof 0x18
};

// InGameEventScriptLoader —— 基类 2。+0x00 map<string,InGameEventScript*>（std::map=0x18）。
class InGameEventScriptLoader
{
public:
    InGameEventScriptLoader();
    ~InGameEventScriptLoader();
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
    virtual char* get_db_table_name();                // 0x810b06c：return (char*)&m_dbTableName（+0x08）
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
// create_event_entity（0x810b402）按事件记录类型 new 对应类（0x20 字节先清零，
// 再调 ctor）。每个 ctor = 调基类 CEventEntity ctor + 设 vptr。
// 各派生类虚表只覆写部分槽位（见各类注释），get_db_table_name 一律覆写 slot 8。
// ============================================================================

// CLevelUpReward ← 事件记录类型 0x194。get_db_table_name 返回 m_script+0xc，
// get_mail_data 返回 m_script+0x28（Action_SendMail）。
class CLevelUpReward : public CEventEntity
{
public:
    CLevelUpReward();                                // 0x081110fc
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810e168
    virtual char* get_db_table_name();               // slot 8 0x810e2d6：m_script+0xc
    char  is_recv_check_item();                      // 0x810e2f2：*(m_script+8)
    char* get_mail_data();                           // 0x810e2e4：m_script+0x28
    int   set_reward_item(int, int, int);            // 0x810e302
    int   check_recv_level(int, int, int, bool);     // 0x810e192
    void  do_action(CUser* user);                    // 0x810e4ea（非虚）
    int   send_reward_from_recv_level(CUser*, int, int);  // 0x810e51a
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CUsedFatigueGiveItem ← 事件记录类型 0x65。虚表覆写 is_eventing(slot2)/
// set_event_data(slot4)/get_db_table_name(slot8)。get_db_table_name 返回 m_script+8。
class CUsedFatigueGiveItem : public CEventEntity
{
public:
    CUsedFatigueGiveItem();                          // 0x081110a0
    virtual bool is_eventing();                      // slot 2 0x811056e
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x81105be
    virtual char* get_db_table_name();               // slot 8 0x81105e8：m_script+8
    bool  getIsCharacEvent(bool& out) const;         // 0x81107ea
    void  sendEventItemFromFatigue(CUser*, unsigned int);  // 0x81105f6
    int   verifyApplyUser(CUser*);                   // 0x8110756
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAttendance ← 事件记录类型 0x56。虚表覆写 is_eventing(slot2)/set_event_data(slot4)/
// send_init_data(slot7)/get_db_table_name(slot8)。get_db_table_name 返回 m_script+8，
// get_mail_data 返回 m_script+0x2c。
class CAttendance : public CEventEntity
{
public:
    CAttendance();                                   // 0x08110fdc
    virtual bool is_eventing();                      // slot 2 0x810e75c
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810eadc
    virtual bool send_init_data(CUser* user, PacketGuard& packet,
                                short key, unsigned char count, bool flag);  // slot 7 0x810e804
    virtual char* get_db_table_name();               // slot 8 0x810e7cc：m_script+8
    char* get_mail_data();                           // 0x810e7da：m_script+0x2c
    int   get_fatigue_condition();                   // 0x810e7e8：*(m_script+0xc)
    int   get_final_day_condition();                 // 0x810e7f6：*(m_script+0x24)
    void  send_attend_reward_item(CUser*, int);      // 0x810e878
    void  send_attend_final_rward_item(CUser*, int, int);  // 0x810e9c0
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CPcRoomPlayTime ← 事件记录类型 0x5c。虚表覆写 is_eventing(slot2)/set_event_data(slot4)/
// get_db_table_name(slot8)。get_db_table_name 返回 m_script+8。
class CPcRoomPlayTime : public CEventEntity
{
public:
    CPcRoomPlayTime();                               // 0x08111084
    virtual bool is_eventing();                      // slot 2 0x8110814
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x8110872
    virtual char* get_db_table_name();               // slot 8 0x8110864：m_script+8
    int   processEventPcRoomPlayTime(CUser*);        // 0x811089c
    int   verifyUserPcRoomPlayEvent(CUser*);         // 0x8110a38
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAccountFirstLogin ← 事件记录类型 0x191/0x192。虚表覆写 set_event_data(slot4)/
// get_db_table_name(slot8)，并新增虚槽 slot 9 = do_action。get_db_table_name 返回
// m_script+8，get_mail_data 返回 m_script+0xc。CAccountFirstLoginDaily 由此类派生。
class CAccountFirstLogin : public CEventEntity
{
public:
    CAccountFirstLogin();                            // 0x08110f88
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810deda
    virtual char* get_db_table_name();               // slot 8 0x810debe：m_script+8
    virtual void do_action(int flags, CUser* user, std::bitset<32u> bs);  // slot 9 0x810df04
    char* get_mail_data();                           // 0x810decc：m_script+0xc
protected:
    InGameEventScript* m_script1c;                   // +0x1c
};

// CAddItemCountDB ← 事件记录类型 0x193。虚表覆写 set_event_data(slot4)/
// get_db_table_name(slot8)。get_db_table_name 返回 m_script+0x20。
class CAddItemCountDB : public CEventEntity
{
public:
    CAddItemCountDB();                               // 0x08110fa4
    virtual void set_event_data(int key, InGameEventScript* script);  // slot 4 0x810e064
    virtual char* get_db_table_name();               // slot 8 0x810e15a：m_script+0x20
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
    virtual char* get_db_table_name();               // slot 8 0x810f988：m_script+8
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
    virtual char* get_db_table_name();               // slot 8 0x810fcdc：m_script+8
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
    virtual char* get_db_table_name();               // slot 8 0x810f9c0：m_script+8
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
};

class CEventScriptMng : public InGameEventInfoList, public InGameEventScriptLoader
{
public:
    CEventScriptMng();                              // ORIG 0x810b136 T
    ~CEventScriptMng();
    bool is_eventing(int key);                      // ORIG 0x810ba36 T
    void send_event_init_data(CUser* user, bool flag);   // ORIG 0x810c40e T
    void process_level_up_reward(CUser* user, bool flag);   // ORIG 0x0810bf56（CUser::_onLevelUp 尾调用；声明先行）
    void process_add_item_count_db(CUser* user, int itemId, int count);   // ORIG T 0x0810bea8（事件 0x193 拾取计数）
    // ---- 已恢复的访问器/工具方法（各报告） ----
    int  get_convert_to_user_confirmflag(int key);      // ORIG 0x810b0cc
    CEventEntity* get_event_entity(int key);            // ORIG 0x810b96a
    bool is_event_entity(int key) const;                // ORIG 0x810b9d4
    int  get_record_type(int key);                      // ORIG 0x810c7b8
    int  get_today_time();                              // ORIG 0x810c6d6：return m_field48
    unsigned char get_today_time_day();                 // ORIG 0x810c6ca：return m_flag4c
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

private:
    std::map<int, CEventEntity*> m_entities;   // +0x30，sizeof 0x18 → 0x48
public:                                        // ORIG 直访成员
    int m_field48;                             // +0x48
    unsigned char m_flag4c;                    // +0x4c
private:
    std::vector<int> m_vec50;                  // +0x50，sizeof 0xc → 0x5c
};

}  // namespace EventClassify

#endif  // DNF_CEVENTSCRIPTMNG_H
