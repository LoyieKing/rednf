// df_game_r CDungeon / CDungeon_TowerOfDespair / STDungeonScript 还原（G2 副本链）。
// 布局依据（ORIG df_game_r 反汇编）：
//   CDungeon ctor 0x08373f3c / dtor 0x08376fd8（sizeof 0x934，vtable + 16*4 容器族）
//   STDungeonScript ctor 0x0836b5e2 / dtor 0x0836bd5c / Clear 0x0836abc6（sizeof 0x50a）
//   MazeScript ctor 0x088f2b4a / copy 0x08376d8a / dtor 0x08376f00（sizeof 0x74）
//   stDungeonAssignItem_t ctor 0x088f2ca8（int + list<pair<int,int>>，0x1c 字节）
//   STWarRoomData ctor 0x0836a1f6 / clear 0x0836a398 / copy 0x0836a578（0x100 字节）
#pragma once

#define CDUNGEON_CLASS_DEFINED

#include <ext/hash_map>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"
#include "CClearCondition.h"
#include "CDailyScheduleManager.h"
#include "advancealtar_StageDifficulty.h"  // advancealtar::StageType（自本头收敛的叶子）
class CUser;
class CMap;

#ifndef DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED
#define DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED

enum ENUM_MAZE_QUEST_TYPE
{
    ENUM_MAZE_QUEST_TYPE_0 = 0
};
#endif

// 地图方向位（ORIG InsertMap 0x0834b988 的 pair 构造实参类型：
// _ZNSt4pairIKiP4CMapEC1I14PATH_DIRECTIONRS2_EEOT_OT0_ ×12；值 1/2/4/8 为
// direction 形参的四个位。枚举类型名入 mangled，故必须用真实名 PATH_DIRECTION。
#ifndef DNF_ENUM_PATH_DIRECTION_DEFINED
#define DNF_ENUM_PATH_DIRECTION_DEFINED

enum PATH_DIRECTION
{
    PATH_DIRECTION_1 = 1,
    PATH_DIRECTION_2 = 2,
    PATH_DIRECTION_4 = 4,
    PATH_DIRECTION_8 = 8
};
#endif

// ---------------------------------------------------------------------------
// 迷宫/网格支撑类型（GridScript/RidableObject/RidableScript/DungeonClearCondition）
// ---------------------------------------------------------------------------

struct GridScript
{
    int m_gridValue;                        // +0x00
    std::vector<int> m_mapCandidates;       // +0x04
    std::vector<int> m_secondCandidates;    // +0x10
    std::vector<int> m_layeredMapIndexes;   // +0x1c

    GridScript();
    void clear();
    // 拷贝构造/赋值由编译器按成员合成（ORIG C1ERKS_/aSERKS_ 为合成形态：
    // int 直拷 + vector 拷贝构造，83d281c）。
};

struct RidableObject
{
    // ORIG clear 0x830e47c：前 5 int 置 -1、+0x14 置 0xc8（sizeof=0x18）；
    // 字段名为与 DungeonRidableObject 前 6 字段的位置镜像【推断】，拷贝语义实证。
    int m_x;        // +0x00
    int m_y;        // +0x04
    int m_field0c;  // +0x08
    int m_field10;  // +0x0c
    int m_field14;  // +0x10
    int m_field18;  // +0x14

    RidableObject();
    // [b28d-Main 代落·S273 P317] 删用户空析构：ORIG 仅实例化平凡版
    // `_ZNSt12_Destroy_auxILb1EE9__destroyIP13RidableObjectEEvT_S4_` ⇒ trivial；
    // ImportDungeonScript 区间 0 处 D1 call（我方 16 EXTRA）。D1/D2 仍发射同名 W。
    void clear();
};

class RidableScript
{
public:
    RidableScript();
    ~RidableScript();
    void clear();

    int m_0;                                // +0x00
    // [D07MonsterScriptLeaf · 广播已发] ORIG 0x082fe707 `movzbl 0x4(%eax),%edx` 零扩展
    // 单段 ⇒ 真型 unsigned char（旧 char 使消费点写 `*(unsigned char*)&m_4` 多一条
    // `add $0x4,%eax`）；宽度/偏移/sizeof/mangled 不变。
    unsigned char m_4;                      // +0x04
    std::vector<RidableObject> m_objects;   // +0x08
};


// MazeScript（0x74 字节，由 ORIG ctor/copy/dtor 逐字段实证）
class MazeScript
{
public:
    MazeScript();
    ~MazeScript();
    GridScript* getGrid(int x, int y);  // ORIG 88f2c2a：返回 &m_grids[w*y+x]
    const GridScript& getGridR(int x, int y) const;  // ORIG 0822adc2（实现在 MazeScript.cpp）
    void clear();

