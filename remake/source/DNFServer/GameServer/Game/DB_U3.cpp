// U3 杂项 DB 同步请求 TU（U3ShopDb 批次，2026-08-23）。
// 两个 DB packet makeRequest，逐字段对照 docs/class_func_reports 反编译 C：
//   DB_AccountHack_CheckMyAccount::makeRequest  0x0843a13e
//   DB_InsertCeraItemHistory::makeRequest       0x0844511a
// 包构建约定：StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard →
// 写 opcode/字段 → MsgQueueMgr::put(DB_Q, guard)。
// 类声明与 CMailBoxHelper.cpp 中的前置声明逐字一致（ORIG 返回 void，此处
// 沿用调用方声明的 int 以保持类定义一致；mangled 符号不含返回类型）。

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
    static int makeRequest(unsigned int uid, unsigned int accId,
                           unsigned int a, unsigned int money,
                           char* senderName, int nameLen);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x8439f06
};

class DB_InsertCeraItemHistory
{
public:
    static int makeRequest(int itemIdx, unsigned int senderCharacNo,
                           unsigned int receiverCharacNo, char flag);
    bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x8445032
};

// ---- DB_AccountHack_CheckMyAccount::makeRequest ORIG @ 0x0843a13e ----
// stream: opcode 0x18b | param_1(uint)
// in-buffer(SIG_CHECK_MY_ACCOUNT_IN_DB): +0x00=param_3, +0x04=param_2,
//   +0x08 memset 0x1d 后 strncpy(param_5, param_6), +0x25=param_4
int DB_AccountHack_CheckMyAccount::makeRequest(unsigned int uid,
                                               unsigned int accId,
                                               unsigned int a,
                                               unsigned int money,
                                               char* senderName, int nameLen)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8846), true);
    guard.operator*()->operator<<(0x18b);  // SIG_CHECK_MY_ACCOUNT
    guard.operator*()->operator<<((int)uid);

    SIG_CHECK_MY_ACCOUNT_IN_DB* sig =
        guard->GetInBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>();
    sig->m_param0 = a;
    sig->m_accId = accId;
    sig->m_money = money;
    std::memset(sig->m_name, 0, 0x1d);
    std::strncpy(sig->m_name, senderName, nameLen);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

// ---- DB_InsertCeraItemHistory::makeRequest ORIG @ 0x0844511a ----
// stream: opcode 0x2b1 | -1 | itemIdx(int) | sender(uint) | receiver(uint)
//         | flag(char 符号扩展)
int DB_InsertCeraItemHistory::makeRequest(int itemIdx,
                                          unsigned int senderCharacNo,
                                          unsigned int receiverCharacNo,
                                          char flag)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xac1b);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x2b1);          // SIG_INSERT_CERA_ITEM_HISTORY
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(itemIdx);
    guard.operator*()->operator<<(senderCharacNo);   // ORIG lsEj（unsigned）
    guard.operator*()->operator<<(receiverCharacNo); // ORIG lsEj（unsigned）
    guard.operator*()->operator<<(flag);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}


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

// ---- DB_InsertArchieveEventLog::makeRequest ORIG @ 0x0842e856 ----
// stream: opcode 0x117 | -1 | GetInBuffer<SIG_INSERT_ACHIEVE_EVENT>：
//   +0x00=accId、+0x04=characNo、+0x08=type。投 LOGDB_Q（ORIG put 第 2 实参 4）。
// SIG 结构在 SigTypes.h 为 0x100 占位缓冲，按报告偏移以函数内命名视图字段化
// （成员访问折叠出与 ORIG 相同的 mov %edx,0xN(%eax) 存储形态）。
void DB_InsertArchieveEventLog::makeRequest(
    unsigned int accId, unsigned int characNo,
    SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7349), true);
    guard.operator*()->operator<<(0x117);          // SIG_INSERT_ACHIEVE_EVENT
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_characNo;                   // +0x04
        SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE m_type;  // +0x08（原枚举存储）
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_INSERT_ACHIEVE_EVENT>());
    v->m_accId = accId;
    v->m_characNo = characNo;
    v->m_type = type;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ---- DB_InsertUnlimitSupportLog::makeRequest ORIG @ 0x0842ebd0 ----
