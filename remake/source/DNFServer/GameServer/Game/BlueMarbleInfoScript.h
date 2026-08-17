#ifndef GAME_BLUEMARBLE_INFOSCRIPT_H_
#define GAME_BLUEMARBLE_INFOSCRIPT_H_

// ============================================================================
// df_game_r BlueMarbleInfoScript（G2-4 大富翁配置脚本，尺寸 0x8c）
// 布局依据 ORIG 构造（0x088d5690）/clear（0x088d57c0）/ScriptManager +0x30 偏移：
//   +0x00 int m_playerMax          +0x04 int m_enterLevel
//   +0x08 int m_enterMaxLevel      +0x0c int m_enterCount
//   +0x10 int m_dieCount           +0x14 int m_diceMin
//   +0x18 int m_diceMax            +0x1c int m_itemWeightTotal
//   +0x20 int m_dungeonDifficulty  +0x24 vector<BlueMarbleDungeonDifficulty>
//   +0x30 int m_superArmorTime     +0x34 int m_buffMaxValue
//   +0x38 float m_goldBase         +0x3c float m_goldMulti
//   +0x40 int m_equipmentUpgradeMax
//   +0x44/+0x50/+0x5c vector<BlueMarbleRandomDungeonInfo>
//   +0x68 vector<BlueMarbleRandomItemInfo>  +0x74 vector<BlueMarbleRewardInfo>
//   +0x80 vector<BlueMarbleTilePos>（总尺寸 0x8c）
// getRandomDungeonIndex/getBossDungeonIndex/getUniqueDungeonIndex 按 ORIG ABI
// 返回 std::vector<int>（sret 在 this 之前，ret $0x4，mangled 名 Ei）。
// ============================================================================

#include <vector>

// ---- 脚本数据结构（字段顺序来自 importScript 的 ScanInt 序列） ----
struct BlueMarbleRandomDungeonInfo
{
    int m_minLevel;     // +0x00
    int m_maxLevel;     // +0x04
    int m_dungeonIndex; // +0x08
};

struct BlueMarbleRandomItemInfo
{
    int m_itemIndex;  // +0x00
    int m_weight;     // +0x04
};

struct BlueMarbleRewardInfo
{
    int m_a;  // +0x00
    int m_b;  // +0x04
};

struct BlueMarbleTilePos
{
    int m_a;  // +0x00
    int m_b;  // +0x04
};

struct BlueMarbleDungeonDifficulty
{
    int m_dungeon;     // +0x00
    int m_difficulty;  // +0x04
};

class BlueMarbleInfoScript
{
public:
    BlueMarbleInfoScript();
    ~BlueMarbleInfoScript();

    void clear();
    int importScript(const char* dir, const char* path);

    std::vector<int> getRandomDungeonIndex(int level);
    std::vector<int> getBossDungeonIndex(int level);
    std::vector<int> getUniqueDungeonIndex(int level);
    int getDungeonDifficulty(int dungeon);

private:
    int m_playerMax;              // +0x00
    int m_enterLevel;             // +0x04
    int m_enterMaxLevel;          // +0x08
    int m_enterCount;             // +0x0c
    int m_dieCount;               // +0x10
    int m_diceMin;                // +0x14
    int m_diceMax;                // +0x18
    int m_itemWeightTotal;        // +0x1c
    int m_dungeonDifficulty;      // +0x20
    std::vector<BlueMarbleDungeonDifficulty> m_dungeonDifficultyList;  // +0x24
    int m_superArmorTime;         // +0x30
    int m_buffMaxValue;           // +0x34
    float m_goldBase;             // +0x38
    float m_goldMulti;            // +0x3c
    int m_equipmentUpgradeMax;    // +0x40
    std::vector<BlueMarbleRandomDungeonInfo> m_randomDungeonList;      // +0x44
    std::vector<BlueMarbleRandomDungeonInfo> m_bossDungeonList;        // +0x50
    std::vector<BlueMarbleRandomDungeonInfo> m_uniqueDungeonList;      // +0x5c
    std::vector<BlueMarbleRandomItemInfo> m_randomItemList;            // +0x68
    std::vector<BlueMarbleRewardInfo> m_rewardList;                    // +0x74
    std::vector<BlueMarbleTilePos> m_tilePosList;                      // +0x80
};

#endif  // GAME_BLUEMARBLE_INFOSCRIPT_H_
