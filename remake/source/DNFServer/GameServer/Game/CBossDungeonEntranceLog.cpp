// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossDungeonEntranceLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CBossDungeonEntranceLog.h"

#include <cstring>

#include "CUser.h"          // CUserCharacInfo get_charac_level/getCurCharacGrowType/get_charac_job
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"

#include "CBossDungeonEntranceLog/IncrementBossStageStart.inc"
                                                      unsigned char a,
                                                      unsigned char b)
{
    // [E04 2026-09-10 解 N]（旧 round2-L30 判词「残余 gap +2 ⇒ N18a/P292 跨版本族
    //   封顶」作废）key 是「32 位容器 + 4 位位域」双视角：byte0..2 为普通 uchar 成员、
    //   byte3 为两个 4 位位域 —— 位域赋值自身产生 ORIG 的两处
    //   `and $0xf,%eax; mov %eax,%edx; and $0xf,%edx`（值截断 + 赋值截断）；
    //   union 版使 `k.key = 0` 复现单条 `movl $0x0,-0x40(%ebp)`（`*(unsigned int*)&k`
    //   形会多发 lea+mov）。探针 /tmp/qts/cb.cpp：93 = 93 逐条全等。
    union KeyBits {
        unsigned int key;
        struct {
            unsigned char m_field0;
            unsigned char m_level;
            unsigned char m_growType;
            unsigned char m_job : 4;
            unsigned char m_channel : 4;
        };
    };
    KeyBits k;
    k.key = 0;                                                // ORIG movl $0x0,-0x40
    k.m_field0 = a;                                           // ORIG movzbl -0x5c → -0x40
    k.m_level = (unsigned char)user.get_charac_level();       // → -0x3f
    k.m_growType = (unsigned char)user.getCurCharacGrowType();// → -0x3e
    k.m_job = (unsigned char)user.get_charac_job();
    k.m_channel = b;

    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.find(k.key);
    // ORIG 布局：未命中块 fall-through、命中块在尾部（`call eq; test %al; je <命中>`）
    //   ⇒ 条件取反写 `if (it == end) { insert } else { ++ }`。
    if (it == m_logMap.end())
    {
        STBossDungeonTowerLog entry;           // ORIG memset(local,0,0x10)
        std::memset(&entry, 0, sizeof(entry));
        entry.stageStartCount = 1;             // ORIG *(int*)local = 1
        m_logMap.insert(std::make_pair(k.key, entry));
    }
    else
    {
        it->second.stageStartCount += 1;      // ORIG ++*(int*)(pair+4)
    }
}

#include "CBossDungeonEntranceLog/CBossDungeonEntranceLog.inc"


#include "CBossDungeonEntranceLog/dtor_CBossDungeonEntranceLog.inc"


#include "CBossDungeonEntranceLog/reset.inc"


#include "CBossDungeonEntranceLog/IncrementBossStageEnd.inc"


#include "CBossDungeonEntranceLog/IncrementBossPickUpItem.inc"
                                                      unsigned char a,
                                                      unsigned char b)
{
    // [round2-L30] 修法（ORIG 0x081564f0/0x0815662e/0x0815678e）：
    //   ①`|` 两侧书写序交换（先 `(job & 0xf)` / `((b & 0xf) << 4)`）⇒ 复现 ORIG
    //     `call; and $0xf,%eax; mov %eax,%edx; and $0xf,%edx` 的右操作数先求值序；
    //   ②删 `unsigned int key = k.key;` 具名局部，find/insert 直接用 `k.key`
    //     （P0/P183：ORIG 无 key 槽，直传 &k）。
    // [E04 2026-09-10 解 N] key = 「32 位容器 + 4 位位域」双视角（同 IncrementBossStageStart：
    //   位域赋值自身产生 ORIG 的两处 `and $0xf,%eax`；union 使 `k.key = 0` 单条 disp 写）。
    union KeyBits {
        unsigned int key;
        struct {
            unsigned char m_field0;
            unsigned char m_level;
            unsigned char m_growType;
            unsigned char m_job : 4;
            unsigned char m_channel : 4;
        };
    };
    KeyBits k;
    k.key = 0;                                                // ORIG movl $0x0,-0x40
    k.m_field0 = a;                                           // ORIG movzbl -0x5c → -0x40
    k.m_level = (unsigned char)user.get_charac_level();       // → -0x3f
    k.m_growType = (unsigned char)user.getCurCharacGrowType();// → -0x3e
    k.m_job = (unsigned char)user.get_charac_job();
    k.m_channel = b;

    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.find(k.key);
    // ORIG 布局：未命中块 fall-through、命中块在尾部（`call eq; test %al; je <命中>`）
    if (it == m_logMap.end())
    {
        STBossDungeonTowerLog entry;
        std::memset(&entry, 0, sizeof(entry));
        entry.pickUpItemCount = 1;             // ORIG -0x4c = 1
        m_logMap.insert(std::make_pair(k.key, entry));
    }
    else
    {
        it->second.pickUpItemCount += 1;       // ORIG ++*(int*)(pair+0xc)
    }
}

#include "CBossDungeonEntranceLog/BossTowerLogDBUpdate.inc"

