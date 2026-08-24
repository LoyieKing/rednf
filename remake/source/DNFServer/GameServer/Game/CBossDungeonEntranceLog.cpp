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

// ORIG 0x081564f0：命中 → ++stageStartCount(+0)；未命中 → memset0 后 insert
// {stageStartCount=1, 其余 0}。
void CBossDungeonEntranceLog::IncrementBossStageStart(CUser& user,
                                                      unsigned char a,
                                                      unsigned char b)
{
    // ---- key 打包（ORIG +0x009..+0x070：逐字节写 -0x40..-0x3d）----
    union KeyBytes {
        unsigned int key;
        unsigned char byte[4];
    };
    KeyBytes k;
    k.key = 0;                                       // ORIG movl $0x0,-0x40
    k.byte[0] = a;                                   // ORIG movzbl -0x5c → -0x40
    k.byte[1] = (unsigned char)user.get_charac_level();        // → -0x3f
    k.byte[2] = (unsigned char)user.getCurCharacGrowType();    // → -0x3e
    // ORIG：byte3 = (byte3 & ~0xf) | (job & 0xf)；再 | ((b & 0xf) << 4)
    k.byte[3] = (unsigned char)((k.byte[3] & 0xf0) |
                                ((unsigned int)user.get_charac_job() & 0xf));
    k.byte[3] = (unsigned char)(((unsigned int)k.byte[3] & 0xf) | (((unsigned int)b & 0xf) << 4));
    unsigned int key = k.key;

    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.find(key);
    if (it != m_logMap.end())
    {
        it->second.stageStartCount += 1;      // ORIG ++*(int*)(pair+4)
    }
    else
    {
        STBossDungeonTowerLog entry;           // ORIG memset(local,0,0x10)
        std::memset(&entry, 0, sizeof(entry));
        entry.stageStartCount = 1;             // ORIG *(int*)local = 1
        m_logMap.insert(
            std::make_pair(key, entry));       // ORIG make_pair<uint&,ST&> + insert(RKS5_)
    }
}

// ORIG 0x0815662e：命中 → ++stageEndCount(+4)、playTimeSum(+0xc) += playTimeSec；
// 未命中 → insert {stageStartCount=1, stageEndCount=1, playTimeSum=playTimeSec}。
void CBossDungeonEntranceLog::IncrementBossStageEnd(CUser& user,
                                                    unsigned char a,
                                                    unsigned int playTimeSec,
                                                    unsigned char b)
{
    // ---- key 打包（ORIG +0x009..+0x070：逐字节写 -0x40..-0x3d）----
    union KeyBytes {
        unsigned int key;
        unsigned char byte[4];
    };
    KeyBytes k;
    k.key = 0;                                       // ORIG movl $0x0,-0x40
    k.byte[0] = a;                                   // ORIG movzbl -0x5c → -0x40
    k.byte[1] = (unsigned char)user.get_charac_level();        // → -0x3f
    k.byte[2] = (unsigned char)user.getCurCharacGrowType();    // → -0x3e
    // ORIG：byte3 = (byte3 & ~0xf) | (job & 0xf)；再 | ((b & 0xf) << 4)
    k.byte[3] = (unsigned char)((k.byte[3] & 0xf0) |
                                ((unsigned int)user.get_charac_job() & 0xf));
    k.byte[3] = (unsigned char)(((unsigned int)k.byte[3] & 0xf) |
                                (((unsigned int)b & 0xf) << 4));
    unsigned int key = k.key;

    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.find(key);
    if (it != m_logMap.end())
    {
        it->second.stageEndCount += 1;         // ORIG ++*(int*)(pair+8)
        it->second.playTimeSum += playTimeSec; // ORIG *(uint*)(pair+0x10) += param_3
    }
    else
    {
        STBossDungeonTowerLog entry;
        std::memset(&entry, 0, sizeof(entry));
        entry.stageStartCount = 1;             // ORIG -0x54 = 1
        entry.stageEndCount = 1;               // ORIG -0x50 = 1
        entry.playTimeSum = playTimeSec;       // ORIG -0x48 = param_3
        m_logMap.insert(std::make_pair(key, entry));
    }
}

// ORIG 0x0815678e：命中 → ++pickUpItemCount(+8)；未命中 → insert {pickUpItemCount=1}。
void CBossDungeonEntranceLog::IncrementBossPickUpItem(CUser& user,
                                                      unsigned char a,
                                                      unsigned char b)
{
    // ---- key 打包（ORIG +0x009..+0x070：逐字节写 -0x40..-0x3d）----
    union KeyBytes {
        unsigned int key;
        unsigned char byte[4];
    };
    KeyBytes k;
    k.key = 0;                                       // ORIG movl $0x0,-0x40
    k.byte[0] = a;                                   // ORIG movzbl -0x5c → -0x40
    k.byte[1] = (unsigned char)user.get_charac_level();        // → -0x3f
    k.byte[2] = (unsigned char)user.getCurCharacGrowType();    // → -0x3e
    // ORIG：byte3 = (byte3 & ~0xf) | (job & 0xf)；再 | ((b & 0xf) << 4)
    k.byte[3] = (unsigned char)((k.byte[3] & 0xf0) |
                                ((unsigned int)user.get_charac_job() & 0xf));
    k.byte[3] = (unsigned char)(((unsigned int)k.byte[3] & 0xf) |
                                (((unsigned int)b & 0xf) << 4));
    unsigned int key = k.key;

    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.find(key);
    if (it != m_logMap.end())
    {
        it->second.pickUpItemCount += 1;       // ORIG ++*(int*)(pair+0xc)
    }
    else
    {
        STBossDungeonTowerLog entry;
        std::memset(&entry, 0, sizeof(entry));
        entry.pickUpItemCount = 1;             // ORIG -0x4c = 1
        m_logMap.insert(std::make_pair(key, entry));
    }
}

// ORIG 0x081568cc：size>0 时打包 header(0x2e0,-1,size) + 逐项(key + 0x10 字节
// value)，随后 clear 并投递 DB 队列（QUEUE_IDX 4）。
void CBossDungeonEntranceLog::BossTowerLogDBUpdate()
{
    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.begin();

    Stream* stream =
        GlobalData::s_stream_pool->Acquire("localglobal/global_Statistics.cpp", 0x69);
    CStreamGuard guard(stream, true);          // ORIG CStreamGuard(pSVar2, 1)

    int count = (int)m_logMap.size();          // ORIG map::size
    if (count > 0)
    {
        guard << 0x2e0;                        // DB 请求号（ORIG 经 operator* 恒等转发）
        guard << (int)-1;
        guard << count;

        for (; it != m_logMap.end(); ++it)
        {
            guard << (int)(*it).first;         // ORIG operator<<(key)，int 版 0x080c8c56
            STBossDungeonTowerLog* out =
                guard.GetInBuffer<STBossDungeonTowerLog>();
            std::memcpy(out, &(*it).second, 0x10);
            std::memset(&(*it).second, 0, 0x10);
        }

        m_logMap.clear();                      // ORIG map::clear

        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
    }
}