    // +0x00/+0x04：匿名 union{struct,数组}——①struct 聚合使 ORIG C1ERKS
    // 0x08376d92/aSERKS 的成对搬移可复现（C28L20C 同法）；②数组视图使 clear
    // 0x088f2a76 的 `for(i=0;i<=1;++i) arr[i]=1` 索引写可复现（ORIG 用
    // `movl $0x1,(%eax,%edx,4)` 循环而非两条直写）。布局/偏移/访问语法不变。
    union
    {
        struct
        {
            int m_width;                          // +0x00（clear 置 1）
            int m_height;                         // +0x04（clear 置 1）
        };
        int m_dims[2];                            // 同布局数组视图（clear 循环用）
    };
    std::vector<GridScript> m_grids;          // +0x08
    std::vector<std::pair<int, int> > m_vec14;  // +0x14
    std::vector<std::pair<int, int> > m_vec20;  // +0x20
    // [DNF-FIX-PVF 2026-10-07] 以下字段名按 ImportDungeonScript 写入点（mazeLocal
    // = ORIG ebp-0x1324，字段偏移 = 槽 + 0x1324）与 tag 具名；布局/宽度不变。
    int m_sealDoorMapIdx;                     // +0x2c（clear 置 -1；[seal door map index]
                                              //      ORIG 0x088e98fe -0x12f8；SetGridPath→m_hellMapIdx）
    int m_sealDoorPosX;                       // +0x30（clear 置 -1；[seal door pos] 首值
                                              //      ORIG 0x088e9972 -0x12f4）
    int m_sealDoorPosY;                       // +0x34（clear 置 -1；[seal door pos] 次值
                                              //      ORIG 0x088e9984 -0x12f0）
    RidableScript m_ridable;                  // +0x38
    int m_questType;                          // +0x4c（maze quest type；clear 置 -1；
                                              //      set_dungeon 按 0/1/-1 分流；
                                              //      [quest connection] 首值 ORIG 0x088e86af -0x12d8）
    int m_questIdx;                           // +0x50（clear 置 -1；[quest connection] 次值
                                              //      ORIG 0x088e86c1 -0x12d4）
    int m_questMinDiff;                       // +0x54（clear 置 -1；[quest connection] type==0
                                              //      第三值 ORIG 0x088e8737 -0x12d0；
                                              //      GetAppropriateMaze 要求 <= diff）
    std::vector<DungeonClearCondition> m_clearCondition; // +0x58
    int m_mazeIndex;                          // +0x64（clear 置 0；push_back 前
                                              //      m_mazeList170.size()，ORIG 0x088e8600/
                                              //      0x088f09c8 -0x12c0；VerifyMaze 日志 maze(%d)）
    int m_partyCountMin;                      // +0x68（GetRandMazebyPartyCount 下界；
                                              //      [party num check] ORIG 0x088e8ab1 -0x12bc）
    int m_partyCountMax;                      // +0x6c（GetRandMazebyPartyCount 上界；
                                              //      [party num check] ORIG 0x088e8ac3 -0x12b8）
    unsigned char m_escapeHell;               // +0x70（clear 字节清零；拷贝按 1 字节，
                                              //      ORIG movzbl 0x7876e89 实证；[escape hell]
                                              //      ScanInt!=0，ORIG 0x088e8b37 setne→-0x12b4）
};
// ORIG vector<MazeScript>::size 0x0838a192：sar 2 + imul 0x4f72c235（29 的逆元）⇒ 0x74。
static_assert(sizeof(MazeScript) == 0x74, "MazeScript size mismatch");

// ---- CDungeon 支撑类型（尺寸来自 ORIG 容器函数 / 构造体）----

struct stDungeonAssignItem_t
{
    int m_assignItemIdx;                        // +0x00（-1 = 按标准等级生成）
    std::list<std::pair<int, int> > m_list;     // +0x04

    stDungeonAssignItem_t();
    ~stDungeonAssignItem_t();
    void clear();
    // 拷贝构造/赋值合成（ORIG C1ERKS_ 83d1a94：int 直拷 + list 拷贝构造）。
};

struct RandomList
{
    int m_mapIndex;   // +0x00（randomMapIndex 0x0814f4fc 输出地图索引）
    int m_weight;     // +0x04（10000 段权重，累加比较）

    // [b28d-Main 代落·S273 P317] 同族（ORIG 平凡 __destroyIP10RandomListE 实证）。
};

struct stEventMonster_t
{
    int m_eventIdx;      // +0x00
    int m_monsterIdx;    // +0x04
    int m_prob;          // +0x08
    int m_field0c;       // +0x0c
    int m_field10;       // +0x10

    // [b28d-Main 代落·S273 P317] 同族（ORIG D1/D2 W size=5）。
};

struct stSecondEventMonster_t
{
    int m_eventIdx;      // +0x00
    int m_monsterIdx;    // +0x04
    int m_prob;          // +0x08
    int m_field0c;       // +0x0c
    int m_field10;       // +0x10

    // [b28d-Main 代落·S273 P317] 同族。
};

struct DimensionPartyCount
{
    char m_0;   // +0x00
    char m_1;   // +0x01
    char m_2;   // +0x02

    DimensionPartyCount()
    {
        m_0 = 0xff;
        m_1 = 0xff;
        m_2 = 0xff;
    }

    DimensionPartyCount(char a, char b, char c)
    {
        m_0 = a;
        m_1 = b;
        m_2 = c;
    }
    bool operator==(const DimensionPartyCount& other) const
    {
        return m_0 == other.m_0 && m_1 == other.m_1;
    }
};

// ORIG ImportDungeonScript 0x088ea9eb 逐条读取四个 int；WarField 按四列
// 累积概率选择 monster list。使用具名数组替代原始 0x10 字节占位区。
class STMonsterTypeSpawnProb
{
public:
    STMonsterTypeSpawnProb();
    void clear();
    int m_probability[4];       // +0x00..+0x0c
};

// ORIG ImportDungeonScript 0x088e8418：pathgate object 写入 3+3+2+2
// 个 int，+0x118 与 +0x124 为未消费的结构槽。Clear（0x0836adf5）为
// 物理双层循环 i<=3×j<=2、地址 +0xf8+(3i+j)*4 连续 12 int 全写 -1，
// 故建模为 int[4][3]（组边界=循环步长，非 3+3+2+2 分组）。
struct STPathgateObject
{
    int m_cells[4][3];          // +0x00..+0x2c（STDungeonScript +0xf8..+0x127）
                                // [0]=group0、[1]=group1、[2][0..1]=group2、
                                // [2][2]=unused@+0x118、[3][0..1]=group3、
                                // [3][2]=unused@+0x124
};
static_assert(sizeof(STPathgateObject) == 0x30, "STPathgateObject size mismatch");

class STDeadTowerData
{
public:
    STDeadTowerData();
    ~STDeadTowerData();
    void clear();
    std::vector<int> m_vec;   // +0x00
};

class STWarRoomData
{
public:
    STWarRoomData();
    ~STWarRoomData();
    void clear();
    void copy(STWarRoomData* other);

