// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W7LinkDB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "GlobalData.h"
#include "StreamPool.h"
#include "Stream.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "SigTypes.h"

#include <string.h>

// ---- 类声明（与 CUserOnlyOrig.cpp:76-85 / W8_CU_G.cpp 局部声明签名一致；ORIG nm 实证）----
class DB_AutoPunishedSecondUserData
{
public:
    static void makeRequest(unsigned int accId, unsigned long long gold,
                            unsigned int uid, unsigned int who);
};

class DB_InsertAutoPunishFirstUser
{
public:
    static void makeRequest(unsigned int accId, char const* ip,
                            unsigned int a3, unsigned int a4,
                            unsigned int a5, unsigned int a6,
                            unsigned short a7, unsigned int a8);
};

class DB_InsertAutoPunishSecondUser
{
public:
    static void makeRequest(unsigned int accId, unsigned int who,
                            unsigned long long gold, bool punish);
};

class DB_SaveGiveAvengerTitleFlag
{
public:
    static void makeRequest(int uid, unsigned int accId, unsigned int characNo);
};

class DB_HackIPSave
{
public:
    static void makeRequest(char const* startIp, char const* rawIp,
                            unsigned int a3, unsigned int a4);
};

class DB_ForceSaveSimpleSSO
{
public:
    static void makeRequest(unsigned int accId, int errCnt);
};

class DB_CountOfHumanCertify
{
public:
    static void makeRequest(unsigned int accId);
};