// ORIG 首指令 cmpl $0x946501,param_1; jbe → uid <= 0x946501 直接返回。
// GetInBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>：+0x00=uid、+0x04=serverGroup
// （G_CEnvironment()+0x378 == m_serverEnvir.m_serverGroup，具名链见 CEnvironment.h）、
// +0x08=characNo、+0x0c strncpy(name,0x1d)（ORIG 无 memset）。投 DB_Q（实参 2）。
void DB_InsertUnlimitSupportLog::makeRequest(unsigned int uid, unsigned int characNo,
                                             const char* name)
{
    if (uid <= 0x946501)
        return;

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7392), true);
    guard.operator*()->operator<<(0x122);          // SIG_INSERT_UNLIMIT_SUPPORT_LOG
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_serverGroup;                // +0x04
        unsigned int m_characNo;                   // +0x08
        char m_name[0x1d];                         // +0x0c
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>());
    v->m_accId = uid;
    v->m_characNo = characNo;
    // 报告语句序：先写 +8，再调 G_CEnvironment 回填 +4（serverGroup）。
    v->m_serverGroup =
        static_cast<unsigned int>(G_CEnvironment()->m_serverEnvir.m_serverGroup);
    std::strncpy(v->m_name, name, 0x1d);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- DB_ReqSaveHackUserPunish::makeRequest ORIG @ 0x08425494 ----
// GetInBuffer<SIG_SAVE_HACK_USER_PUNISH>：+0x00=uid、+0x04=type、+0x08 ip：
//   NULL → memcpy "000.000.000.000"（0x10 含 NUL）；否则 strncpy(ip,0x10)。
//   两分支 ORIG 形态不同（add 直算 vs lea 经 edx），按原表达式转写。投 DB_Q。
void DB_ReqSaveHackUserPunish::makeRequest(unsigned int uid, unsigned int type,
                                           const char* ip)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6304), true);
    guard.operator*()->operator<<(0xb5);           // SIG_SAVE_HACK_USER_PUNISH
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_type;                       // +0x04
        char m_ip[0x10];                           // +0x08
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_SAVE_HACK_USER_PUNISH>());
    v->m_accId = uid;
    v->m_type = type;
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

// ---- DB_UpdateHappyBeanInfo::makeRequest ORIG @ 0x08425a02 ----
// stream: opcode 0xbb | -1 | 逐字段 uid/accId/characNo/name（operator<< 直写）。
// 投 DB_Q（ORIG put 第 2 实参 2）。
void DB_UpdateHappyBeanInfo::makeRequest(unsigned int uid, unsigned int accId,
                                         unsigned int characNo, const char* name)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63a3), true);
    guard.operator*()->operator<<(0xbb);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<((char*)name);   // ORIG ls(Pc)：const char* 直接绑 bool 重载

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- DB_Insert0712WinterEventCoupon::_makeRequest ORIG @ 0x0842e6e0 ----
// stream: opcode=op | -1 | GetInBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>：
//   +0x00=accId、+0x04=characNo、+0x08=count。Get() 返回 Stream*，0=不可投递。
// ORIG Acquire 后 guard 初始 false（release 语义差异，按原样保留）。
Stream* DB_Insert0712WinterEventCoupon::_makeRequest(ENUM_INTERNALPACKET op,
                                                     unsigned int accId,
                                                     unsigned int characNo,
                                                     unsigned int count)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7331), false);
    guard.operator*()->operator<<((int)op);
    guard.operator*()->operator<<(-1);

    SIG_INSERT_0712_WINTER_EVENT_COUPON* v =
        guard.operator->()->GetInBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>();
    v->m_accId = accId;
    v->m_characNo = characNo;
    v->m_count = count;

    return guard.Get();
}

