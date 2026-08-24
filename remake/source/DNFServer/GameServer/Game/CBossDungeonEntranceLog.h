// ============================================================================
// CBossDungeonEntranceLog.h — 全局类（无命名空间），BossTower 入场统计日志
//
// ORIG 证据（df_game_r, elf32-i386）：
//   - 对象 0x18 字节，唯一成员 std::map<unsigned int, STBossDungeonTowerLog,
//     less<unsigned int>, allocator<pair<const unsigned int,
//     STBossDungeonTowerLog> > >（ORIG map::clear @0x081459a6 mangled
//     _ZNSt3mapIj21STBossDungeonTowerLogSt4lessIjESaISt4pairIKjS0_EEE5clearEv；
//     目标工具链 std::map 为 24B，见 CCreatureScriptMgr.h 同口径）。
//   - ctor 0x081456c2（W）：map C1 + reset()。
//   - dtor 0x08145704（W）：reset() + map D1。
//   - reset 0x08145746（W）：map::clear()。
//   - IncrementBossStageStart 0x081564f0（T）：
//     key = (uchar)a
//         | get_charac_level()<<8
//         | getCurCharacGrowType()<<16
//         | ((get_charac_job()&0xf) | ((uchar)b&0xf)<<4)<<24；
//     命中 → ++stageStartCount；未命中 → insert {start=1}。
//   - IncrementBossPickUpItem 0x0815678e（T）：同 key；
//     命中 → ++pickUpItemCount；未命中 → insert {pickup=1}。
//   - IncrementBossStageEnd 0x0815662e（T）：同 key；
//     命中 → ++stageEndCount、playTimeSec 累加至 +0x0c；
//     未命中 → insert {start=1, end=1, playTimeSec}。
//   - BossTowerLogDBUpdate 0x081568cc（T）：size>0 时
//     StreamPool::Acquire(s_stream_pool,"localglobal/global_Statistics.cpp",0x69)
//     → CStreamGuard(stream,true) → <<0x2e0、<<-1、<<size →
//     逐项 <<key、GetInBuffer<STBossDungeonTowerLog> 写入 0x10 字节、
//     清零该项 → map::clear → MsgQueueMgr::put(s_msgq_mgr, 4, guard)。
//     （DB 请求号 0x2e0，队列 QUEUE_IDX 4。）
// ============================================================================
#ifndef DNF_GAME_CBOSSDUNGEONENTRANCELOG_H_
#define DNF_GAME_CBOSSDUNGEONENTRANCELOG_H_

#include <map>

class CUser;

// ORIG POD（无 ctor/dtor 符号；BossTowerLogDBUpdate 按 0x10 字节整块 memcpy）。
struct STBossDungeonTowerLog
{
    unsigned int stageStartCount;   // +0x00（StageStart 命中 ++ / 首次 insert =1）
    unsigned int stageEndCount;     // +0x04（StageEnd 命中 ++ / 首次 insert =1）
    unsigned int pickUpItemCount;   // +0x08（PickUpItem 命中 ++ / 首次 insert =1）
    unsigned int playTimeSum;       // +0x0c（StageEnd 累加 playTimeSec）
};

class CBossDungeonEntranceLog
{
public:
    CBossDungeonEntranceLog();   // ORIG 0x081456c2（W）
    ~CBossDungeonEntranceLog();  // ORIG 0x08145704（W）

    void reset();                // ORIG 0x08145746（W）：map::clear

    void IncrementBossStageStart(CUser& user, unsigned char a, unsigned char b);   // 0x081564f0
    void IncrementBossStageEnd(CUser& user, unsigned char a,
                               unsigned int playTimeSec, unsigned char b);         // 0x0815662e
    void IncrementBossPickUpItem(CUser& user, unsigned char a, unsigned char b);   // 0x0815678e
    void BossTowerLogDBUpdate();   // 0x081568cc

private:
    std::map<unsigned int, STBossDungeonTowerLog> m_logMap;   // +0x00（0x18）
};

#endif  // DNF_GAME_CBOSSDUNGEONENTRANCELOG_H_
