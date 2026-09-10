// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W7LinkDB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W7LinkDB.cpp —— W7 链接收敛批（DB 类 makeRequest 真实定义唯一 owner）
// 覆盖 ORIG 符号（nm /home/wangyilei/下载/df_game_r 实证）：
//   T 0x0843a790 _ZN29DB_AutoPunishedSecondUserData11makeRequestEjyjj
//   T 0x0842dd7c _ZN28DB_InsertAutoPunishFirstUser11makeRequestEjPKcjjjjtj
//   T 0x0842e34e _ZN29DB_InsertAutoPunishSecondUser11makeRequestEjjyb
//   T 0x084422dc _ZN27DB_SaveGiveAvengerTitleFlag11makeRequestEijj
//   T 0x08436ff8 _ZN13DB_HackIPSave11makeRequestEPKcS1_jj
//   T 0x084281ae _ZN21DB_ForceSaveSimpleSSO11makeRequestEji
//   T 0x084283ca _ZN22DB_CountOfHumanCertify11makeRequestEj
// 引用方：W8_CU_G.cpp / CUserOnlyOrig.cpp 局部类声明（签名一致）。
// 包构建约定（与 W7_LinkTailDbs.cpp / DB_U3.cpp 相同，均自 makeRequest 报告逐指令转写）：
//   StreamPool::Acquire(GlobalData::s_stream_pool, "DBThread.cpp", <行号>)
//   → CStreamGuard(stream, true) → guard << opcode [/ << -1] [/ << 参数]
//   [→ guard.GetInBuffer<SIG_*>() 填 payload] → MsgQueueMgr::put(DB_Q=2, guard)。
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
}

// ----------------------------------------------------------------------------
// DB_InsertAutoPunishFirstUser::makeRequest(uint accId, const char* ip,
//     uint a3, uint a4, uint a5, uint a6, ushort a7, uint a8)
// ORIG @ 0x0842dd7c（报告 DB_InsertAutoPunishFirstUser/makeRequest.md）：
// Acquire("DBThread.cpp", 0x723e) → << 0x10a | << -1 →
// GetInBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>()（0x2c 字节，无 memset）填：
//   +0x00 = accId；strncpy(+0x04, ip, 0x10)；+0x14..+0x20 = a3..a6；
#include "W7LinkDB/makeRequest_1.inc"
}

#include "W7LinkDB/makeRequest_2.inc"
}

// ----------------------------------------------------------------------------
// DB_SaveGiveAvengerTitleFlag::makeRequest(int uid, uint accId, uint characNo)
// ORIG @ 0x084422dc（报告 DB_SaveGiveAvengerTitleFlag/makeRequest.md）：
// Acquire("DBThread.cpp", 0xa276) → << 0x28d | << (uint)uid | << accId |
//   << characNo（全部 op<<(unsigned int)，ORIG 080e2292 三连）→ put(DB_Q)。
// 注意：ORIG 此处无 -1 槽。
// ----------------------------------------------------------------------------
#include "W7LinkDB/makeRequest_3.inc"
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
}

// ----------------------------------------------------------------------------
// DB_ForceSaveSimpleSSO::makeRequest(uint accId, int errCnt)
// ORIG @ 0x084281ae（报告 DB_ForceSaveSimpleSSO/makeRequest.md）：
// Acquire("DBThread.cpp", 0x69b1) → << 0xcf | << -1 | << accId(op<<uint) |
//   << errCnt(op<<int) → put(DB_Q)。
// ----------------------------------------------------------------------------
#include "W7LinkDB/makeRequest_5.inc"
}

// ----------------------------------------------------------------------------
// DB_CountOfHumanCertify::makeRequest(uint accId)
// ORIG @ 0x084283ca（报告 DB_CountOfHumanCertify/makeRequest.md）：
// Acquire("DBThread.cpp", 0x69d6) → << 0x111 | << -1 | << accId(op<<uint)
// → put(DB_Q)。
// ----------------------------------------------------------------------------
#include "W7LinkDB/makeRequest_6.inc"
}