    int m_0;                                  // +0x00
    int m_4;                                  // +0x04
    int m_8;                                  // +0x08
    int m_c;                                  // +0x0c
    int m_10;                                 // +0x10
    std::vector<int> m_vec14[4];              // +0x14..+0x43（ORIG ctor 数组循环 836a202）
    std::vector<int> m_vec44;                 // +0x44
    int m_50;                                 // +0x50
    int m_54;                                 // +0x54
    int m_58;                                 // +0x58
    int m_5c[4];                              // +0x5c
    std::vector<int> m_vec6c[4];              // +0x6c..+0x9b（ORIG ctor 数组循环 836a275）
    int m_9c[4];                              // +0x9c
    float m_ac;                               // +0xac（[kill count const]）
    std::vector<STMonsterTypeSpawnProb> m_spawnProb; // +0xb0
    float m_bc;                               // +0xbc（[monster exp bonus per user decrease]）
    float m_c0;                               // +0xc0（[result exp bonus per user decrease]）
    int m_c4[4];                              // +0xc4
    int m_d4;                                 // +0xd4
    int m_d8;                                 // +0xd8
    float m_dc[4];                            // +0xdc（[monster type spawn interval rate]）
    int m_ec;                                 // +0xec
    int m_f0;                                 // +0xf0
    int m_f4;                                 // +0xf4
    int m_f8;                                 // +0xf8
    int m_fc;                                 // +0xfc
};

class RequiredItem
{
public:
    RequiredItem();
    void clear();

    int m_itemIdx;     // +0x00（-1 = 无入场物品；check_item_exist/make_item 实证）
    int m_itemCount;   // +0x04（需求数量，m_addInfo2 比较实证）
    bool m_enableFlag; // +0x08（b28d-Main P292：ORIG 0x0859e7fe `movzbl; xor $1; test; je` ⇒ bool；非 0 启用校验）
};

#include "advancealtar_StageDifficulty.h"  // StageType 权威定义（叶子头）
#include "AdvanceAltarField.h"  // advancealtar::_RewardItem 等共享类型权威定义
namespace advancealtar
{
class ClearRewardData
{
public:
    ClearRewardData();
    ~ClearRewardData();
    ClearRewardData& operator=(const ClearRewardData& other);
    void clear();

    // ORIG clear 0x83xxxx：vector<_RewardItem> 数组 [3]（clear/ctor/dtor 均
    // 以 i=0..2 循环实证，8376c9c 区段）；sizeof 0x24。
    std::vector<_RewardItem> m_vec[3];  // +0x00/+0x0c/+0x18
};

struct RoundRewardData
{
    int m_0;                          // +0x00
    int m_4;                          // +0x04
    std::vector<_RewardItem> m_vec8;  // +0x08
};

class SurvivalRewardData
{
public:
    SurvivalRewardData();
    ~SurvivalRewardData();
    SurvivalRewardData& operator=(const SurvivalRewardData& other);

    std::vector<RoundRewardData> m_vec;  // +0x00（ORIG sizeof 0xc，operator= 仅复制 vector）
    // ORIG T W? 无：_ZNK12advancealtar18SurvivalRewardData18getRoundRewardDataEi
    // @0x0889aebe（**const** 成员，nm 实证 K 前缀）：遍历 m_vec 首个满足
    // m_0<=round<=m_4 的 RoundRewardData，命中返回 &元素.m_vec8，未命中返 0。
    // Wave5 AltarProc 追加声明、AltarMisc 批次实现（原非 const 签名与 ORIG
    // mangled 不符，2026-08-27 AltarMisc 改正）。
    std::vector<_RewardItem>* getRoundRewardData(int round) const;
};

// ---- StageType / StageDifficulty / RewardItemType 字符串枚举解析器 ----
// ORIG: getStageType 0x88990ba / getDifficulty 0x8899038 /
// getRewardItemType 0x889acaa；实现见 STDungeonScript.cpp。
// StageType 已收敛至 advancealtar_StageDifficulty.h 叶子头（2026-08-24
// GlobalBossAndAltar；本头经 CDataManager.h→advancealtar.h 链或直接 include 取得）。


class RewardItemType
{
public:
    enum T
    {
        REWARD_NORMAL = 0,
        REWARD_EVENT = 1
    };
    static bool getRewardItemType(const std::string& name, T& out);
};
}

struct TournamentRewardRate
{
    // ORIG C1/C2 W 0x088f2da0：4×movl 0 逐字段清零。
    TournamentRewardRate()
    {
        m_0 = 0;
        m_4 = 0;
        m_8 = 0;
        m_c = 0;
    }

    int m_0;   // +0x00
    int m_4;   // +0x04
    int m_8;   // +0x08
    int m_c;   // +0x0c
};

struct TournamentRewardItem
{
    // ORIG sizeof=12：vector size() 反比 magic 0xAAAAAAAB（/12，82855e7）与
    // allocate 的 n*3*4（new_allocator）双证。字段语义未证，按 POD 整型建模。
    // C1 0x088f2dcc 三字段清零；D1 0x088f9ef6 空体（vector 元素内联实例化）。
    TournamentRewardItem()
    {
        m_data[0] = 0;
        m_data[1] = 0;
        m_data[2] = 0;
    }
    // [b28d-Main 代落·S273 P317] 删头内空析构（同族）。
    int m_data[3];
};

class TournamentRewardItemRate
{
public:
    TournamentRewardItemRate();
    ~TournamentRewardItemRate();
    TournamentRewardItemRate& operator=(const TournamentRewardItemRate& other);
    void Clear();

    std::vector<TournamentRewardItem> m_items;  // +0x00
    int m_c;                                    // +0x0c
};  // sizeof == 0x10（ORIG Clear 0x836ab66 与 STDungeonScript::Clear
    //    m_itemRate4b4→m_4c4@+0x4c4 实证；无 m_10 字段）

struct STNamedMonsterMapPos
{
    STNamedMonsterMapPos()
    {
        m_y = 0;
        m_x = m_y;
    }
    // [b28d-Main 代落·S273 P317] 删头内空析构（同族）。
    int m_x;   // +0x00
    int m_y;   // +0x04
};
class stConditionEnterBossRoom_t
{
public:
    stConditionEnterBossRoom_t();  // ORIG 836b552：仅 vector 构造，m_0 不初始化
    ~stConditionEnterBossRoom_t();
    void clear();

    int m_0;                    // +0x00
    std::vector<int> m_vec4;    // +0x04
};

