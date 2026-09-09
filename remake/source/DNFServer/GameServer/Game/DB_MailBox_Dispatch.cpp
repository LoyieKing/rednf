// ============================================================================
// df_game_r 还原 —— DB_MailBox 域 DB 侧 dispatch 任务（OrigMailWave18 批，
// 独占 TU，未入 CMake，待 Main 登记）
// 覆盖 7 个 ORIG T 符号（地址见 DB_MailBox_Dispatch.h 头注释）：
//   1. getList2inQuery(uint*, char*)                    0x083ff968
//   2. DB_MailBox_Req_Delete::dispatch                 0x0841f084
//   3. DB_MailBox_Req_CheckReceiver::dispatch          0x0841c882
//   4. DB_Multi_MailBox_Req_Send::dispatch             0x0843bada
//   5. DB_Multi_MailBox_Req_Send::_sendResult          0x0843bc16
//   6. DB_ReqQueryCharacInfoMailbox::dispatch          0x0843bd46
//   7. DB_ReqQueryCharacInfoMailbox::_sendResult       0x0843c224
// 报告均已全文阅读后转写；SIG 布局：SIG_MAILBOX_DEL_MAIL / SIG_MAILBOX_LIST /
//   SIG_MAILBOX_CHECK_RECEIVER / SIG_QUERY_CHARAC_INFO（2026-09-05 §4.3 已字段化，
//   本 TU 全部具名字段访问）。m_payload 透传区内 +0xaa 标题串（= this+0xdb）无字段化
//   证据，用 `(const char*)((unsigned int)sig + 0xdb)` 整型加法形复现 ORIG 0x0843bb71
//   两步 `add $0xdb,%eax; mov %eax,%ecx`（P319；数组退化在 4.4 恒折单条 lea）。
// 依赖：DB_MailBox_Req_Send::InsertLetter（DB_MailBox_Req_Send.h，已实现）；
//   NumberToString 定义归 LinkFinalDeps.cpp；DBDispatcher::dispatch 归 DB_Login.cpp。
// ============================================================================

#include "DB_MailBox_Dispatch.h"

#include "SigTypes.h"       // SIG_MAILBOX_DEL_MAIL / SIG_MAILBOX_CHECK_RECEIVER / SIG_QUERY_CHARAC_INFO
#include "GlobalData.h"     // s_db_mgr / s_systemTime_ / s_stream_pool / s_big_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"      // cMyTrace
#include "LogManager.h"
#include "DNFFunctionLib.h" // NumberToString
#include "DB_MailBox_Req_Send.h"  // DB_MailBox_Req_Send::InsertLetter / ENUM_LETTER_STAT

#include <ctime>
#include <cstring>
#include <cstdio>

// ============================================================================
// 1. getList2inQuery —— ORIG 0x083ff968（_lt_global_gt_/getList2inQuery.md）：
//    out[0]=0 → memcpy(out,"in (",5) → for(i=1; i<list[0]; ++i)
//    sprintf(out,"%s%d,",out,list[i]) → sprintf(out,"%s%d)",out,list[i]) → out。
// ============================================================================
char* getList2inQuery(unsigned int* list, char* out)
{
    out[0] = '\0';
    memcpy(out, "in (", 5);
    unsigned int i;
    for (i = 1; i < list[0]; ++i)
    {
        sprintf(out, "%s%d,", out, list[i]);
    }
    sprintf(out, "%s%d)", out, list[i]);
    return out;
}

