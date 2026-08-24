// df_game_r WongWork::CDeathTowerRanking（死亡之塔排行榜）。
// 全局 3 张排行榜表（ORIG BSS 0x941f8c0 起，每表 0x70f4，getRankTable 懒构造）。
// 布局依据 ctor 0x082a787c / _getRankingTable 0x08469d90 / _getBestRecordTable
// 0x08469dae / stBestRecord_t ctor 0x082a76c0 / getVirtualRank 0x08468de0：
//   +0x0000 塔记录块 m_towers[5]（每塔 0x390 = 0x98 前缀 + 5×0x98 stBestRecord_t 槽，
//            槽 0 = 该塔当前榜首，+0x90 标志位为 checkRenewTopRecord 的“有纪录”位）
//   +0x11d0 hash_map<uint,uint> m_rankingTables[5]（stride 0x14）
//   +0x1234 hash_map<uint,stBestRecord_t> m_bestTables[5]
//   +0x1298 std::map<uint,uint> m_rankMap[5][200]（rank map；[塔][层]，层 stride 0x18，
//            塔 stride 0x12c0，键=通关时间，值=名次；总 0x5dc0 → +0x7058）
//   +0x7058 stBestRecord_t m_tableBest（表级最优纪录；+0x94 即 +0x70ec 为存故名次）
//   +0x70f0 unsigned int m_tableType（init 写入的 type）
//   总尺寸 0x70f4。
#ifndef DNF_CDEATHTOWERRANKING_H
#define DNF_CDEATHTOWERRANKING_H

#include <string.h>
#include <map>
#include <ext/hash_map>

namespace WongWork
{
class CDeathTowerRanking
{
public:
    // 死亡之塔排名层数/塔数（getVirtualRank 层扫描上界 <200，塔 stride = 200*0x18）。
    static const unsigned int kRankStageCount = 200;
    static const unsigned int kRankTowerCount = 5;

    // ORIG 0x082a774c W（_ZN8WongWork18CDeathTowerRanking12getRankTableEj）：
    // 返回 3 张排行榜表（0x941f8c0 起，每表 0x70f4）中 type 对应表指针；
    // type==0 返回首表，否则 0x941f8c0 + (type-1)*0x70f4。
    static void* getRankTable(unsigned int type);
    // ORIG _ZN8WongWork18CDeathTowerRanking4initEj（W）：按 type 初始化排行榜表。
    static void init(unsigned int type);
    // ORIG 0x08469d69 W（_ZN8WongWork18CDeathTowerRanking23makeDungeonIdx2TowerIdxEj）：
    // 死亡之塔副本 idx（0x2af8..0x2afd）→ 塔序号（0..5），其余返回 0xffffffff。
    static unsigned int makeDungeonIdx2TowerIdx(unsigned int dungeonIdx);
    // ORIG 0x084682fe T（_ZN8WongWork18CDeathTowerRanking18checkRenewMyRecordEjjjj，
    // 成员 this=表）：按 towerIdx/key/stage/playTime 查 best 表，返回 -1/0/1。
    int checkRenewMyRecord(unsigned int towerIdx, unsigned int key,
                           unsigned int stage, unsigned int playTime);
    // ORIG 0x08468290 T（_ZN8WongWork18CDeathTowerRanking19checkRenewTopRecordEjjj，
    // 成员 this=表）：返回 1=破纪录，0=未破。
    int checkRenewTopRecord(unsigned int towerIdx, unsigned int a,
                            unsigned int b);
    // ORIG 0x08468de0 T（_ZN8WongWork18CDeathTowerRanking14getVirtualRankEjjj，
    // 成员 this=表）：返回虚拟名次（-1=更好，详见报告）。
    int getVirtualRank(unsigned int towerIdx, unsigned int stage,
                       unsigned int playTime);
    // ORIG 0x08468b4c T（_ZN8WongWork18CDeathTowerRanking18unregistBestRecordEj，
    // 成员 this=表）：对 5 张 best-record 子表按 characNo erase。
    void unregistBestRecord(unsigned int characNo);
    // ORIG 0x08468552 T（_ZN8WongWork18CDeathTowerRanking15unregistRankingEj，
    // 成员 this=表）：对 5 张 ranking 子表按 characNo erase。
    void unregistRanking(unsigned int characNo);

    // ---- stBestRecord_t（尺寸 0x98；ctor 0x082a76c0 / operator> 0x08469ce4 /
    // operator== 0x08469d30）----
    struct stBestRecord_t
    {
        stBestRecord_t()
        {
            m_pad0[0] = m_pad0[1] = m_pad0[2] = m_pad0[3] = 0;
            memset(m_memberData, 0, sizeof(m_memberData));
            memset(m_pad7c, 0, sizeof(m_pad7c));
            m_stage = 0;
            m_playTime = 0;
            m_flag90 = 0;
            m_field94 = -1;
        }
        bool operator>(const stBestRecord_t& other) const
        {
            if (m_stage != other.m_stage)
                return m_stage > other.m_stage;
            return m_playTime < other.m_playTime;
        }
        bool operator==(const stBestRecord_t& other) const
        {
            return m_stage == other.m_stage && m_playTime == other.m_playTime;
        }

        char m_pad0[4];              // +0x00
        char m_memberData[4][30];    // +0x04（ctor 循环清零）
        char m_pad7c[12];            // +0x7c
        unsigned int m_stage;        // +0x88
        unsigned int m_playTime;     // +0x8c
        unsigned char m_flag90;      // +0x90（ctor 置 0；塔榜首槽此位=“有纪录”）
        int m_field94;               // +0x94（ctor 置 -1；表级纪录此位=存故名次）
    };
    typedef __gnu_cxx::hash_map<unsigned int, stBestRecord_t,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<stBestRecord_t> > BestMap;
    // 每层排行榜：键=通关时间，值=该层名次（getVirtualRank 用其插值算虚拟名次）。
    typedef std::map<unsigned int, unsigned int> RankMap;

    // 每塔纪录块（0x390 = 0x98 前缀 + 5 槽；槽 0 为塔当前榜首，见 checkRenewTopRecord）。
    struct stTowerBlock
    {
        char m_pad0[0x98];           // +0x00 前缀
        stBestRecord_t m_slots[5];   // +0x98..+0x390
    };

private:
    stTowerBlock m_towers[kRankTowerCount];               // +0x0000（0x11d0）
    __gnu_cxx::hash_map<unsigned int, unsigned int,
                        __gnu_cxx::hash<unsigned int>,
                        std::equal_to<unsigned int>,
                        std::allocator<unsigned int> >
        m_rankingTables[5];                               // +0x11d0
    __gnu_cxx::hash_map<unsigned int, stBestRecord_t,
                        __gnu_cxx::hash<unsigned int>,
                        std::equal_to<unsigned int>,
                        std::allocator<stBestRecord_t> >
        m_bestTables[5];                                  // +0x1234
    RankMap m_rankMap[kRankTowerCount][kRankStageCount];  // +0x1298（0x5dc0）
    stBestRecord_t m_tableBest;                           // +0x7058（0x98）
    unsigned int m_tableType;                             // +0x70f0
};
}

#endif  // DNF_CDEATHTOWERRANKING_H