// ORIG sizeof=12（vector<RecoveryInfo>::size 反比 magic /12 实证）。
struct RecoveryInfo
{
    // [b28d-Main 代落·S273 P317] 删用户空析构（ORIG D1 W 0x088f9346 空体，同族）。
    // ImportDungeonScript 0x88eedd5 起三处 fstps 直存 → ORIG 为 float[3]。
    float m_data[3];
};

struct RecoveryStairInfo
{
    RecoveryStairInfo();
    ~RecoveryStairInfo();

    // ORIG C1ERKS_/aSERKS_（88f627e/88f6344）：前 3 个字段逐个拷贝 +
    // vector 拷贝，为合成形态 → 拷贝构造/赋值由编译器生成。
    int m_0;                          // +0x00（ImportDungeonScript 0x88eed65 fistpl 实证 int）
    int m_4;                          // +0x04（同上 0x88eedad）
    float m_8;                        // +0x08（ImportDungeonScript 0x88eedc1 fstps 实证 float）
    std::vector<RecoveryInfo> m_vec;  // +0x0c
};

struct UseCoinDungeonDiff
{
    // ORIG C1/C2 W 0x088f2dee：movl $0xffffffff→+0 / movb $1→+4。
    UseCoinDungeonDiff()
    {
        m_0 = -1;
        m_4 = true;
    }

    int m_0;    // +0x00
    bool m_4;   // +0x04（checkFreeRevivalCondition 以 xor $1 取反测试，83652fc）
    char m_5;   // +0x05
    char m_6;   // +0x06
    char m_7;   // +0x07
};

// _DailySchedule 权威定义已上移至 CDailyScheduleManager.h（本头经其包含）。

namespace AbsoluteChangeStat
{
enum T
{
    T_0 = 0
};
}

class STTowerDialog
{
public:
    STTowerDialog();
    // ORIG W 0x088f6698（C1ERKS/C2ERKS 同址，头内联）：int 拷贝 → string 拷贝
    // 构造 → 3 byte 拷贝。
    STTowerDialog(const STTowerDialog& other)
        : m_index(other.m_index), m_name(other.m_name),
          m_flag8(other.m_flag8), m_flag9(other.m_flag9), m_flagA(other.m_flagA)
    {
    }
    ~STTowerDialog();
    STTowerDialog& operator=(const STTowerDialog& other);
    void clear();

    int m_index;        // +0x00（clear 置 0；ORIG 报告语义 index）
    std::string m_name; // +0x04（clear 调 std::string::clear）
    char m_flag8;       // +0x08（三个 flag 字节；clear 置 0）
    char m_flag9;       // +0x09
    char m_flagA;       // +0x0a
};

class STTowerFullDialog
{
public:
    STTowerFullDialog();
    ~STTowerFullDialog();
    void clear();

    std::vector<STTowerDialog> m_vec0;    // +0x00
    std::vector<STTowerDialog> m_vecc;    // +0x0c
    std::vector<STTowerDialog> m_vec18;   // +0x18
};