// ============================================================================
// 2. DB_MailBox_Req_Delete::dispatch —— ORIG 0x0841f084（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_DEL_MAIL> → GetDBHandle(DB_HANDLE_3,0)；
//    count==0 返 0；count==1 单条 upDate（postal_id=%d）；否则
//    getList2inQuery 生成列表串（postal_id %s）；exec(true) 失败返 0，成功返 1。
//    局部 static char strQuery[]（ORIG _ZZN21DB_MailBox_Req_Delete8dispatch
//    EiiP6StreamE8strQuery .bss 证据）。
// ============================================================================
bool DB_MailBox_Req_Delete::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_MAILBOX_DEL_MAIL* sig = stream->GetOutBuffer<SIG_MAILBOX_DEL_MAIL>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    // 具名字段绑定（0x0841f084 报告：+0x00 count movl、+0x04 首 idx、+0x54 SQL
    // delete_flag 实参；SIG_MAILBOX_DEL_MAIL union 视图见 SigTypes_D.h）。
    if (sig->m_count == 0)
        return 0;

    if (sig->m_count == 1)
    {
        db->set_query(
            "upDate postal set delete_flag=%d,receive_time=now() where postal_id=%d",
            sig->m_type, sig->m_postalIds[0]);
    }
    else
    {
        static char strQuery[0x100];
        db->set_query(
            "upDate postal set delete_flag=%d,receive_time=now() where postal_id %s",
            sig->m_type, getList2inQuery(&sig->m_count, strQuery));
    }

    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// 3. DB_MailBox_Req_CheckReceiver::dispatch —— ORIG 0x0841c882（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER> → 双 DB 句柄(2/3) →