// ---- DB_Insert0712WinterEventCoupon::makeRequest ORIG @ 0x0842e648 ----
// 0x112 → DB_Q（ORIG put 第 2 实参 2）；成功再 0x113 → LOGDB_Q（实参 4）。
void DB_Insert0712WinterEventCoupon::makeRequest(unsigned int accId,
                                                 unsigned int characNo,
                                                 unsigned int count)
{
    Stream* first = _makeRequest((ENUM_INTERNALPACKET)0x112, accId, characNo, count);
    if (first != 0)
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, first);
        Stream* second =
            _makeRequest((ENUM_INTERNALPACKET)0x113, accId, characNo, count);
        if (second != 0)
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, second);
    }
}

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

// ---- DB_InsertCeraItemHistory::dispatch ORIG @ 0x8445032 ----
// 读流 itemIdx(int)/sender(uint)/receiver(uint)/flag(char 符号扩展)，DB_HANDLE_3，
// set_query("inSert into user_items_change_history(ui_id,from_charac_no,
// to_charac_no,occ_date,change_type) values(%u,%u,%u,now(),%u)", itemIdx,
// sender, receiver, (int)flag) → exec(true) → if(!ok) false else true。
bool DB_InsertCeraItemHistory::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int senderCharacNo = 0;
    unsigned int receiverCharacNo = 0;
    int itemIdx = 0;
    char flag = 0;
    *stream >> itemIdx;
    *stream >> senderCharacNo;
    *stream >> receiverCharacNo;
    *stream >> flag;

    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into user_items_change_history(ui_id,from_charac_no,to_charac_no,"
        "occ_date,change_type) values(%u,%u,%u,now(),%u)",
        itemIdx, senderCharacNo, receiverCharacNo, (int)flag);
    if (!db->exec(true))
        return false;
    else
        return true;
}

// ---- DB_ReqSaveHackUserPunish::dispatch ORIG @ 0x8425374 ----
// GetOutBuffer<SIG_SAVE_HACK_USER_PUNISH>；取 DB_HANDLE_6 与 DB_HANDLE_1 两个句柄
// （前者未用）。先 upDate member_punish_info ... where m_id=%s and punish_type=%d
// （m_id=NumberToString(accId,0)，punish_type=2 字面量）；若 exec 失败或影响行数
// 为 0，再 inSert into member_punish_info(...) values(%s, %d, ...) 同参；第二条
// exec 失败 → 返 false，否则 true。
bool DB_ReqSaveHackUserPunish::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_HACK_USER_PUNISH* sig =
        stream->GetOutBuffer<SIG_SAVE_HACK_USER_PUNISH>();
    MySQL* db6 =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    MySQL* db1 =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);

    db1->set_query(
        "upDate member_punish_info set occ_time=now(), start_time=now(), "
        "end_time='9999-12-31 23:59:59', punish_value=0, apply_flag=2 where "
        "m_id=%s and punish_type=%d",
        NumberToString(sig->m_accId, 0), 2);
    if (!db1->exec(true) || db1->getAffectedRowCount() == 0)
    {
        db1->set_query(
            "inSert into member_punish_info(m_id, punish_type, occ_time, "
            "start_time, end_time, punish_value, apply_flag) values(%s, %d, "
            "now(), now(), '9999-12-31 23:59:59', 0, 2)",
            NumberToString(sig->m_accId, 0), 2);
        if (!db1->exec(true))
            return false;
    }
    return true;
}

