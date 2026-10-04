// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_U3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// U3 杂项 DB 同步请求 TU（U3ShopDb 批次，2026-08-23）。
// 两个 DB packet makeRequest，逐字段对照 docs/class_func_reports 反编译 C：
//   DB_AccountHack_CheckMyAccount::makeRequest  0x0843a13e
//   DB_InsertCeraItemHistory::makeRequest       0x0844511a
// 包构建约定：StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard →
// 写 opcode/字段 → MsgQueueMgr::put(DB_Q, guard)。
// b28d-61 [P159]：ORIG 两函数尾部均无 `mov $0x0,%eax`（直接 leave;ret）⇒ 实际
// 返回 void。原「沿用调用方 int 声明」使 4.4 多出 `mov $0x0,%ebx`+`mov %ebx,%eax`
// （返回值跨 guard 析构保活）与 EH 状态初始化；本 TU 声明/定义改 void，调用方
// （CMailBoxHelper.cpp/CTradeSpace.cpp 局部前置声明 int）均为语句调用不取返回值，
// mangled 不含返回类型，链接无影响。

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "StreamDeps.h"       // G_CEnvironment（UnlimitSupportLog serverGroup 字段）
#include "DBMgr.h"            // DBMgr::GetDBHandle（dispatch 取 DB 句柄；连带 MySQL.h/CEnvironment.h）
#include <cstring>

// NumberToString 权威声明在 CInventory.h（0x0810904b u32 版），但该头在本 TU
// include 链会经 CDataManager.h 触发重头文件解析；此处仅前向声明同名全局函数，
// 与权威签名一致（同 W8LT_DBTask.cpp 既有做法）。
char* NumberToString(unsigned int value, int radix);

#include "CacheCharacterType.h"  // ENUM_INTERNALPACKET（_makeRequest 首参）

class DB_AccountHack_CheckMyAccount
{
public:
    static void makeRequest(unsigned int uid, unsigned int accId,
                            unsigned int a, unsigned int money,
                            char* senderName, int nameLen);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x8439f06
};

class DB_InsertCeraItemHistory
{
public:
    static void makeRequest(int itemIdx, unsigned int senderCharacNo,
                            unsigned int receiverCharacNo, char flag);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x8445032
};

#include "DB_U3/makeRequest.inc"


#include "DB_U3/makeRequest_1.inc"



// ============================================================================
// DBHistoryDeps 批次（2026-08-24）：DB_InsertArchieveEventLog /
// DB_InsertUnlimitSupportLog / DB_ReqSaveHackUserPunish::makeRequest。
// 类声明与调用方 TU 局部前置声明逐字一致（CUserStatA.cpp / DisPatcherCore.cpp），
// 返回 void 按 ORIG 反编译；符号即 ORIG T：
//   _ZN25DB_InsertArchieveEventLog11makeRequestEjjN24SIG_INSERT_ACHIEVE_EVENT17ENUM_ACHIEVE_TYPEE
//   _ZN26DB_InsertUnlimitSupportLog11makeRequestEjjPKc
//   _ZN24DB_ReqSaveHackUserPunish11makeRequestEjjPKc
// ============================================================================

class DB_InsertArchieveEventLog
{
public:
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x842e7be
};

class DB_InsertUnlimitSupportLog
{
public:
    static void makeRequest(unsigned int uid, unsigned int characNo,
                            const char* name);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x842eb2e
};

class DB_ReqSaveHackUserPunish
{
public:
    static void makeRequest(unsigned int uid, unsigned int type, const char* ip);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x8425374
};

#include "DB_U3/makeRequest_2.inc"
    unsigned int accId, unsigned int characNo,
    SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7349), true);
    guard.operator*()->operator<<(0x117);          // SIG_INSERT_ACHIEVE_EVENT
    guard.operator*()->operator<<(-1);

    SIG_INSERT_ACHIEVE_EVENT* v =
        guard.operator->()->GetInBuffer<SIG_INSERT_ACHIEVE_EVENT>();
    v->m_accId = accId;
    v->m_characNo = characNo;
    v->m_type = type;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