//    charac_info 按收件人名查 charac_no/lev/delete_flag/m_id（循环读全部行）→
//    BigStreamPool 回包：<<0x7e <<slot，回填 +0x1d/+0x21/+0x25/+0x29，
//    memcpy(+0x31,0x3f88) 载荷透传 → 黑名单 count(*)>0 时 +0x3fb9=0x4d → put。
// b28d-SweepDBMailBoxDispatch（AE，266=266，jcc 序列逐位全等）：三处真差异已修——
//   ① SQL 串补具名 char* 局部（ORIG 0x0841c8f8 存 -0x24、0x0841c980 回读；P244）；
//   ② NumberToString 内联进 set_query 实参位（ORIG 0x0841cb73 先求值 sender 入 %ebx
//      跨 call 保活，cdecl 右→左；P329）；
//   ③ 黑名单判定改嵌套 if（ORIG 0x0841cbd1/cbe0/cbfe/cc05 四个独立 je 同出口；P225）。
//   另删 sender 实参冗余 (int) cast（P265：unsigned 字段+异型 cast 致 mem→eax→ebx 两段，
//   ORIG 为单段 `mov 0x2d(%eax),%ebx`）。
// ============================================================================
bool DB_MailBox_Req_CheckReceiver::dispatch(int userIdx, int slot, Stream* stream)
{
    // [b28d-48] N82 封顶（NDIFF 218，271/266 条）：帧/槽平移 + 判定链块序。
    SIG_MAILBOX_CHECK_RECEIVER* sig = stream->GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    int characNo = 0;
    int lev = 0;
    int deleteFlag = 0;
    unsigned int mId = 0;

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, (ENUM_SERVER_GROUP)0);
    MySQL* db3 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, (ENUM_SERVER_GROUP)0);

    // P244：ORIG 0x0841c8f8 `movl $SQL,-0x24(%ebp)`（0x0841c980 回读后压 0x4(%esp)）
    // ⇒ 原码是具名 char* 局部；store 发射在 name 清零之前（ORIG 0x0841c8f8 早于
    // 0x0841c8ff 的清零段）。
    const char* query =
        "seLect charac_no,lev,delete_flag,m_id from charac_info where charac_name='%s'";
    char name[0x3b] = { '\0' };  // ORIG 0x3b 字节（2*0x1d+1）
    db2->escape_string(name, sig->m_receiverName);
    db2->set_query(query, name);
    if (!db2->exec(true))
        return 0;

    for (unsigned int i = 0; i < (unsigned int)db2->get_n_rows(); ++i)
    {
        if (!db2->fetch())
            return 0;
        if (!db2->get_int(0, characNo))
            return 0;
        if (!db2->get_int(1, lev))
            return 0;
        if (!db2->get_int(2, deleteFlag))
            return 0;
        if (!db2->get_uint(3, mId))
            return 0;
    }

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x509a), true);
    guard.operator*()->operator<<(0x7e);
    guard.operator*()->operator<<(slot);

    SIG_MAILBOX_CHECK_RECEIVER* out = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    out->m_receiverCharacNo = (unsigned int)characNo;
    out->m_receiverLevel = (unsigned int)lev;
    out->m_receiverDeleteFlag = (unsigned int)deleteFlag;
    out->m_receiverAccId = mId;
    memcpy(out->m_payload, sig->m_payload, 0x3f88);

    // P329：NumberToString 内联进实参位。ORIG 0x0841cb70 先读 sender 入 %ebx 跨
    // NumberToString call 保活、0x0841cb84 才 call NumberToString（cdecl 右→左求值）；
    // 具名 mIdStr 局部会把 sender 压回栈槽（4.4 不升格）并多一条中转。
    if (db3->set_query(
            "seLect count(*) from charac_black_list where m_id = %s and charac_no = %d",
            NumberToString(mId, 0), sig->m_senderCharacNo))
    {
        int blackCnt = 0;
        out->m_resultFlag = 0;
        // P225：ORIG 四个独立 `test/je` 到同一出口（0x0841cbd1/cbe0/cbfe/cc05）⇒
        // 嵌套 if；`&&` 链在 4.4 会把整链物化成 bool 岛（mov$1/jmp/mov$0/test/je）。
        if (db3->exec(true))
        {
            if (db3->fetch())
            {
                if (db3->get_int(0, blackCnt))
                {
                    if (blackCnt > 0)
                        out->m_resultFlag = 0x4d;
                }
            }
        }
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// 4. DB_Multi_MailBox_Req_Send::dispatch —— ORIG 0x0843bada（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER> → 到期时间 = 下一整点
//    （getCurSec+1h 清零分秒 → mktime）→ GetDBHandle(DB_HANDLE_3,0) →
//    InsertLetter(db, +0x1d 收件人, +0x2d 发件人, +0x3fc1 name, +0xdb title,
//    +0x3fbd letterId, t, 0)，失败 logFormat(0x8e53) → _sendResult(slot, sig)
//    → 返 1。
// b28c-L20: N1 — 唯一真残差 `sig->m_payload + 0xaa`（m_payload char[0x3f88] @+0x31 ⇒
//   +0xdb）：ORIG 0x0843bb71 `add $0xdb,%eax; mov %eax,%ecx` 两步，4.4 恒折单条
//   `lea 0xdb(%eax),%esi`（数组退化+常量折叠），其余为寄存器名对调（N82附）。ndiff 13。
// ============================================================================
bool DB_Multi_MailBox_Req_Send::dispatch(int userIdx, int slot, Stream* stream)
{
    // b28d-SweepDBMailBoxDispatch（AE，84=84，jcc 全等）：title 实参已改整型加法形
    // 复现 ORIG 两步 add/mov（见调用点注释）。
    static const int handleIdx = 3;
    SIG_MAILBOX_CHECK_RECEIVER* sig = stream->GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER>();

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmBuf;
    struct tm* tmPtr = localtime_r(&now, &tmBuf);
    tmPtr->tm_hour += 1;
    tmPtr->tm_min = 0;
    tmPtr->tm_sec = 0;
    time_t t = mktime(tmPtr);

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    if (!DB_MailBox_Req_Send::InsertLetter(
            db, (int)sig->m_receiverCharacNo, (int)sig->m_senderCharacNo,
            sig->m_senderName,                 // +0x3fc1（0843bb6e lea）
            // 报告 +0xdb：透传区内标题串（区无字段化证据）。等价于 m_payload+0xaa
            // （m_payload@+0x31）；P319 整型加法形复现 ORIG 0x0843bb71 两步
            // `add $0xdb,%eax; mov %eax,%ecx`（数组退化在 4.4 恒折单条 lea）。
            (const char*)((unsigned int)sig + 0xdb),
            sig->m_letterId,                   // +0x3fbd int&（0843bb65 lea；InsertLetter Ri）
            t, LETTER_STAT_NONE))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x8e53,
                              "InsertLetter() error");
    }

    _sendResult(slot, sig);
    return 1;
}

