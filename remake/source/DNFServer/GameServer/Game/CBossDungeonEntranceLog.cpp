// ============================================================================
// df_game_r 还原 —— CBossDungeonEntranceLog（BossTower 入场统计日志）
// 逐函数依据 docs/class_func_reports/CBossDungeonEntranceLog/ 各报告汇编转写；
// ctor/dtor/reset 依据 ORIG nm（W 符号）与 objdump 0x081456c2/0x08145704/0x08145746。
//
// 三个 Increment* 的 key 打包（ORIG -0x40 局部零初始化 → byte0=a →
// get_charac_level→byte1 → getCurCharacGrowType→byte2 →
// (get_charac_job&0xf)|((b&0xf)<<4)→byte3）在各函数内联展开，语句顺序与
// ORIG 汇编一致，不引入 ORIG 不存在的局部辅助函数。
// ============================================================================
#include "CBossDungeonEntranceLog.h"

#include <cstring>

#include "CUser.h"          // CUserCharacInfo get_charac_level/getCurCharacGrowType/get_charac_job
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"

// ORIG 0x081564f0：命中 → ++stageStartCount(+0)；未命中 → memset0 后 insert
// {stageStartCount=1, 其余 0}。
void CBossDungeonEntranceLog::IncrementBossStageStart(CUser& user,
                                                      unsigned char a,
                                                      unsigned char b)
{
    // [round2-L30] 修法（ORIG 0x081564f0/0x0815662e/0x0815678e）：
    //   ①`|` 两侧书写序交换（先 `(job & 0xf)` / `((b & 0xf) << 4)`）⇒ 复现 ORIG
    //     `call; and $0xf,%eax; mov %eax,%edx; and $0xf,%edx` 的右操作数先求值序；
    //   ②删 `unsigned int key = k.key;` 具名局部，find/insert 直接用 `k.key`
    //     （P0/P183：ORIG 无 key 槽，直传 &k）。
    // 残余（gap +2）：ORIG 两处 4.1 冗余 `and $0xf,%eax`（call 结果先窄化再中转，
    //   4.4 恒 1 条）⇒ N18a/P292 跨版本族封顶。
    // [E04 2026-09-10 解 N] key 是「32 位容器 + 4 位位域」双视角（ORIG 两处
    //   `and $0xf,%eax` 冗余截断 = 位域赋值的值截断 + 赋值截断；byte0..2 为普通
    //   uchar 成员），union 版使 `k.key = 0` 复现单条 `movl $0x0,-0x40(%ebp)`。
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

CBossDungeonEntranceLog::CBossDungeonEntranceLog()
{
    // ORIG 0x081456c2：map C1 由编译器生成，函数体仅调 reset()。
    reset();
}

CBossDungeonEntranceLog::~CBossDungeonEntranceLog()
{
    // ORIG 0x08145704：reset() 后 map D1（编译器生成）。
    reset();
}

void CBossDungeonEntranceLog::reset()
{
    // ORIG 0x08145746：tail-call map::clear。
    m_logMap.clear();
}

// ORIG 0x0815662e：命中 → ++stageEndCount(+4)、playTimeSum(+0xc) += playTimeSec；
// 未命中 → insert {stageStartCount=1, stageEndCount=1, playTimeSum=playTimeSec}。
void CBossDungeonEntranceLog::IncrementBossStageEnd(CUser& user,
                                                    unsigned char a,
                                                    unsigned int playTimeSec,
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
        entry.stageStartCount = 1;             // ORIG -0x54 = 1
        entry.stageEndCount = 1;               // ORIG -0x50 = 1
        entry.playTimeSum = playTimeSec;       // ORIG -0x48 = param_3
        m_logMap.insert(std::make_pair(k.key, entry));
    }
    else
    {
        it->second.stageEndCount += 1;         // ORIG ++*(int*)(pair+8)
        it->second.playTimeSum += playTimeSec; // ORIG *(uint*)(pair+0x10) += param_3
    }
}