#include "DB_U3/makeRequest_3.inc"
                                             const char* name)
{
    if (uid <= 0x946501)
        return;

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7392), true);
    guard.operator*()->operator<<(0x122);          // SIG_INSERT_UNLIMIT_SUPPORT_LOG
    guard.operator*()->operator<<(-1);

    SIG_INSERT_UNLIMIT_SUPPORT_LOG* v =
        guard.operator->()->GetInBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>();
    v->m_accId = uid;
    v->m_characNo = (int)characNo;
    v->m_serverId =
        G_CEnvironment()->m_serverEnvir.m_serverGroup;
    std::strncpy(v->m_name, name, 0x1d);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "DB_U3/makeRequest_4.inc"
                                           const char* ip)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6304), true);
    guard.operator*()->operator<<(0xb5);           // SIG_SAVE_HACK_USER_PUNISH
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        // +0x04。int 字段←unsigned 形参异型 = 4.4 RHS-first 3 条形
        // （ORIG 0x08425494 `mov 0xc(%ebp),%edx; mov -0xc(%ebp),%eax;
        // mov %edx,0x4(%eax)`；同型 jA 形反产 LHS-first，探针 k4.cpp jD；
        // 与 SigTypes_E.h::m_punish 同批改同源证据）。
        int m_type;                                // +0x04
        char m_ip[0x10];                           // +0x08
    };
    SIG_SAVE_HACK_USER_PUNISH* v =
        guard.operator->()->GetInBuffer<SIG_SAVE_HACK_USER_PUNISH>();
    v->m_accId = uid;
    v->m_punish = type;
    if (ip == 0)
        std::memcpy(v->m_ip, "000.000.000.000", 0x10);
    else
        std::strncpy(v->m_ip, ip, 0x10);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// LinkF1 批次（2026-08-28）：_onQuestClear 依赖补全；同步修正 CUserOnlyOrig.cpp
// 局部发明签名。符号即 ORIG T：
//   _ZN22DB_UpdateHappyBeanInfo11makeRequestEjjjPKc      @0x08425a02
//   _ZN30DB_Insert0712WinterEventCoupon11makeRequestEjjj @0x0842e648
//   _ZN30DB_Insert0712WinterEventCoupon12_makeRequestE21ENUM_INTERNALPACKETjjj
//                                                        @0x0842e6e0
// 报告：docs/class_func_reports/DB_UpdateHappyBeanInfo/makeRequest.md、
//       docs/class_func_reports/DB_Insert0712WinterEventCoupon/{makeRequest,_makeRequest}.md
// ============================================================================

class DB_Insert0712WinterEventCoupon
{
public:
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            unsigned int count);
    static Stream* _makeRequest(ENUM_INTERNALPACKET op, unsigned int accId,
                                unsigned int characNo, unsigned int count);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x842e45e
};

class DB_UpdateHappyBeanInfo
{
public:
    static void makeRequest(unsigned int uid, unsigned int accId,
                            unsigned int characNo, const char* name);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x84258ee
};

#include "DB_U3/makeRequest_5.inc"


#include "DB_U3/_makeRequest.inc"


#include "DB_U3/makeRequest_6.inc"


// ============================================================================
// Dispatch 批次（2026-08-29）：三类 DB_Q 消息的 DB 侧处理（ORIG T 强符号）：
//   _ZN24DB_InsertCeraItemHistory8dispatchEiiP6Stream  @0x08445032
//   _ZN24DB_ReqSaveHackUserPunish8dispatchEiiP6Stream  @0x08425374
//   _ZN22DB_UpdateHappyBeanInfo8dispatchEiiP6Stream    @0x084258ee
// 对照 ORIG 反汇编逐指令还原；类在 ORIG 为 DBDispatcher 派生（C1 设 vptr，
// 见 W8LT_DBTask.cpp 注册 _ZN22DB_UpdateHappyBeanInfoC1Ev @0x844e5b2），本 TU
// 维持既有无继承静态类声明（与调用方局部声明一致），dispatch 实现为非虚成员，
// mangled 符号与 ORIG 一致（虚/非虚不影响符号名；函数体 O0 下 this 均在
// 0x8(%ebp)，指令形态相同）。注册表虚调用侧由 W8LT_DBTask.cpp 独占后续处理。
// ============================================================================

#include "DB_U3/dispatch.inc"


#include "DB_U3/dispatch_1.inc"


#include "DB_U3/dispatch_2.inc"