// ---- DB_UpdateHappyBeanInfo::dispatch ORIG @ 0x84258ee ----
// 读流 uid(uint)/characNo(uint)/name(char* 0x1d，读而未用)；DB_HANDLE_1。
// upDate event_happybean_log set cnt=cnt+1 where occ_date=now() and m_id=%s；
// 若 exec 失败或影响行数 0，inSert into event_happybean_log(occ_date,m_id,cnt)
// values(now(),%s,1)。恒返 1。
bool DB_UpdateHappyBeanInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    unsigned int uid = 0;
    unsigned int characNo = 0;
    char name[0x1d];
    *stream >> uid;
    *stream >> characNo;
    *stream >> name;

    db->set_query(
        "upDate event_happybean_log set cnt=cnt+1 where occ_date=now() and m_id=%s",
        NumberToString(uid, 0));
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_happybean_log(occ_date,m_id,cnt) values(now(),%s,1)",
            NumberToString(uid, 0));
        db->exec(true);
    }
    return 1;
}

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
bool DB_AccountHack_CheckMyAccount::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CHECK_MY_ACCOUNT_IN_DB* sig =
        stream->GetOutBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>();
    unsigned int m_id = 0;
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    char name[0x3b] = { '\0' };

    db->escape_string(name, sig->m_name);   // +0x08
    db->set_query(
        "seLect m_id from charac_info where charac_name='%s' limit 1", name);
    if (!db->exec(true))
        return false;
    if (!db->fetch())
        return false;
    if (!db->get_int(0, m_id))
        return false;

    if (reinterpret_cast<DBU3AccountHackIn*>(sig)->m_accId != m_id)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8833), true);
        guard.operator*()->operator<<(0x18c);          // SIG_CHECK_MY_ACCOUNT_OK
        guard.operator*()->operator<<(slot);

        DBU3AccountHackOut* out =
            reinterpret_cast<DBU3AccountHackOut*>(guard.operator->()->GetInBuffer<SIG_CHECK_MY_ACCOUNT_OK>());
        out->m_a = reinterpret_cast<DBU3AccountHackIn*>(sig)->m_a;          // +0x00
        out->m_money = reinterpret_cast<DBU3AccountHackIn*>(sig)->m_money;  // +0x25
        out->m_id = m_id;

        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return true;
}
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
bool DB_Insert0712WinterEventCoupon::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_INSERT_0712_WINTER_EVENT_COUPON* sig =
        stream->GetOutBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>();

    if (userIdx == 0x112)
    {
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate event_goldcard_info set coupon=coupon+%d where m_id=%s",
            reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count,
            NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0));
        if (!db->exec(true) || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into event_goldcard_info(m_id,coupon) values(%s,%d)",
                NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0),
                reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count);
            db->exec(true);
        }
    }
    else
    {
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate event_goldcard_log set cnt=cnt+%d where m_id=%s and occ_date=now()",
            reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count,
            NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0));
        if (!db->exec(true) || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into event_goldcard_log(m_id,cnt,occ_date) values(%s,%d,now())",
                NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0),
                reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count);
            db->exec(true);
        }
    }
    return true;
}

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
bool DB_InsertArchieveEventLog::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    SIG_INSERT_ACHIEVE_EVENT* sig =
        stream->GetOutBuffer<SIG_INSERT_ACHIEVE_EVENT>();

    db->set_query(
        "inSert into log_charac_lev_log(occ_time,m_id,charac_no,achieve_type) "
        "values(now(),%s,%d,%d)",
        NumberToString(reinterpret_cast<DBU3AchieveIn*>(sig)->m_accId, 0),
        reinterpret_cast<DBU3AchieveIn*>(sig)->m_characNo,
        reinterpret_cast<DBU3AchieveIn*>(sig)->m_type);
    db->exec(true);
    return true;
}

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
bool DB_InsertUnlimitSupportLog::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    SIG_INSERT_UNLIMIT_SUPPORT_LOG* sig =
        stream->GetOutBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>();

    db->set_query(
        "inSert into event_unlimitsupport_lev(m_id,occ_time,server_id,charac_no,"
        "charac_name) values(%s,now(),%d,%d,'%s')",
        NumberToString(reinterpret_cast<DBU3UnlimitIn*>(sig)->m_accId, 0),
        reinterpret_cast<DBU3UnlimitIn*>(sig)->m_serverId,
        reinterpret_cast<DBU3UnlimitIn*>(sig)->m_characNo,
        reinterpret_cast<DBU3UnlimitIn*>(sig)->m_name);
    db->exec(true);
    return true;
}