// ---- STDungeonScript（sizeof 0x50a，ORIG ctor 0x0836b5e2 / dtor 0x0836bd5c / Clear 0x0836abc6）----
// 布局依据 ORIG Clear/set_dungeon 反汇编；32 位 std::string = 4 字节（COW 指针）。
struct STDungeonScript
{
    int m_0;                                // +0x00
    int m_4;                                // +0x04
    int m_8;                                // +0x08
    char m_c;                               // +0x0c
    std::string m_str10;                    // +0x10
    int m_14;                               // +0x14
    int m_18;                               // +0x18（dungeon index）
    std::string m_str1c;                    // +0x1c
    std::string m_str20;                    // +0x20
    std::string m_str24;                    // +0x24
    std::string m_str28;                    // +0x28
    int m_2c;                               // +0x2c
    std::string m_str30;                    // +0x30
    int m_34;                               // +0x34
    std::vector<std::string> m_vec38;       // +0x38
    int m_44;                               // +0x44
    std::vector<std::string> m_vec48;       // +0x48
    std::string m_str54;                    // +0x54
    std::string m_str58;                    // +0x58
    // +0x5c：ORIG 以二维形访问（add r,r; lea (r,r,1)，Clear 0x0836acd0 /
    // Import 0x088e741c），一维写法在 4.4.6 恒产 add mem 形（探针实证）⇒ 双视角。
    union {
        int m_5c[6];
        int m_5cGrid[3][2];
    };
    std::string m_str74;                    // +0x74
    int m_78;                               // +0x78（标准等级源）
    RequiredItem m_requiredItem7c;          // +0x7c
    unsigned char m_88;                              // +0x88
    RequiredItem m_requiredItem8c;          // +0x8c
    std::vector<std::pair<int, int> > m_vec98;   // +0x98
    int m_a4;                               // +0xa4
    int m_a8;                               // +0xa8
    int m_ac;                               // +0xac
    float m_b0;                             // +0xb0（set_dungeon 位模式复制到 CDungeon.m_14）
    int m_b4[5];                            // +0xb4（5 个 int，set_dungeon 复制到 CDungeon+0x6e0）
    std::vector<std::pair<int, int> > m_vecc8;   // +0xc8
    std::vector<std::pair<int, int> > m_vecd4;   // +0xd4
    std::vector<std::pair<int, int> > m_vece0;   // +0xe0
    std::vector<stDungeonAssignItem_t> m_vecec;  // +0xec
    STPathgateObject m_pathgate;             // +0xf8（ORIG POD 区）
    int m_128[5];                           // +0x128（set_dungeon copy 到 CDungeon+0x880）
    int m_13c;                              // +0x13c
    bool m_140;                                      // +0x140（[b28d-solo-CDungeon] uchar→bool：
                                                     //  与 CDungeon::m_89d(bool) 同型，ORIG set_dungeon
                                                     //  0x0834b388 `movzbl 0x140,%edx; mov %dl,0x89d` 单段直存）
    unsigned char m_141;                             // +0x141
    unsigned char m_142;                             // +0x142
    std::vector<std::pair<int, int> > m_vec144;  // +0x144
    unsigned char m_150;                             // +0x150
    float m_154;                            // +0x154（[use zoom rate]）
    unsigned char m_158;                             // +0x158
    unsigned char m_159;                             // +0x159
    std::vector<int> m_vec15c;              // +0x15c
    int m_168;                              // +0x168
    int m_16c;                              // +0x16c
    std::vector<MazeScript> m_mazeList170;  // +0x170
    std::string m_str17c;                   // +0x17c
    STDeadTowerData m_deadTower180;         // +0x180
    STWarRoomData m_warRoom18c;             // +0x18c
    // [b28d-SweepCPartyDepsD2-2] uchar→bool（P248 档②/T3 消费端直证）：ORIG
    //   cal_hellparty_value 0x085b591e / increase_ancient_dungeon_ticket 0x0859bcc6
    //   读 CDungeon::m_89c（本字段的拷贝目标）为 `movzbl;xor$1;test;je`（=`!field`）；
    //   探针 /tmp/probe_d2.cpp 实测 bool 字段产该形、uchar 字段恒产 `cmp$1`。写入仅
    //   STDungeonScript.cpp:1963 `ScanInt((bool*)0) != 0` 与 :107 `=0` ⇒ 值域严格
    //   {0,1}；1 字节/偏移/mangled 零变化（与同头 m_140/m_89d 同法）。
    bool m_28c;                                      // +0x28c（ORIG Clear 清零）
    unsigned char m_28d;                             // +0x28d
    unsigned char m_28e;                             // +0x28e
    unsigned char m_28f;                             // +0x28f（ORIG Clear 清零）
    std::map<AbsoluteChangeStat::T, float> m_statMap290;  // +0x290（sizeof 0x18）
    unsigned char m_2a8;                             // +0x2a8
    unsigned char m_2a9;                             // +0x2a9
    unsigned char m_2aa;                             // +0x2aa
    unsigned char m_2ab;                             // +0x2ab（defense dungeon）
    int m_2ac;                                       // +0x2ac（advance altar type）
    int m_2b0[3];                           // +0x2b0
    int m_2bc[3];                           // +0x2bc
    std::vector<int> m_vec2c8;              // +0x2c8
    advancealtar::ClearRewardData m_clearReward2d4;  // +0x2d4
    advancealtar::SurvivalRewardData m_survival2f8;  // +0x2f8
    unsigned char m_304;                             // +0x304
    unsigned char m_305;                             // +0x305
    unsigned char m_306;                             // +0x306
    std::vector<DimensionPartyCount> m_dimension308; // +0x308
    unsigned char m_314;                             // +0x314
    unsigned char m_315;                             // +0x315
    unsigned char m_316;                             // +0x316
    unsigned char m_317;                             // +0x317
    std::vector<_DailySchedule> m_dailySchedule;     // +0x318
    unsigned char m_324;                             // +0x324
    unsigned char m_325;                             // +0x325
    stConditionEnterBossRoom_t m_bossRoom328;        // +0x328
    int m_338;                              // +0x338
    int m_33c;                              // +0x33c
    std::map<int, std::vector<RandomList> > m_map340; // +0x340
    int m_358;                              // +0x358
    std::vector<std::pair<int, int> > m_vec35c;      // +0x35c
    int m_368;                              // +0x368
    int m_36c;                              // +0x36c
    int m_370;                              // +0x370
    float m_374;                            // +0x374（[tower high skill initial cool time rate]）
    float m_378;                            // +0x378（[tower high skill initial cool time rate]）
    int m_37c;                              // +0x37c
    int m_380;                              // +0x380
    std::vector<RecoveryStairInfo> m_vec384;         // +0x384
    STTowerFullDialog m_towerDialog390;     // +0x390
    int m_3b4;                              // +0x3b4
    int m_3b8;                              // +0x3b8
    int m_3bc;                              // +0x3bc
    int m_3c0;                              // +0x3c0
    int m_3c4;                              // +0x3c4
    std::vector<STNamedMonsterMapPos> m_vec3c8;      // +0x3c8
    int m_3d4;                              // +0x3d4
    int m_3d8;                              // +0x3d8
    int m_3dc;                              // +0x3dc
    std::vector<stEventMonster_t> m_vec3e0;          // +0x3e0
    int m_3ec;                              // +0x3ec
    int m_3f0;                              // +0x3f0
    int m_3f4;                              // +0x3f4
    std::vector<stSecondEventMonster_t> m_vec3f8;    // +0x3f8
    std::vector<std::vector<float> > m_vec404;       // +0x404
    int m_410[5];                           // +0x410
    int m_424[5];                           // +0x424
    float m_438;                            // +0x438（[monster difficulty bonus]）
    float m_43c;                            // +0x43c（[monster difficulty bonus]）
    float m_440;                            // +0x440（[monster difficulty bonus]）
    float m_444;                            // +0x444（[monster difficulty bonus]）
    std::string m_str448;                   // +0x448
    unsigned char m_44c;                             // +0x44c
    int m_450;                              // +0x450
    int m_454;                              // +0x454
    std::vector<int> m_vec458;              // +0x458
    std::vector<int> m_vec464;              // +0x464
    std::vector<int> m_vec470;              // +0x470
    unsigned char m_47c;                             // +0x47c
    int m_480;                              // +0x480
    int m_484;                              // +0x484
    int m_488;                              // +0x488
    bool m_48c;                                      // +0x48c（b28d-solo-CDungeon: uchar→bool，
                                                     //  ORIG set_dungeon 直存 mov %dl,0x6c8 无 test/setne）
    int m_490;                              // +0x490
    unsigned char m_494;                             // +0x494
    int m_498;                              // +0x498
    std::map<int, TournamentRewardRate> m_map49c;    // +0x49c
    TournamentRewardItemRate m_itemRate4b4;          // +0x4b4
    union
    {
        int m_4c4;                          // +0x4c4（set_dungeon 按位复制）
        float m_4c4Value;                   // +0x4c4（[tournament clear reward gold rate]）
    };
    std::map<unsigned char, unsigned int> m_map4c8;  // +0x4c8
    bool m_4e0;                                      // +0x4e0（b28d-solo-CDungeon: uchar→bool，同上）
    std::map<int, UseCoinDungeonDiff> m_map4e4;      // +0x4e4
    int m_4fc;                              // +0x4fc
    int m_500;                              // +0x500
    bool m_504;                                      // +0x504（b28d-solo-CDungeon: uchar→bool，同上）
    bool m_505;                                      // +0x505（b28d-solo-CDungeon: uchar→bool，同上）
    int m_508;                              // +0x508（[disable exit]，ORIG dword 写入）
    char m_50c;                             // +0x50c（ORIG Clear movb $0,0x50c）

