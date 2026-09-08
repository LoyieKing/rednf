// ============================================================================
// df_game_r 还原 —— W8LT_DBTask（BatchW2_03）
// ORIG 符号（nm 核对）：
//   T _ZN6DBTaskC2Ev / C1Ev           0x083fbfc6  sz 0x13
//   T _ZN6DBTaskD2Ev / D1Ev           0x083fbfda  sz 0x13
//   T _ZN6DBTask16RegistDispatcherEv  0x083fbfee  sz 0x3168
//   T _ZN6DBTask18UnregistDispatcherEv 0x083ff156 sz 0x72
//   T _ZN6DBTask12HandleEventsEN11MsgQueueMgr9QUEUE_IDXERi 0x083ff1c8 sz 0x22b
//   T _ZN6DBTask9DBProcessEP6StreamRi  0x083ff3f4 sz 0x1fd
//   W _ZN16stPvPTempTable_tD1Ev/D2Ev   0x08458e92  sz 0x5
// 报告：docs/class_func_reports/DBTask/{DBTask.md,~DBTask.md}（ctor/dtor 忠实
// 转发），其余四函数按 objdump 反汇编逐指令转写；字符串常量取自 .rodata。
//
// 对象布局（DBTask）：+0x00 int 当前队列 idx；+0x04 起 DBDispatcher*[852]
// （RegistDispatcher 循环边界 cmpl $0x353 == 851、最大注册槽 slot[848]+?——
// 见 RegistDispatcher 注释；sizeof = 4 + 852*4 = 0xd54）。
//
// 派生 dispatcher 类建模口径：ORIG 各 C1 反汇编全部为「调用 DBDispatcher 基类
// ctor + 编译器 vptr 赋值」共 ~10 指令的标准空构造（逐个 objdump 核对，证据见
// /tmp/df_batches/w8lt/regist_dispatcher.asm）；对象尺寸均 0x4（operator new 的
// 尺寸实参）。因此此处空派生类与 ORIG 完全一致。批界与阻塞条目在
// RegistDispatcher 内以注释保留 ORIG 地址。
#ifndef DNF_GAME_W8LT_DBTASK_H_
#define DNF_GAME_W8LT_DBTASK_H_

#include "MsgQueueMgr.h"
#include "DBDispatcher.h"
#include "GameEnums.h"   // ENUM_SKILL_TREE_KIND（DB_UpdateComboSkill::setComboSkill 按值形参）
#include <map>           // DB_UpdateFatigueAttendance::makeRequest 形参
#include <utility>       // std::pair

class Stream;
class CAccountCargo;
class DBDispatcher;
class MySQL;                 // DB_UpdateComboSkill::setComboSkill 形参（指针）
struct comoboSkillData;      // DB_UpdateComboSkill::setComboSkill 形参（指针）
struct SIG_SAVE_EVENTITEM;

class DBTask
{
public:
    static const int DISPATCHER_COUNT = 852;   // ORIG Unregist/Regist 循环上界 0..851

    DBTask();                                    // 0x083fbfc6
    ~DBTask();                                   // 0x083fbfda

    // SIGFIX:b28c-28 ORIG rodata _ZZN6DBTask16RegistDispatcherEvE19__PRETTY_FUNCTION__
    //   @0x08c5dae0 = "bool DBTask::RegistDispatcher()"（P174）；体尾 mov $1,%ebx
    //   正常返 true、catch 路径返 false；唯一调用点本 TU 3031 行语句调用。
    bool RegistDispatcher();                     // 0x083fbfee
    bool UnregistDispatcher();                   // 0x083ff156（恒返 true）
    bool HandleEvents(MsgQueueMgr::QUEUE_IDX queueIdx,
                      int& signal);              // 0x083ff1c8
    bool DBProcess(Stream* stream, int& signal); // 0x083ff3f4

private:
    int m_curQueueIdx;            // +0x00（HandleEvents 写入当前处理的队列枚举值）
    DBDispatcher* m_dispatchers[852];  // +0x04（消息 id -> dispatcher 表）
};


// DB_SaveFeaturedGoodsIdx —— 权威声明点（LinkClosureB-2）：
// ORIG T _ZN23DB_SaveFeaturedGoodsIdx11makeRequestEjiiii @0x0841b2da
// （static 成员，报告 docs/class_func_reports/DB_SaveFeaturedGoodsIdx/makeRequest.md）
// CCeraShop_Special.cpp 提供定义；DB_SaveUserHandicap::makeRequest 尾部调用。
class DB_SaveFeaturedGoodsIdx : public DBDispatcher
{
public:
    DB_SaveFeaturedGoodsIdx() {}   // ORIG C1 0x844e0e2 (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int accId, int characNo, int restrictCode,
                            int curSec, int d);
    // ORIG T 0x0841b210 _ZN23DB_SaveFeaturedGoodsIdx8dispatchEiiP6Stream（虚覆盖；W8LT_DBTask.cpp 定义）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};


