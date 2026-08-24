// ============================================================================
// CBossStage.h — WongWork::CBossStage（CBossPlay 派生）
//
// ORIG 证据（df_game_r, elf32-i386）：
//   - CBossStage : CBossPlay（0x9e8 基类）；CBossStage 自身尺寸 0xaf0。
//   - Ctor（0x0814bd0c）：调 CBossPlay ctor、置 CBossStage vtable、构造
//     MapInfo@+0x9f4、Clear、get_rand_int(60000) 写 +0xa30/+0xa34，+0xae0=1、
//     +0x9f0=0、+0xae8=0。
//   - Dtor（0x0814bdd6）：置 vtable、Clear、rand、析构 MapInfo@+0x9f4、CBossPlay dtor。
//   - reset（0x0814bea2）：+0x9e8/+0x9ec=0、MapInfo::Clear@+0x9f4、memset +0x9c8,0x10。
//   - 虚表：CBossPlay 6 虚槽 + 新增 4 虚（handleMoveMap/handlePickupItem(char&)/
//     handleDropItem/handleKillMonster）。
//   - 后段（+0x9e8..+0xaf0）字段无具名成员（MapInfo 等），保留 m_pad 地址形式，
//     ctor/dtor/reset 在 CBossStage.cpp 按 ORIG 偏移访问。
//
// 由 FixCBossPlay 建立；CBossTower : CBossStage。
// ============================================================================
#ifndef DNF_CBOSSSTAGE_H
#define DNF_CBOSSSTAGE_H

#include "CBossPlay.h"
#include "WarField.h"   // MapInfo 完整类型（m_stageData 成员）

class CUser;
struct map_item;
struct map_monster;
struct MSG_MONSTER_DIE;
class CMap;

namespace WongWork
{

class CBossStage : public CBossPlay
{
public:
    CBossStage();               // ORIG 0x0814bd0c
    ~CBossStage();              // ORIG 0x0814bdd6
    void reset();               // ORIG 0x0814bea2

    // ---- 新增虚函数（虚表槽，紧随 CBossPlay 的 6 个虚函数之后）----
    virtual int handleMoveMap(unsigned short* mapIndex);   // 0x0814f66a
    virtual int handlePickupItem(CUser* user, int a, bool b, char& c);  // 0x0814fa76
    virtual int handleDropItem(CUser* user, int a, int b, char c, int d, int e);  // 0x0814ff4a
    virtual int handleKillMonster(CUser* user, unsigned short a, unsigned short b,
                                  const MSG_MONSTER_DIE& die, int* d);  // 0x08150388

    // ---- 其余非虚方法（实现见 CBossStage.cpp，逐函数 ORIG 报告转写）----
    bool GoldPickupItem(CUser* user, unsigned int a, const map_item& item);  // 0x0814f0e6
    int KillMonsterGetExp(const map_monster& mob, unsigned int& exp);        // 0x0814d300
    bool NormalPickupItem(CUser* user, unsigned int a, const map_item& item); // 0x0814ecec
    bool RandomConsistMap(CUser& user);                                       // 0x0814f426（ORIG eax 流出）
    bool randomMapIndex(int& mapIdx);                                         // 0x0814f4fc
    bool RandomPickupItem(CUser* user, unsigned int a, const map_item& item); // 0x0814e684
    bool RoutingPickupItem(CUser* user, unsigned int a, map_item& item);      // 0x0814e57a
    int SendMapInfoPacket();                                                  // 0x0814ce12
    bool consistMap(const CMap* map);                                         // 0x0814bef4
    bool dropItem(CUser* user, char a, int b, int c, map_item& item);         // 0x0814de1a
    bool killMonsterDropItem(const map_monster& mob, int a,
                             unsigned short b, unsigned short c);             // 0x0814d628
    bool killMonsterErase(unsigned short a, map_monster& mob, int& b);        // 0x0814d214
    bool moveNextStage();                                                     // 0x0814d1b6
private:
    // ---- 后段具名成员（ORIG 偏移实证，总尺寸 0xaf0）----
    int m_startTick;           // +0x9e8（handleMoveMap 计时起点，reset 清零）
    int m_endTick;             // +0x9ec（handleMoveMap 计时终点【推断：通关时刻】）
    CDungeon* m_dungeon;       // +0x9f0（consistMap/handleMoveMap/掉落域）
    MapInfo m_stageData;       // +0x9f4（0xec；ctor 显式构造+Clear）
    int m_stageCur;            // +0xae0（当前层，ctor=1）
    int m_stageMax;            // +0xae4
    int m_difficulty;          // +0xae8
    int m_expParamA;           // +0xaec（KillMonsterGetExp 的费率表索引参数）

    // 序号/seed 复用 MapInfo 既有字段（ORIG 同址访问）：
    //   m_stageData.m_assignItemIdx(+0x3c)=怪物序号、m_dropItemCnt(+0x40)=物品
    //   序号、m_mapBasisLevel(+0x48)=seed 值。
};

// sizeof 断言（C++98 形式）：CBossStage ORIG 尺寸 0xaf0
typedef char assert_sizeof_cbossstage[
    (sizeof(WongWork::CBossStage) == 0xaf0) ? 1 : -1];

}  // namespace WongWork

#endif  // DNF_CBOSSSTAGE_H