namespace {

#include "W7LinkDB/put_db_request.inc"
{
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

}  // namespace

// ----------------------------------------------------------------------------
// DB_AutoPunishedSecondUserData::makeRequest(uint accId, ull gold, uint uid, uint who)
// ORIG @ 0x0843a790（报告 docs/class_func_reports/DB_AutoPunishedSecondUserData/
// makeRequest.md）：Acquire("DBThread.cpp", 0x8912) → guard(stream,true) →
//   << 0x1ca | << (uint)uid(arg@0x14)
// → GetInBuffer<SIG_SECOND_USER_DATA>()（0x14 字节）清零后填：
//   +0x00 = accId, +0x04..0x0b = gold(8B), +0x0c = uid, +0x10 = who
// → put(DB_Q)。
#include "W7LinkDB/makeRequest.inc"
                                                unsigned int uid, unsigned int who)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8912), true);

    **guard << 0x1ca;                   // SIG opcode（op<<(int)）
    **guard << static_cast<int>(uid);   // op<<(int)，ORIG 080c8c56

    // GetInBuffer<SIG_SECOND_USER_DATA>：ORIG memset(payload, 0, 0x14)
    {
        struct SecondUserDataView   // SIG_SECOND_USER_DATA 载荷视图
        {
            unsigned int m_accId;        // +0x00
            unsigned long long m_gold;   // +0x04
            unsigned int m_uid;          // +0x0c
            unsigned int m_who;          // +0x10
        };
        SecondUserDataView* v =
            reinterpret_cast<SecondUserDataView*>(guard->GetInBuffer<SIG_SECOND_USER_DATA>());
        memset(v, 0, 0x14);
        v->m_accId = accId;
        v->m_gold = gold;
        v->m_uid = uid;
        v->m_who = who;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ----------------------------------------------------------------------------
// DB_InsertAutoPunishFirstUser::makeRequest(uint accId, const char* ip,
//     uint a3, uint a4, uint a5, uint a6, ushort a7, uint a8)
// ORIG @ 0x0842dd7c（报告 DB_InsertAutoPunishFirstUser/makeRequest.md）：
// Acquire("DBThread.cpp", 0x723e) → << 0x10a | << -1 →
// GetInBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>()（0x2c 字节，无 memset）填：
//   +0x00 = accId；strncpy(+0x04, ip, 0x10)；+0x14..+0x20 = a3..a6；
#include "W7LinkDB/makeRequest_1.inc"
                                               unsigned int a3, unsigned int a4,
                                               unsigned int a5, unsigned int a6,
                                               unsigned short a7, unsigned int a8)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x723e), true);

    **guard << 0x10a;
    **guard << -1;

    {
        struct InsertAutoPunishFirstView   // SIG_INSERT_AUTO_PUNISH_FIRST_USER 载荷视图
        {
            unsigned int m_accId;        // +0x00
            char m_ip[0x10];             // +0x04
            unsigned int m_a3;           // +0x14
            unsigned int m_a4;           // +0x18
            unsigned int m_a5;           // +0x1c
            unsigned int m_a6;           // +0x20
            unsigned short m_a7;         // +0x24
            unsigned int m_a8;           // +0x28
        };
        InsertAutoPunishFirstView* v = reinterpret_cast<InsertAutoPunishFirstView*>(
            guard->GetInBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>());
        v->m_accId = accId;
        strncpy(v->m_ip, ip, 0x10);
        v->m_a3 = a3;
        v->m_a4 = a4;
        v->m_a5 = a5;
        v->m_a6 = a6;
        v->m_a7 = a7;
        v->m_a8 = a8;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "W7LinkDB/makeRequest_2.inc"
                                                unsigned long long gold, bool punish)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x72f4), true);

    **guard << 0x10b;
    **guard << -1;

    {
        struct InsertAutoPunishSecondView   // SIG_INSERT_AUTO_PUNISH_SECOND_USER 载荷视图
        {
            unsigned int m_accId;        // +0x00
            unsigned int m_who;          // +0x04
            unsigned long long m_gold;   // +0x08
            bool m_punish;               // +0x10（P211：与 RHS punish(bool) 同型化⇒复现 ORIG LHS-first `mov -0xc,%eax; movzbl -0x24,%edx; mov %dl,0x10(%eax)` @0x0842e34e）
        };
        InsertAutoPunishSecondView* v = reinterpret_cast<InsertAutoPunishSecondView*>(
            guard->GetInBuffer<SIG_INSERT_AUTO_PUNISH_SECOND_USER>());
        v->m_accId = accId;
        v->m_who = who;
        v->m_gold = gold;
        v->m_punish = punish;            // bool↔bool 同型直存（movzbl/mov %dl）
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ----------------------------------------------------------------------------
// DB_SaveGiveAvengerTitleFlag::makeRequest(int uid, uint accId, uint characNo)
// ORIG @ 0x084422dc（报告 DB_SaveGiveAvengerTitleFlag/makeRequest.md）：
// Acquire("DBThread.cpp", 0xa276) → << 0x28d | << (uint)uid | << accId |
//   << characNo（全部 op<<(unsigned int)，ORIG 080e2292 三连）→ put(DB_Q)。
// 注意：ORIG 此处无 -1 槽。
// ----------------------------------------------------------------------------
#include "W7LinkDB/makeRequest_3.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa276), true);

    **guard << 0x28d;                   // op<<(int)
    **guard << uid;                     // op<<(int)（ORIG 080c8c56）
    **guard << accId;                   // op<<(uint)
    **guard << characNo;                // op<<(uint)

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ----------------------------------------------------------------------------
// DB_HackIPSave::makeRequest(const char* startIp, const char* rawIp, uint a3, uint a4)
// ORIG @ 0x08436ff8（报告 DB_HackIPSave/makeRequest.md）：
// Acquire("DBThread.cpp", 0x82be) → << 400(0x190) | << -1；
// 守卫：仅当 startIp==NULL 或 strlen(startIp)>7 时继续（否则直接返回不投递）。
// GetInBuffer<SIG_HACK_IP_SAVE>()（0x28 字节，无 memset）填：
//   strncpy(+0x00, startIp, 0xd)；+0x20 = a3；+0x24 = a4；
//   strncpy(+0x0d, rawIp, 0x10)。→ put(DB_Q)。
// ----------------------------------------------------------------------------
#include "W7LinkDB/makeRequest_4.inc"
                                unsigned int a3, unsigned int a4)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x82be), true);

    **guard << 400;
    **guard << -1;

    if (startIp != 0)
    {
        if (strlen(startIp) <= 7)
            return;                                        // ORIG jbe → 直接到析构返回
    }

    {
        struct HackIpSaveView   // SIG_HACK_IP_SAVE 载荷视图
        {
            char m_startIp[0xd];     // +0x00
            char m_rawIp[0x10];      // +0x0d
            char m_pad[3];           // +0x1d
            unsigned int m_a3;       // +0x20
            unsigned int m_a4;       // +0x24
        };
        HackIpSaveView* v = reinterpret_cast<HackIpSaveView*>(
            guard->GetInBuffer<SIG_HACK_IP_SAVE>());
        strncpy(v->m_startIp, startIp, 0xd);
        v->m_a3 = a3;
        v->m_a4 = a4;
        strncpy(v->m_rawIp, rawIp, 0x10);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ----------------------------------------------------------------------------
// DB_ForceSaveSimpleSSO::makeRequest(uint accId, int errCnt)
// ORIG @ 0x084281ae（报告 DB_ForceSaveSimpleSSO/makeRequest.md）：
// Acquire("DBThread.cpp", 0x69b1) → << 0xcf | << -1 | << accId(op<<uint) |
//   << errCnt(op<<int) → put(DB_Q)。
// ----------------------------------------------------------------------------
#include "W7LinkDB/makeRequest_5.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x69b1), true);

    **guard << 0xcf;
    **guard << -1;
    **guard << accId;
    **guard << errCnt;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ----------------------------------------------------------------------------
// DB_CountOfHumanCertify::makeRequest(uint accId)
// ORIG @ 0x084283ca（报告 DB_CountOfHumanCertify/makeRequest.md）：
// Acquire("DBThread.cpp", 0x69d6) → << 0x111 | << -1 | << accId(op<<uint)
// → put(DB_Q)。
// ----------------------------------------------------------------------------
#include "W7LinkDB/makeRequest_6.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x69d6), true);

    **guard << 0x111;
    **guard << -1;
    **guard << accId;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