    STDungeonScript();
    ~STDungeonScript();
    void Clear();
};

// ---- CDungeon（sizeof 0x934，vtable @0，CDungeon_TowerOfDespair 只换 vtable）----
class CDungeon
{
public:
    CDungeon();
    ~CDungeon();   // ORIG 非虚：vtable 无 dtor 槽（0x8c34a08 仅 _do_after_dungeon_start）

    virtual bool _do_after_dungeon_start(CUser* user) const;   // [b28d-Main 代落·S326 P292] int→bool（ORIG 0x0859fd1e 调用点 `call *%edx; xor $0x1,%eax; test %al,%al; je`）

    // ---- 弱访问器（ORIG W 符号，定义于 CDungeon.cpp）----
    int get_index() const;
    int get_min_level() const;
    int get_standard_level() const;
    const char* GetDungeonName() const;
    int getDungeonKind() const;
    float get_exp_weight() const;
    char get_limit_party_count() const;   // ORIG 调用方全 movsbl（signed char 实证）
    char get_limit_Inout_count() const;
    unsigned char get_blood_dungeon_type() const;  // ORIG 单 movzbl（826b92e）
    unsigned char get_dimension_possible() const;  // ORIG 单 movzbl（826b93x）
    unsigned char get_dimension_min_partymem() const;
    bool limitOfStackableItemInTower() const;  // b28d-Main(P292)：ORIG 0x081c1b68 `call; xor $0x1,%eax; test %al,%al; je` + 0x081c1b8e `call; test %al,%al; je` ⇒ bool
    bool isNoFatigueDungeon() const;               // b25c Lane21 bool 化：ORIG 0x822b4de 单 movzbl 直返无规范化，xor 直判调用点需 bool callee
    int GetType_DungeonInHeritance() const;
    bool isTowerOfDespairDungeon() const;
    unsigned char getQuestNpcDungeon() const;      // ORIG 单 movzbl
    int getAICharacterAppearRate() const;
    int getEventMonsterTotalProb() const;
    int getTryGenEventMonsterCnt() const;
    int getTryGenEventMonsterProb() const;
    int getSecondEventMonsterTotalProb() const;
    int getTryGenSecondEventMonsterCnt() const;
    int getTryGenSecondEventMonsterProb() const;
    int getRandomTowerMaxStage() const;
    int getDeathTowerMaxStage() const;
    int getDeathTowerMapIndex(unsigned int idx) const;
    int getTowerMaxClearRewardItemNum() const;
    int getHeroesRequiredQuest() const;
    int GetDummyAppearCount() const;
    // [b28d-Lane27] 类型旋钮 uchar→bool（Main 批准）：ORIG 调用点 0x08307efe
    //   `call; test %al,%al; je`（**2 条、无 setne** = bool callee，P292 表）；
    //   本体 0x0830e610 单 `movzbl 0x624(%eax),%eax; ret`（无归一化，字段本身即 bool 语义）。
    //   返回类型不入 mangled；bool/unsigned char 同宽 ⇒ sizeof/偏移零变化。
    bool IsHaveDummyMap() const;
    // [b28d-solo-CBossStage] 返回类型 uchar→bool：ORIG 调用点 0x0814d6dc
    //   `call; test %al,%al; je`（P292 两条形=bool；uchar 恒 4 条含 setne）。
    //   返回类型不入 mangled；sizeof/成员偏移零影响。
    bool hasTowerItemDrop() const;
    std::vector<std::pair<int, int> >* getDropItems() const;
    std::vector<std::pair<int, int> >* getObjectDropItems() const;
    std::vector<stDungeonAssignItem_t>* getSpecialPassiveObjectItems() const;
    std::vector<stEventMonster_t>* getEventMonsters() const;
    std::vector<stSecondEventMonster_t>* getSecondEventMonsters() const;
    std::map<int, std::vector<RandomList> >* getRandomTowerMapIndexes() const;
    std::vector<MazeScript>* GetQuestMazeScriptVector(
        ENUM_MAZE_QUEST_TYPE mazeQuestType) const;

    void set_dungeon(STDungeonScript& script);
    void InsertMap(int mapType, int direction, CMap* map);
    void insertStart(int direction, CMap* map);
    int ChooseRandomMap(int mapType, int direction, int entrance) const;
    int GetMaze(int idx) const;
    void GetRandMaze(int& out) const;
    int GetRandMazebyPartyCount(int partyCount) const;
    int GetStartMap(int direction) const;
    int get_clear_item() const;
    int get_clear_item(int level) const;
    bool VerifyMaze() const;
    bool VerifyMaze(const MazeScript& maze) const;
    bool get_dimension_member_count(DimensionPartyCount& out) const;
    bool isRiskDungeon() const;   // ORIG 调用点按字节谓词测试（checkDungeonDifficulty 0x85be932）
    bool isTournamentDungeon() const;
    int getTournamentRoundFatigue() const;
    bool getLimitCoinDiff(int diff, int& out) const;  // b28d-Main(P292)：ORIG 0x085be61a 调用点 `call; test %al,%al; je` ⇒ bool
    bool checkFreeRevivalCondition(int diff) const;
    bool IsEnterEachMap() const;
    bool IsOpenEvenEnemy() const;
    bool IsOpenScheduleDungeon() const;
    int GetPartyMemberCoinLimit() const;