// ============================================================================
// Dispatch 补全批次（2026-08-29 续）：其余 4 类 DB_Q 消息的 DB 侧处理：
//   _ZN29DB_AccountHack_CheckMyAccount8dispatchEiiP6Stream  @0x08439f06
//   _ZN30DB_Insert0712WinterEventCoupon8dispatchEiiP6Stream @0x0842e45e
//   _ZN25DB_InsertArchieveEventLog8dispatchEiiP6Stream      @0x0842e7be
//   _ZN26DB_InsertUnlimitSupportLog8dispatchEiiP6Stream     @0x0842eb2e
// 类声明见上（与 W8LT_DBTask.cpp 注册一致），dispatch 非虚成员，mangled 同 ORIG。
// ============================================================================

// ---- DB_AccountHack_CheckMyAccount::dispatch ORIG @ 0x8439f06 ----
// GetOutBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>；DB_HANDLE_2。buf[0x3b] 初始化式清零
// （P4：ORIG and/shr/rep stos 内联序列）。escape_string(buf, name) 后查
// "seLect m_id from charac_info where charac_name='%s' limit 1"；exec/fetch/get_int
// 任一失败返 false。查得 m_id != 请求 accId(+0x04) 时回包 SIG_CHECK_MY_ACCOUNT_OK
// （opcode 0x18c | slot）：+0x00=请求 a、+0x04=请求 money(+0x25)、+0x08=库中 m_id，
// 载荷视图（P80 pack(1)：+0x25 为奇数偏移 int）。
#pragma pack(push, 1)
struct DBU3AccountHackIn
{
    unsigned int m_a;        // +0x00
    unsigned int m_accId;    // +0x04
    char m_name[0x1d];       // +0x08
    unsigned int m_money;    // +0x25
};
struct DBU3AccountHackOut
{
    unsigned int m_a;        // +0x00
    unsigned int m_money;    // +0x04
    unsigned int m_id;       // +0x08
};
#pragma pack(pop)
#include "DB_U3/dispatch_3.inc"

// ---- DB_Insert0712WinterEventCoupon::dispatch ORIG @ 0x842e45e ----
// GetOutBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>（+0x00=m_id、+0x08=count）。
// userIdx 即 op（ORIG cmpl $0x112,0xc(%ebp)）：
//   0x112 → DB_HANDLE_1，upDate event_goldcard_info coupon=coupon+%d where m_id=%s
//   （%d=count、%s=NumberToString(m_id)）；exec 失败或影响行数 0 → inSert
//   event_goldcard_info(m_id,coupon) values(%s,%d)（%s=NumberToString、%d=count）。
//   其它 → DB_HANDLE_4，event_goldcard_log 同构（occ_date=now() 条件与列）。
// 恒返 1（true）。
struct DBU3WinterCouponIn
{
    unsigned int m_accId;       // +0x00
    unsigned int m_characNo;    // +0x04（dispatch 未用）
    int m_count;                // +0x08
};
#include "DB_U3/dispatch_4.inc"


// ---- DB_InsertArchieveEventLog::dispatch ORIG @ 0x842e7be ----
// GetOutBuffer<SIG_INSERT_ACHIEVE_EVENT>（+0x00=accId、+0x04=characNo、
// +0x08=achieveType）；DB_HANDLE_4。单条 inSert log_charac_lev_log：
// values(now(),%s,%d,%d) = NumberToString(accId), characNo, achieveType。
// exec 后恒返 1（true）。
struct DBU3AchieveIn
{
    unsigned int m_accId;                                     // +0x00
    int m_characNo;                                           // +0x04
    SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE m_type;       // +0x08
};
#include "DB_U3/dispatch_5.inc"


// ---- DB_InsertUnlimitSupportLog::dispatch ORIG @ 0x842eb2e ----
// GetOutBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>（+0x00=m_id、+0x04=serverId、
// +0x08=characNo、+0x0c=name[0x1d]）；DB_HANDLE_6。单条
// inSert event_unlimitsupport_lev(m_id,occ_time,server_id,charac_no,charac_name)
// values(%s,now(),%d,%d,'%s') = NumberToString(m_id), serverId, characNo, name。
// exec 后恒返 1（true）。
struct DBU3UnlimitIn
{
    unsigned int m_accId;       // +0x00
    int m_serverId;             // +0x04
    int m_characNo;             // +0x08
    char m_name[0x1d];          // +0x0c
};
#include "DB_U3/dispatch_6.inc"