// ORIG 0x0815678e：命中 → ++pickUpItemCount(+8)；未命中 → insert {pickUpItemCount=1}。
void CBossDungeonEntranceLog::IncrementBossPickUpItem(CUser& user,
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

// ORIG 0x081568cc：size>0 时打包 header(0x2e0,-1,size) + 逐项(key + 0x10 字节
// value)，随后 clear 并投递 DB 队列（QUEUE_IDX 4）。
void CBossDungeonEntranceLog::BossTowerLogDBUpdate()
{
    // [round2-L30] 修法四则（ORIG 0x081568cc size 0x973）：
    //   ①函数头 begin+end 双求值（P13/P214④，ORIG 0x081568d4/0x081568e9）；
    //   ②删 `Stream* stream` 具名局部（Acquire 结果直传 ctor，P0/配方③）；
    //   ③3 处 header 写 + 1 处 key 写补 `guard.operator*()->operator<<(x)`（P132）；
    //   ④循环体 `it->first`（ptEv）+ 具名 key + `guard.operator->()->GetInBuffer`
    //      （P103/P0）。
    // 结果 116→126（orig 127，gap 11→1）；残余 = 整帧槽号常数平移 4（ORIG it -0x14 /
    //   guard -0x20 / count -0x10 vs 我方 -0x18/-0x24/-0x14，相对间距全等）⇒ 判据⑪
    //   (b) 类 ⇒ **N126（版本首槽基址常数）**，声明重排无效。
    // ORIG 0x081568d4：头部先 `call begin`（-0x14）再 `call end`（-0x18）——两个
    //   迭代器都在函数头求值（P13/P214④）。
    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.begin();
    std::map<unsigned int, STBossDungeonTowerLog>::iterator itEnd = m_logMap.end();

    // ORIG Acquire 结果**无槽**（0x0815690b `mov %eax,0x4(%esp)` 直传 CStreamGuard
    //   C1）⇒ 删我方 `Stream* stream` 具名局部（P0/配方③）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localglobal/global_Statistics.cpp", 0x69),
        true);                                  // ORIG CStreamGuard(pSVar2, 1)

    int count = (int)m_logMap.size();          // ORIG map::size
    if (count > 0)
    {
        // P132：ORIG 0x0815694a..0x0815697x 每处 header 写入都是
        //   `lea -0x20(%ebp); call CStreamGuard::operator*(); …; call operator<<`
        //   ⇒ 源形必须保留 `guard.operator*()->operator<<(x)` 两层调用（直接
        //   `guard << x` 会省略 ORIG 的 de() 调用）。
        guard.operator*()->operator<<(0x2e0);  // DB 请求号
        guard.operator*()->operator<<((int)-1);
        guard.operator*()->operator<<(count);

        for (; it != itEnd; ++it)     // ORIG 用函数头缓存的 end（0x0815693d 段比较 -0x18）
        {
            // ORIG 0x0815699f..0x081569c6：`call _Rb_tree_iterator::ptEv()`（即
            //   `it->first`）→ 结果具名落槽 -0x24（P0：ORIG 有槽往返 ⇒ 原码有具名
            //   key）→ `%ebx` 跨 `CStreamGuard::de()` 保活 → operator<<(int)。
            unsigned int key = it->first;
            guard.operator*()->operator<<((int)key);
            // P103：ORIG 0x081569cb 先 `call CStreamGuard::ptEv()` 再调
            //   GetInBuffer ⇒ 源形必须保留 operator-> 两层链。
            STBossDungeonTowerLog* out =
                guard.operator->()->GetInBuffer<STBossDungeonTowerLog>();
            std::memcpy(out, &(*it).second, 0x10);
            std::memset(&(*it).second, 0, 0x10);
        }

        m_logMap.clear();                      // ORIG map::clear

        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
    }
}