// DB_SwapCreatureItem —— 权威声明点（StubSwapCreature）：
// ORIG T _ZN19DB_SwapCreatureItem11makeRequestEii @0x0841fef2
// （static 成员，报告 docs/class_func_reports/DB_SwapCreatureItem/makeRequest.md）
// W8LT_DBTask.cpp 提供定义；DB_LoadInventory.cpp GetUserCreature 调用点。
class DB_SwapCreatureItem : public DBDispatcher
{
public:
    DB_SwapCreatureItem() {}   // ORIG C1 0x844e39e (sz 0x4: base ctor+vptr only)
    static void makeRequest(int itemIdx1, int slot2);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841fe0c
};


// DB_UpdateGMData —— 权威声明点（OrigEventLeaf8d）：
// ORIG T _ZN15DB_UpdateGMData11makeRequestEb @0x0843982c
// （static 成员，报告缺失按 §3.2 兜底 objdump 转写）：
// Acquire("DBThread.cpp",0x8761) → CStreamGuard(stream,true)
//   → <<0x14a(int) → <<-1(int) → <<flag(bool) → put(DB_Q(2))。
// W8LT_DBTask.cpp 提供定义；CGmRegistEvent.cpp StartEvent 调用点。
class DB_UpdateGMData : public DBDispatcher
{
public:
    DB_UpdateGMData() {}   // ORIG C1 0x844ede6 (sz 0x4: base ctor+vptr only)
    static void makeRequest(bool flag);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843990a
};

// DB_SaveEventItem —— 权威声明点（OrigDBEventItem）：
// ORIG T（报告 docs/class_func_reports/DB_SaveEventItem/*）：
//   dispatch              0x0841a7e4
//   _CheckAlreadyGetYegamItem 0x0841a51a
//   _HardCodeEventItems   0x0841a662
//   _UpdateEventYegamInfo 0x0841a5ea
// W8LT_DBTask.cpp 提供定义；DBTask::RegistDispatcher 注册 slot 76。
class DB_SaveEventItem : public DBDispatcher
{
public:
    DB_SaveEventItem() {}   // ORIG C1 0x844dad8 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x0841a7e4
    bool _CheckAlreadyGetYegamItem(int id, int type);               // ORIG 0x0841a51a
    bool _HardCodeEventItems(SIG_SAVE_EVENTITEM* sig);              // ORIG 0x0841a662
    bool _UpdateEventYegamInfo(int id, int type);                   // ORIG 0x0841a5ea
};

// DB_UpdateCharacView —— 权威声明点（GoalAuditDB7）：
// ORIG T _ZN19DB_UpdateCharacView8dispatchEiiP6Stream @0x08408848
// （报告 docs/class_func_reports/DB_UpdateCharacView/dispatch.md）。
// W8LT_DBTask.cpp 提供定义；DBTask::RegistDispatcher 注册 slot 6。
class DB_UpdateCharacView : public DBDispatcher
{
public:
    DB_UpdateCharacView() {}   // ORIG C1 0x844d85c (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x08408848
};


// DB_UpdateBingoEvent —— 权威声明点（GoalAuditDB7）：
// ORIG T _ZN19DB_UpdateBingoEvent11makeRequestEijib @0x08449032、
// _ZN19DB_UpdateBingoEvent8dispatchEiiP6Stream @0x08448eb8
// （报告 docs/class_func_reports/DB_UpdateBingoEvent/*）。
// W8LT_DBTask.cpp 提供定义；CUser::updateBingoData 调用 makeRequest；
// DBTask::RegistDispatcher 注册 slot 799。
class DB_UpdateBingoEvent : public DBDispatcher
{
public:
    DB_UpdateBingoEvent() {}   // ORIG C1 0x844fd6e (sz 0x4: base ctor+vptr only)
    static void makeRequest(int uid, unsigned int accId, int eventId, bool won);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x08448eb8
};


