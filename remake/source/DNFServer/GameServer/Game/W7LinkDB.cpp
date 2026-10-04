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

// [AnonLane2 2026-09-11 匿名轴] 旧匿名 helper
// `_ZN12_GLOBAL__N_114put_db_requestER12CStreamGuard`（ORIG 全库零命中）删除：
// 全库（本 TU 的 7 个 makeRequest.inc + 其它 TU）**零调用点**，属 P154 死函数；
// 本 TU 的 7 个 makeRequest 强符号（T）由各自 .inc 定义发射，不受影响。

#include "W7LinkDB/makeRequest.inc"
                                                unsigned int uid, unsigned int who)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8912), true);

    **guard << 0x1ca;                   // SIG opcode（op<<(int)）
    **guard << static_cast<int>(uid);   // op<<(int)，ORIG 080c8c56

    // GetInBuffer<SIG_SECOND_USER_DATA>：ORIG memset(payload, 0, 0x14)
    {
        SIG_SECOND_USER_DATA* v =
            guard->GetInBuffer<SIG_SECOND_USER_DATA>();
        memset(v, 0, 0x14);
        v->m_mId = accId;
        v->m_gold = gold;
        v->m_tradeCnt = (int)uid;
        v->m_punished = who;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

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
        SIG_INSERT_AUTO_PUNISH_FIRST_USER* v =
            guard->GetInBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>();
        v->m_memberId = accId;
        strncpy(v->m_ip, ip, 0x10);
        v->m_hackType = a3;
        v->m_cnt = a4;
        v->m_hackSubType = a5;
        v->m_hackSubCnt = a6;
        v->m_punishFlag = a7;
        v->m_etcValue = a8;
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
        SIG_INSERT_AUTO_PUNISH_SECOND_USER* v =
            guard->GetInBuffer<SIG_INSERT_AUTO_PUNISH_SECOND_USER>();
        v->m_memberId = accId;
        v->m_hackMemberId = who;
        v->m_gold = gold;
        v->m_punishFlag = punish;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "W7LinkDB/makeRequest_3.inc"


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
        SIG_HACK_IP_SAVE* v =
            guard->GetInBuffer<SIG_HACK_IP_SAVE>();
        strncpy(v->m_classIp, startIp, 0xd);
        v->m_hackType = (int)a3;
        v->m_hackSubType = (int)a4;
        strncpy(v->m_fullIp, rawIp, 0x10);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "W7LinkDB/makeRequest_5.inc"


#include "W7LinkDB/makeRequest_6.inc"