// ============================================================================
// 5. DB_Multi_MailBox_Req_Send::_sendResult —— ORIG 0x0843bc16（_sendResult.md）：
//    BigStreamPool::Acquire("DBThread.cpp",0x8e67) → CStreamGuard(stream,true) →
//    <<0x1f5 → cMyTrace("bool ..._sendResult(int, SIG_MAILBOX_CHECK_RECEIVER*)",
//    0x8e6a,0)("E_MULTI_MAILBOX_REQ_SEND : %d",0x1f5) → <<slot →
//    GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER> → memcpy 0x3fde → put(DISPATCH_Q) → 1。
// b28c-L20: AE — 全表达式临时链 cMyTrace(...)(...) 复现 ORIG 槽方向（guard -0x24 深 /
//   trace -0x1c 浅；具名 trace 时 4.4 反向，P221/P227）。
// ============================================================================
bool DB_Multi_MailBox_Req_Send::_sendResult(int slot, SIG_MAILBOX_CHECK_RECEIVER* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x8e67), true);

    guard.operator*()->operator<<(0x1f5);

    // b28c-L20: 全表达式临时链（P221/P227）复现 ORIG 0x0843bc85 的 ctor/operator()
    // 槽方向（ORIG trace -0x1c 浅 / guard -0x24 深；具名 trace 时 4.4 反向）。
    cMyTrace(__PRETTY_FUNCTION__, 0x8e6a, 0)("E_MULTI_MAILBOX_REQ_SEND : %d", 0x1f5);
    guard.operator*()->operator<<(slot);
    SIG_MAILBOX_CHECK_RECEIVER* out = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    memcpy(out, sig, 0x3fde);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
}