// DB_UpdateComboSkill —— 权威声明点（GoalAuditDB7）：
// ORIG T _ZN19DB_UpdateComboSkill8dispatchEiiP6Stream @0x0844a8ea、
// _ZN19DB_UpdateComboSkill13setComboSkillEP5MySQLjhP15comoboSkillData20ENUM_SKILL_TREE_KIND
// @0x0844aa4c（报告 docs/class_func_reports/DB_UpdateComboSkill/*）。
// W8LT_DBTask.cpp 提供定义；DBTask::RegistDispatcher 注册 slot 797。
class DB_UpdateComboSkill : public DBDispatcher
{
public:
    DB_UpdateComboSkill() {}   // ORIG C1 0x844fe86 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x0844a8ea
    bool setComboSkill(MySQL* db, unsigned int characNo, unsigned char count,
                       comoboSkillData* data,
                       ENUM_SKILL_TREE_KIND kind);                  // ORIG 0x0844aa4c
};
// DB_UpdateSchoolPoint —— 权威声明点（FixCall_D RecoverFatigue 消费）：
// DB_UpdateSchoolPoint —— 权威声明点（FixCall_D RecoverFatigue 消费）：
// ORIG T _ZN20DB_UpdateSchoolPoint11makeRequestEjii @0x0842f0f0、
// _ZN20DB_UpdateSchoolPoint8dispatchEiiP6Stream @0x0842f05e。
// W8LT_DBTask.cpp 提供定义；DBTask::RegistDispatcher 注册。
class DB_UpdateSchoolPoint : public DBDispatcher
{
public:
    DB_UpdateSchoolPoint() {}   // ORIG C1 0x844eb7e (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int mId, int point, int flag); // ORIG 0x0842f0f0
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x0842f05e
};

// UpdateData 使用的 DB 请求类：权威声明与实现归口于此头/ W8LT_DBTask.cpp。
class DB_SaveAccountCargo : public DBDispatcher
{
public:
    DB_SaveAccountCargo() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    static void makeRequest(int userIdx, unsigned int accId,
                            CAccountCargo* cargo);
};

class DB_UpdateItemCountValue : public DBDispatcher
{
public:
    DB_UpdateItemCountValue() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    static void makeRequest(unsigned int param1, unsigned int param2);
};

class DB_UpdateAddItemCountValue : public DBDispatcher
{
public:
    DB_UpdateAddItemCountValue() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    static void makeRequest(unsigned int param1, unsigned int param2);
};

class DB_UpdateUserFatigueQuantity : public DBDispatcher
{
public:
    DB_UpdateUserFatigueQuantity() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    static void makeRequest(unsigned int accId, unsigned int qty);
};

class DB_UpdateUserGiftFatigueQuantity : public DBDispatcher
{
public:
    DB_UpdateUserGiftFatigueQuantity() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    static void makeRequest(unsigned int accId, unsigned int qty,
                            unsigned int characNo);
};

// DB_UpdateRevengeData —— 权威声明点（FixCond_D ReturnToSelectCharacList 消费）：
// ORIG T _ZN20DB_UpdateRevengeData11makeRequestEjhh @0x0843f232、
// _ZN20DB_UpdateRevengeData8dispatchEiiP6Stream @0x0843f34c。
// W8LT_DBTask.cpp 提供定义；DBTask::RegistDispatcher 注册。
class DB_UpdateRevengeData : public DBDispatcher
{
public:
    DB_UpdateRevengeData() {}   // ORIG C1 0x844f68a (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int characNo, unsigned char attack,
                            unsigned char revenge); // ORIG 0x0843f232
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x0843f34c
};

// DB_UpdateOntimeLastRecvIdx —— 权威声明点（Fix7_F CUser::UpdateLogout 消费）：
// ORIG T _ZN25DB_UpdateOntimeLastRecvIdx11makeRequestEiji @0x08442966、
// _ZN25DB_UpdateOntimeLastRecvIdx8dispatchEiiP6Stream @0x0844280a。
// W8LT_DBTask.cpp 提供定义；DBTask::RegistDispatcher 注册。
class DB_UpdateOntimeLastRecvIdx : public DBDispatcher
{
public:
    DB_UpdateOntimeLastRecvIdx() {}   // ORIG C1 0x844f946 (sz 0x4: base ctor+vptr only)
    static void makeRequest(int uid, unsigned int accId, int idx); // ORIG 0x08442966
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x0844280a
};

// DB_UpdateFatigueAttendance —— 权威声明点（Fix7_F CUser::UpdateLogout 消费）：
// ORIG T _ZN26DB_UpdateFatigueAttendance11makeRequestEjRKSt3mapIhsSt4lessIhESaISt4pairIKhsEEE
// @0x084447d6、_ZN26DB_UpdateFatigueAttendance8dispatchEiiP6Stream @0x084444ec。
// W8LT_DBTask.cpp 提供定义；DBTask::RegistDispatcher 注册。
class DB_UpdateFatigueAttendance : public DBDispatcher
{
public:
    DB_UpdateFatigueAttendance() {}   // ORIG C1 0x844f9d2 (sz 0x4: base ctor+vptr only)
    static void makeRequest(
        unsigned int accId,
        std::map<unsigned char, short, std::less<unsigned char>,
                 std::allocator<std::pair<const unsigned char, short> > > const& m);
    // ORIG 0x084444ec（虚覆盖）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // DNF_GAME_W8LT_DBTASK_H_
