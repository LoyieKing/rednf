// df_game_r WongWork::CDeathTowerRanking（死亡之塔排行榜）。
// 全局 3 张排行榜表（ORIG BSS 0x941f8c0 起，每表 0x70f4，getRankTable 懒构造）。
// 本头只声明本批次 asm 桥清理所需方法（ORIG 符号地址见各注释）；完整布局与
// 其余方法由 CDeathTower 域后续批次补齐（不写猜测）。
#ifndef DNF_CDEATHTOWERRANKING_H
#define DNF_CDEATHTOWERRANKING_H

namespace WongWork
{
class CDeathTowerRanking
{
public:
    // ORIG 0x082a774c W（_ZN8WongWork18CDeathTowerRanking12getRankTableEj）：
    // 返回 3 张排行榜表（0x941f8c0 起，每表 0x70f4）中 type 对应表指针；
    // type==0 返回首表，否则 0x941f8c0 + (type-1)*0x70f4。
    static void* getRankTable(unsigned int type);
    // ORIG 0x08469d69 W（_ZN8WongWork18CDeathTowerRanking23makeDungeonIdx2TowerIdxEj）：
    // 死亡之塔副本 idx（0x2af8..0x2afd）→ 塔序号（0..5），其余返回 0xffffffff。
    static unsigned int makeDungeonIdx2TowerIdx(unsigned int dungeonIdx);
    // ORIG 0x084682fe T（_ZN8WongWork18CDeathTowerRanking18checkRenewMyRecordEjjjj，
    // 成员 this=表）：按 towerIdx/a/b/c 刷新个人纪录。
    void checkRenewMyRecord(unsigned int towerIdx, unsigned int a,
                            unsigned int b, unsigned int c);
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
};
}

#endif  // DNF_CDEATHTOWERRANKING_H