// ============================================================================
// 6. DB_ReqQueryCharacInfoMailbox::dispatch —— ORIG 0x0843bd46（dispatch.md 全文）：
//    基类 DBDispatcher::dispatch 失败返 0 → GetDBHandle(DB_HANDLE_2,0) →
//    GetOutBuffer<SIG_QUERY_CHARAC_INFO>（首字节即角色名）→ escape_string →
//    charac_info 查 7 列；exec 失败/无行/取行失败 → _sendResult(2,...)；
//    delete_flag!=0 → _sendResult(0x15,...)；成功 → 读列 1..6 →
//    _sendResult(0, slot, sig, characNo, job, lev, growType, sex) → 返 1
//    （ORIG +0x377 第 3 参 name 传 sig=GetOutBuffer 指针，非转义缓冲）。
//    注意 ORIG 对 get_n_rows()==0 有两次独立判定（P48 双调用），照写。
//    b28d-SweepDBMailBoxDispatch（AE，227=227，jcc 序列逐位全等）：4 处失败路径改
//    `return _sendResult(...)`，复现 ORIG 0x0843be5b/bebe/bf6f/bfc5 的 `call; jmp +0x3e6`
//    （跳过 mov $1）⇒ _sendResult 返回类型随之改 bool（P292 探针 f/g 实证：int 返回
//    会多 test/setne 归一化）。成功路径仍 goto done 共享单一置位（ORIG 0x0843c0db
//    `jmp +0x3e1` 到 mov $1）。
// ============================================================================
bool DB_ReqQueryCharacInfoMailbox::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, (ENUM_SERVER_GROUP)0);
    SIG_QUERY_CHARAC_INFO* sig = stream->GetOutBuffer<SIG_QUERY_CHARAC_INFO>();

    // P212 候选实验（NL31）：块内局部全部提头声明（未初始化），初始化语句
    // 留在 ORIG 原位——4.4 全头部声明时槽分配与 4.1 同为"先声明深"单调。
    // 声明序=ORIG 槽深序（get_int 列序≠声明序；ORIG movl 序 -0x2c,-0x30,
    // -0x34,-0x38,-0x1c(dummy),-0x3c,-0x40 ⇒ job,lev,growType,characNo,
    // pad_impl,sex,mId）。
    int deleteFlag;
    int job;
    int lev;
    int growType;
    int characNo;
    int pad_impl;
    int sex;
    unsigned int mId;

    char name[0x3c] = { '\0' };
    db->escape_string(name, sig->m_name);
    db->set_query(
        "seLect delete_flag,m_id,charac_no,job,lev,grow_type,sex from charac_info where charac_name='%s'",
        name);
    // ORIG 0x0843be5b/0x0843bebe：失败路径 `call _sendResult` 后直接
    // `jmp +0x3e6`（跳过 mov $1,%eax）⇒ 原码是 `return _sendResult(...)`
    // （_sendResult 返 bool，无 int→bool 归一化）。
    if (!db->exec(true))
        return _sendResult(2, slot, 0, 0, 0, 0, 0, 0);

    if (db->get_n_rows() == 0)
        return _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);
    // P48 双调用：ORIG（0x0843bec3 `setne %al; je 0843c0dd`）第二次判定是
    // `!= 0` 正向跳成功链、`== 0` 落入尾部 0x15 块（fallthrough）→ 按该形态
    // 转写（行为与两连 `==0` 块等价；块形状影响 verdict 序列）。
    if (db->get_n_rows() != 0)
    {
        deleteFlag = 0;
        if (!db->fetch() || !db->get_int(0, deleteFlag))
            return _sendResult(2, slot, 0, 0, 0, 0, 0, 0);
        if (deleteFlag != 0)
            return _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);

        // 宽度对齐 ORIG（报告 0x0843c02a..0x0843c092）：列 2..6 callee =
        // MySQL::get_int(int, int&)@0x0811692c → 局部须为 int（原 unsigned int
        // 会绑 0x080e22ae 转发重载，callee 不符）；列 1 m_id callee @0x080e22ae
        // = get_int(int, unsigned int&) → mId 保持 unsigned int。
        // ORIG（+0x2a0 `movl $0x0,-0x1c(%ebp)`）在此列初始化块多零写入一个 dummy
        // 槽（只写不读，N82 遗留）。以同块内一个只写不读的局部复刻该条死 movl。
        job = 0;
        lev = 0;
        growType = 0;
        characNo = 0;
        pad_impl = 0;
        sex = 0;
        mId = 0;
        db->get_int(1, mId);
        db->get_int(2, characNo);
        db->get_int(3, job);
        db->get_int(4, lev);
        db->get_int(5, growType);
        db->get_int(6, sex);

        // ORIG（0843c0bd mov -0x20(%ebp),%eax）：第 3 参 name 传的是 GetOutBuffer
        // 返回的 sig 指针，NOT 转义后的 name 缓冲（0843c0bb..0843c0bd 用 -0x20 槽）。
        _sendResult(0, slot, (const char*)sig, characNo, job, lev, growType, sex);
        goto done;
    }

    _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);
done:
    return 1;
}

// ============================================================================
// 7. DB_ReqQueryCharacInfoMailbox::_sendResult —— ORIG 0x0843c224（_sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0x8ed6) → CStreamGuard(stream,true) →
//    <<0x1f6 → <<slot → GetInBuffer<SIG_QUERY_CHARAC_INFO> → +0x20=errCode；
//    errCode==0 时 strncpy(name,0x1d)、+0x24=characNo、+0x28=(char)job、
//    +0x2a=(short)lev、+0x2c=(char)growType、+0x2d=(char)sex → put → 返 1。
// ============================================================================
bool DB_ReqQueryCharacInfoMailbox::_sendResult(int errCode, int slot, const char* name,
                                               int characNo, int job, int lev,
                                               int growType, int sex)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8ed6), true);

    guard.operator*()->operator<<(0x1f6);
    guard.operator*()->operator<<(slot);

    SIG_QUERY_CHARAC_INFO* out = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    out->m_errCode = errCode;
    if (errCode == 0)
    {
        strncpy(out->m_name, name, 0x1d);
        out->m_characNo = characNo;
        out->m_job = (char)job;
        out->m_lev = (short)lev;
        out->m_growType = (char)growType;
        out->m_sex = (char)sex;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}