    int m_typeInheritance;          // +0x04（GetType_DungeonInHeritance；set_dungeon
                                    //      在 script+0x2a9 非 0 时置 1）
    int m_index;                    // +0x08
    int m_minLevel;                 // +0x0c（get_min_level）
    int m_standardLevel;            // +0x10（get_standard_level）
    float m_expWeight;              // +0x14（get_exp_weight；set_dungeon 以位模式复制）
    std::string m_name;             // +0x18（CodePage 转换后的副本名）
    std::map<int, int> m_clearItemMap; // +0x1c
    int m_heroesRequiredQuest;      // +0x34（getHeroesRequiredQuest）
    unsigned char m_dimensionPossible;    // +0x38（get_dimension_possible；
                                          //      非 0 时写 dimensionInout[m_38-1]）
    unsigned char m_dimensionInoutValue;  // +0x39（set_dimensionInout 的 value）
    unsigned char m_dimensionMinPartymem; // +0x3a（get_dimension_min_partymem）
    unsigned char m_limitPartyCount;      // +0x3b（get_limit_party_count）
    unsigned char m_limitInoutCount;      // +0x3c（get_limit_Inout_count）
    std::vector<std::pair<int, int> > m_vec40; // +0x40
    std::vector<std::pair<int, int> > m_vec4c; // +0x4c
    std::vector<stDungeonAssignItem_t> m_vec58; // +0x58
    __gnu_cxx::hash_map<int, CMap*> m_maplist[16]; // +0x64
    std::multimap<int, CMap*> m_map1a4[16];       // +0x1a4
    std::multimap<int, CMap*> m_dummyMap324[16];  // +0x324
    std::multimap<int, CMap*> m_dummyMap4a4[16];  // +0x4a4
    bool m_hasDummyMap;             // +0x624（IsHaveDummyMap；[b28d-Lane27] uchar→bool，同宽）
    int m_dummyAppearCount;         // +0x628（GetDummyAppearCount）
    std::vector<MazeScript> m_mazeList;     // +0x62c
    std::vector<MazeScript> m_mazeList0[2]; // +0x638（ORIG 数组循环构造/析构）
    int m_aiCharacterAppearRate;            // +0x650（getAICharacterAppearRate）
    int m_randomTowerMaxStage;              // +0x654（getRandomTowerMaxStage）
    std::map<int, std::vector<RandomList> > m_map658; // +0x658（随机塔地图表）
    int m_dungeonKind;                      // +0x670（getDungeonKind）
    std::vector<int> m_deathTowerMapIndexes; // +0x674（getDeathTowerMapIndex）
    int m_deathTowerMaxStage;               // +0x680（getDeathTowerMaxStage）
    int m_tryGenEventMonsterCnt;            // +0x684（getTryGenEventMonsterCnt）
    int m_tryGenEventMonsterProb;           // +0x688（getTryGenEventMonsterProb）
    int m_eventMonsterTotalProb;            // +0x68c（getEventMonsterTotalProb）
    std::vector<stEventMonster_t> m_eventMonsters;          // +0x690
    int m_tryGenSecondEventMonsterCnt;                      // +0x69c
    int m_tryGenSecondEventMonsterProb;                     // +0x6a0
    int m_secondEventMonsterTotalProb;                      // +0x6a4
    std::vector<stSecondEventMonster_t> m_secondEventMonsters; // +0x6a8
    bool m_limitStackableItemInTower;              // +0x6b4（同名访问器；b28d-Main: bool 使体保持 movzbl 直返）
    bool m_hasTowerItemDrop;                                // +0x6b5（[b28d-Main 代落·S345 P235/P292] uchar→bool：ORIG hasTowerItemDrop 0x0815092e 体 `movzbl; ret` 无 setne ⇒ 字段与返回同型；1 字节/偏移不变）
    unsigned int m_towerMaxClearRewardItemNum;              // +0x6b8（同名访问器；
                                        // [b28d-solo-CDungeon] int→unsigned：ORIG set_dungeon
                                        //  `mov 0x3b8,%eax; mov %eax,%edx` 异型 RHS-first+中转）
    std::vector<DimensionPartyCount> m_dimensionPartyCount; // +0x6bc
    bool m_noFatigueFlag;                   // +0x6c8（isNoFatigueDungeon；bool 化配对，布局不变）
    bool m_riskFlag;                        // +0x6c9（isRiskDungeon；bool 化配单
                                            //      movzbl 返回，布局不变）
    int m_partyMemberCoinLimit;             // +0x6cc（GetPartyMemberCoinLimit）
    bool m_enterEachMap;                    // +0x6d0（IsEnterEachMap）
    bool m_openEvenEnemy;                   // +0x6d1（IsOpenEvenEnemy）
    std::vector<int> m_vec6d4;      // +0x6d4（语义未证；ORIG Clear 清空）
    int m_6e0[5];                   // +0x6e0（set_dungeon 从 script+0xb4 复制 5 个 int）
    STWarRoomData m_warRoomData;    // +0x6f4
    RequiredItem m_entryRequiredItem;   // +0x7f4（入场必要物品；TOD 门票/vilmark/
                                        //      CheckEnter 链共用，set_dungeon 自
                                        //      script+0x7c 9 字节复制）
    unsigned char m_advanceAltarSurvivalFlag;   // +0x800（script+0x2ab；importer
                                        //      "[advance altar survival map]" 实证）
                                        // [b28d-solo-CDungeon] char→uchar：ORIG set_dungeon
                                        //  `movzbl 0x2ab,%edx; mov %dl,0x800` 单段直存（同型）
    int m_advanceAltarType;             // +0x804（script+0x2ac；importer StageType 实证）
    int m_808[3];                   // +0x808（script+0x2b0 3 int）
    std::vector<int> m_vec814;      // +0x814（script+0x2c8）
    int m_820[3];                   // +0x820（script+0x2bc 3 int）
    advancealtar::ClearRewardData m_clearReward; // +0x82c
    advancealtar::SurvivalRewardData m_survivalReward; // +0x850
    unsigned char m_entryRestrictFlag;  // +0x85c（script+0x88；CParty 2257 vilmark 入场
                                    //      物品链 / PartyClosure 分支门【推断】）
                                    // [b28d-solo-CDungeon] char→uchar：ORIG set_dungeon
                                    //  `movzbl 0x88,%edx; mov %dl,0x85c` 单段直存（同型）
    RequiredItem m_entryRequiredItem2;  // +0x860（第二入场物品槽，script+0x8c）
    std::vector<std::pair<int, int> > m_vec86c; // +0x86c（script+0x98）
    // [b28d-SweepCDungeon T1 直证] char→unsigned char（三字段，sizeof/偏移/mangled
    //   零变化）：ORIG CParty::CheckEnterVillageAttackRevenge 0x085ba84b/0x085ba85d
    //   读本字段为 `movzbl 0x879/0x878(%eax),%eax; movsbl %al,%eax`（= uchar 字段 +
    //   源码 (signed char) cast；char 字段应出单条 movsbl），0x085ba8bb 读
    //   m_maxRevengeCount 为 `movzbl 0x87b(%eax),%eax; cmpb/jae`（uchar + 无符号比较）；
    //   set_dungeon 赋值 ORIG `movzbl 0x314/0x315/0x317(%eax),%edx` 单段（uchar=uchar
    //   同型；char 目标恒产 movzbl→%eax; mov %eax,%edx 两段，探针 /tmp/probe_char.cpp
    //   f1(char) vs f2(uchar) 实证）。消费点：CParty.cpp:1550-1551 需 (signed char)
    //   cast 保持有符号比较（已同步）；DisPatcherCore.cpp:4542/4556、
    //   PartyClosureC2.cpp:904 已带 cast ⇒ 自动转 ORIG 形。
    unsigned char m_minAttackPartyMember;    // +0x878（CheckEnterVillageAttackRevenge 下界）
    unsigned char m_maxAttackPartyMember;    // +0x879（同上上界）
    bool m_villageAttackEventFlag;  // +0x87a（script+0x316；非 0 启用村庄攻击事件
                                    //      成员数/复仇数校验与疲劳豁免【推断】）
                                    // [b28d-51] P248 档②：ORIG 0x085a8cda
                                    //  `movzbl 0x87a; xor $1; test; je` = bool 字段
                                    //  + `==0/!` 形；char 恒无 xor。sizeof/偏移不变。
    unsigned char m_maxRevengeCount;        // +0x87b（复仇次数上限，uchar 比较 085ba8bb）
    int m_87c;                      // +0x87c（script+0xa4；-1 哨兵，语义未证）
    int m_880[5];                   // +0x880（std::copy 自 script+0x128）
    int m_894;                      // +0x894（script+0x13c）
    int m_898;                      // +0x898（script+0x168）
    // [b28d-SweepCPartyDepsD2-2] uchar→bool：ORIG cal_hellparty_value 0x085b591e
    //   `movzbl 0x89c(%eax),%eax; xor $0x1,%eax; test %al,%al; je` ⇒ `!m_89c`
    //   （P248 档② bool 字段直证；uchar 的 `!=1` 在 4.4 恒产 `cmp $1`，探针
    //   /tmp/probe_d2.cpp fH..fJ 四形实证）。写入仅 CDungeon.cpp:457
    //   `= script.m_28c`（已同改 bool ⇒ 单段直存，与 ORIG 0x0834b4xx
    //   `mov %dl,0x89c(%eax)` 一致）。1 字节/偏移/mangled 零变化。
    bool m_89c;                     // +0x89c（script+0x28c）
    // [b28d-solo-BattleDungeonMapA2] ORIG SelectDungeon 0x082ff6b9 为
    // `movzbl 0x89d(%eax),%eax; xor $0x1,%eax; test %al,%al; je`（P248 档②）
    // ⇒ 字段 bool（源码 `!dungeon->m_89d`）；1 字节/偏移不变。
    bool m_89d;                     // +0x89d（script+0x140；非真时清 hell 标记）
    bool m_89e;                     // +0x89e（script+0x28d，村庄怪副本标记；[g03·Main 批准 A/B] uchar→bool，P357 画像：我方 uchar 在 4.4 多出 setne;test 两段）
    unsigned char m_bloodDungeonType; // +0x89f（get_blood_dungeon_type；
                                      //      script+0x28e；1=血腥 2=终极）
    int m_8a0;                      // +0x8a0（script+0x3c4）
    int m_tournamentRoundFatigue;   // +0x8a4（getTournamentRoundFatigue）
    unsigned char m_tournamentFlag; // +0x8a8（isTournamentDungeon：signed <=1 判定）
    std::map<int, TournamentRewardRate> m_map8ac; // +0x8ac
    TournamentRewardItemRate m_itemRate8c4;       // +0x8c4（sizeof 0x10）
    union
    {
        int m_8d4;                  // +0x8d4（set_dungeon 原样 dword 搬运）
        float m_tournamentGoldRate; // +0x8d4（CParty 结算按 float 消费）
    };
    std::map<unsigned char, unsigned int> m_map8d8; // +0x8d8
    std::vector<STNamedMonsterMapPos> m_vec8f0;   // +0x8f0
    unsigned char m_questNpcDungeon; // +0x8fc（getQuestNpcDungeon；script+0x2aa）
    std::vector<std::vector<float> > m_vec900;    // +0x900
    stConditionEnterBossRoom_t m_bossRoom90c;     // +0x90c
    std::map<int, UseCoinDungeonDiff> m_map91c;   // +0x91c
};

class CDungeon_TowerOfDespair : public CDungeon
{
public:
    CDungeon_TowerOfDespair();
    ~CDungeon_TowerOfDespair();   // ORIG 非虚
    virtual bool _do_after_dungeon_start(CUser* user) const;   // [b28d-Main 代落·S326 P292] 派生覆写同步
};

namespace QuickParty
{
class CQuickPartySystemManager;
}

class CGameManager;
CGameManager* G_CGameManager();

/* batch-16：fallback 完整类定义删除（原为 include 图窄而设的本地最小声明；
   CParty.h→CBattle_Field.h→CDungeon.h 新链下与 QuickParty.h:110 权威定义构成
   redefinition（IpsC9 报告 PvP_Room 链）。需要成员调用者的 TU 请显式
   #include "QuickParty.h"；前向声明仍在上方 namespace 块。 */
