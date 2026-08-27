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

class PacketGuard;   // makeMyBestRecordPacket/makeTopRankerRecordPacket 参数（真实定义
                     //   在 PacketGuard.h，class；此处必须同为 class 以免 tag 冲突）

// W6_RankMailDeps 需要（registRanker/registBestRecord 参数；真实布局仅按
// ORIG 字节序引用，见 W6_RankMailDeps.cpp 注释）：
struct stDeathTowerRecordMemberInfo_t;

namespace WongWork
{

class CDeathTowerRanking
{
public:
    // 死亡之塔排名层数/塔数（getVirtualRank 层扫描上界 <200，塔 stride = 200*0x18）。
    static const unsigned int kRankStageCount = 200;
    static const unsigned int kRankTowerCount = 5;

    typedef __gnu_cxx::hash_map<unsigned int, unsigned int,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<unsigned int> > RankTable;

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
    // ORIG 0x08468b8e T（_ZN8WongWork18CDeathTowerRanking22makeMyBestRecordPacketEjjR11PacketGuard）：
    // 把 key 对应个人最优记录序列化进 packet。
    void makeMyBestRecordPacket(unsigned int towerIdx, unsigned int key,
                                PacketGuard& packet);
    // ORIG 0x08468c68 T（_ZN8WongWork18CDeathTowerRanking25makeTopRankerRecordPacketEjR11PacketGuard）：
    // 把该塔榜首记录序列化进 packet。
    void makeTopRankerRecordPacket(unsigned int towerIdx, PacketGuard& packet);

    // ---- W6_RankMailDeps 补齐（2026-08-27，报告 WongWork__CDeathTowerRanking/*）----
    // ORIG T 0x08468614（_ZN8WongWork18CDeathTowerRanking12registRankerEjjPK30stDeathTowerRecordMemberInfo_tjj）：
    // 塔记录槽登记（members 为成员信息数组，stride 0x17）。
    void registRanker(unsigned int towerIdx, unsigned int slotIdx,
                      const stDeathTowerRecordMemberInfo_t* members,
                      unsigned int stageVal, unsigned int playTime);
    // ORIG T 0x08468594：清空全部排行榜/best/层 rank map。
    void resetRanking();
    // ORIG T 0x084683da：key→rank 插入（已存在则改写），并同步 best 记录存故名次。
    void registRanking(unsigned int tableIdx, unsigned int key, unsigned int rank);
    // ORIG T 0x084684ce：仅排名表部分。
    void updateRanking(unsigned int tableIdx, unsigned int key, unsigned int rank);
    // ORIG T 0x08468914：个人最优纪录登记（record +0 存 key）。
    void registBestRecord(unsigned int towerIdx, unsigned int key,
                          const stDeathTowerRecordMemberInfo_t* members,
                          unsigned int stageVal, unsigned int playTime);

    // ---- stBestRecord_t（尺寸 0x98；ctor 0x082a76c0 / operator> 0x08469ce4 /
    // operator== 0x08469d30；ORIG 两运算符均为非 const 成员——已按 nm 对齐）----
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
        bool operator>(stBestRecord_t& other)
        {
            if (m_stage != other.m_stage)
                return m_stage > other.m_stage;
            return m_playTime < other.m_playTime;
        }
        bool operator==(stBestRecord_t& other)
        {
            return m_stage == other.m_stage && m_playTime == other.m_playTime;
        }

        char m_pad0[4];              // +0x00（registBestRecord 用作 key dword）
        char m_memberData[4][30];    // +0x04（ctor 循环清零）
        char m_pad7c[12];            // +0x7c（+0x7c/80/84 三条 byte lane 起点）
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

    // ORIG W 0x08469d90 / 0x08469dae：表内子表寻址（stride 0x14，具名数组直取）。
    RankTable* _getRankingTable(unsigned int idx);
    BestMap*   _getBestRecordTable(unsigned int idx);

    // 每塔纪录块（ORIG 线性寻址见 W6_RankMailDeps.cpp；槽 0 为塔当前榜首，
    // 见 checkRenewTopRecord）。
    struct stTowerBlock
    {
        char m_pad0[0x98];           // +0x00 前缀
        stBestRecord_t m_slots[5];   // +0x98..+0x390
    };
    // ORIG T 0x08468cb6（_ZN8WongWork18CDeathTowerRanking17_makeRecordPacketERKNS0_14stBestRecord_tER11PacketGuard，
    // 定义 CompoundDeathLCB.cpp）：单条纪录序列化——4 成员名 strlen+put_str、
    // 各 put_byte(+0x80/+0x84 槽内 flag 对)，末尾 stage(short)/playTime(int)/名次(int)。
    void _makeRecordPacket(const stBestRecord_t& record, PacketGuard& packet);

private:
    stTowerBlock m_towers[kRankTowerCount];               // +0x0000（0x11d0）
    RankTable m_rankingTables[5];                         // +0x11d0
    BestMap   m_bestTables[5];                            // +0x1234
    RankMap m_rankMap[kRankTowerCount][kRankStageCount];  // +0x1298（0x5dc0）
    stBestRecord_t m_tableBest;                           // +0x7058（0x98）
    unsigned int m_tableType;                             // +0x70f0
};
}

#endif  // DNF_CDEATHTOWERRANKING_H
