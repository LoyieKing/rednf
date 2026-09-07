// df_game_r CMap / STMapScript 还原（G2 副本链）。
// 布局依据：ORIG CMap ctor(0834dcda)/dtor(0834def6)/set_map(0834e18a)、
// STMapScript ctor(0836cc94)/dtor(0836d4bc)、各容器函数元素尺寸。
// CMap sizeof = 0x130；STMapScript sizeof = 0x37c。
#pragma once

#include <cstring>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "advancealtar.h"   // advancealtar::TimeLine（STMapScript 成员）
#include "CVillageMonsterMgr.h"   // MapArea / STAttackedMonster（全局作用域权威头）
#include "SPOBJ_ACTION_DATAS.h" // SPOBJ_ACTION_DATAS（ctor/getActionType 见其 .cpp）

class CDataManager;
class CMap;

int get_rand_int(int range);
bool isInPathArea(int x, int y, const int (*area1)[2], const int (*area2)[2]);
CDataManager* G_CDataManager();



// ORIG ENUM_DRAWLAYER（getDrawLayer 0x088be12d / getDrawLayerName 0x088be212
// 实证取值 0..7；成员名不可考——ORIG 无 DWARF，按层名字符串推断命名）。
enum ENUM_DRAWLAYER
{
    DRAWLAYER_CLOSE = 0,       // "[close]"
    DRAWLAYER_NORMAL = 1,      // "[normal]"
    DRAWLAYER_BOTTOM = 2,      // "[bottom]"
    DRAWLAYER_CLOSEBACK = 3,   // "[closeback]"
    DRAWLAYER_MIDDLEBACK = 4,  // "[middleback]"
    DRAWLAYER_DISTANTBACK = 5, // "[distantback]"
    DRAWLAYER_COVER = 6,       // "[cover]"
    DRAWLAYER_FORCE = 7        // getDrawLayer 未知名缺省值
};
// ---- CMap 支撑类型（尺寸来自 ORIG 容器函数 / 构造体）----

struct _mapItem
{
    int m_objectIdx;   // +0x00（ctor 不初始化）
    int m_field04;     // +0x04（ctor=0）
    int m_field08;     // +0x08（ctor=0）
    int m_field0c;     // +0x0c（ctor=0）

    _mapItem();   // ORIG 0815088e（唯一定义见 CMap.cpp；无 dtor 符号——平凡析构）
};

struct _mapMonster
{
    int m_mobId;              // +0x00  (monster idx, ctor=-1)
    char m_flag04;            // +0x04  (ctor=1)
    int m_field08;            // +0x08
    int m_spawnType;          // +0x0c  (0=单人 1=队伍 2=队伍x2)
    int m_levelAdd;           // +0x10
    int m_field14;            // +0x14
    int m_roleType;           // +0x18
    char m_champion;          // +0x1c
    int m_posX;               // +0x20
    int m_level;              // +0x24  (level = STAssignMonster.m_14/10；ORIG insert_monster 实证)
    int m_hpPct;              // +0x28  (ctor=100)
    int m_orderIdx;           // +0x2c
    char m_flag30;            // +0x30
    char m_pad31[3];          // +0x31
    std::vector<int> m_mobList; // +0x34

    _mapMonster();                        // ORIG 0830e4fc
    _mapMonster(const _mapMonster& other); // ORIG 0813d708
    ~_mapMonster();                       // ORIG 081398c6（仅析构 +0x34 vector）
};                                     // 唯一定义均见 CMap.cpp

class STAssignMonster
{
public:
    STAssignMonster();
    STAssignMonster(const STAssignMonster& other);
    ~STAssignMonster();
    STAssignMonster& operator=(const STAssignMonster& other);
    void Clear();

    int m_0;                        // +0x00  (ctor=-1)
    char m_4;                       // +0x04  (ctor=1)
    int m_8;                        // +0x08
    int m_c;                        // +0x0c
    int m_10;                       // +0x10
    int m_14;                       // +0x14  (level)
    int m_18;                       // +0x18  (ctor=1)
    int m_1c;                       // +0x1c  (ctor=1)
    int m_20;                       // +0x20
    int m_24;                       // +0x24
    std::vector<int> m_vec28;       // +0x28
    int m_34;                       // +0x34
    int m_38[2];                    // +0x38..+0x3f（与权威头 STAssignMonster.h 同步：ORIG Clear scaled 循环实证为数组）
    std::vector<std::pair<int, int> > m_vec40; // +0x40
    int m_4c;                       // +0x4c  (ctor=100)
    int m_50;                       // +0x50
    char m_54;                      // +0x54
    char m_55;                      // +0x55
    int m_58;                       // +0x58  (ctor=-1)
    std::vector<unsigned char> m_vec5c; // +0x5c
};

// ORIG STRivalMapData ctor 0x089e89fa（清 3 个 int；sizeof 0xc）。
struct STRivalMapData
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
    int m_field08;   // +0x08

    STRivalMapData() { m_field00 = 0; m_field04 = 0; m_field08 = 0; }
};

// ORIG STAICharacterMapArrangeData Clear 0x089e85fe / ctor 0x089e8676 /
// copy 0x083d31d2（sizeof 0x38）。Import [ai character] 写 +0x00 id、
// +0x04/+0x08/+0x0c、阵营→+0x10（character 0/monster 100/neutral 200）、
// roleType→+0x14（normal 5/champion 6/super champion 7/boss 8）、
// 坐标字节 →+0x18/+0x19、champion 特征→+0x1c vector<int>、
// [NPC] idx→+0x28（Clear=-1）、碰撞组→+0x2c vector<unsigned char>。
struct STAICharacterMapArrangeData
{
    int m_type;      // +0x00（Import 首个 int）
    int m_field04;   // +0x04
    int m_field08;   // +0x08
    int m_field0c;   // +0x0c
    int m_camp;      // +0x10（阵营；Clear=0）
    int m_roleType;  // +0x14（5..8）
    char m_posX;     // +0x18
    char m_posY;     // +0x19
    std::vector<int> m_championFeatures;       // +0x1c
    int m_npcIdx;                              // +0x28（Clear=-1）
    std::vector<unsigned char> m_collisionGroup; // +0x2c

    STAICharacterMapArrangeData();                                        // ORIG 0x089e8676
    STAICharacterMapArrangeData(const STAICharacterMapArrangeData& other); // ORIG 0x083d31d2
    ~STAICharacterMapArrangeData();                                        // ORIG 0x083dc9a0
    void Clear();                                                          // ORIG 0x089e85fe
}; 

struct STAssignBloodSystem
{
    // ORIG C1/C2 W 0x089e84bc：仅 call Clear。
    STAssignBloodSystem() { Clear(); }

    int m_id;          // +0x00
    int m_field04;     // +0x04
    int m_phase;       // +0x08
    int m_field0c;     // +0x0c
    int m_field10;     // +0x10
    int m_timeA;       // +0x14
    int m_timeB;       // +0x18
    int m_startCnt;    // +0x1c
    int m_cntInc;      // +0x20
    int m_totalCnt;    // +0x24

    void Clear();
};

struct STAssignBloodPhaseTime
{
    // ORIG C1/C2 W 0x089e8518：仅 call Clear。
    STAssignBloodPhaseTime() { Clear(); }

    int m_smallRoundOrder;   // +0x00
    int m_phaseIdx;          // +0x04（-1 = 随机）
    int m_interval;          // +0x08
    float m_time;            // +0x0c
    int m_field10;           // +0x10
    int m_spawnCnt;          // +0x14
    char m_difficulty;       // +0x18
    char m_pad19[3];         // +0x19

    void Clear();
};

// Import [defense monster]/[defense phase time] 与 Blood 系列同构
//（ORIG STAssignDefenseSystem::Clear / STAssignDefensePhaseTime::Clear）。
struct STAssignDefenseSystem
{
    // ORIG C1/C2 W 0x089e8594：仅 call Clear。
    STAssignDefenseSystem() { Clear(); }

    int m_id;          // +0x00
    int m_field04;     // +0x04
    int m_phase;       // +0x08
    int m_field0c;     // +0x0c
    int m_field10;     // +0x10
    int m_timeA;       // +0x14
    int m_timeB;       // +0x18
    int m_startCnt;    // +0x1c
    int m_cntInc;      // +0x20
    int m_totalCnt;    // +0x24

    void Clear();
};

struct STAssignDefensePhaseTime
{
    // ORIG C1/C2 W 0x089e85ea：仅 call Clear。
    STAssignDefensePhaseTime() { Clear(); }

    int m_smallRoundOrder;   // +0x00
    int m_phaseIdx;          // +0x04（-1 = 随机）
    int m_interval;          // +0x08
    float m_time;            // +0x0c
    int m_field10;           // +0x10
    int m_spawnCnt;          // +0x14

    void Clear();
};

// ORIG AssignNpc::ctor 0x089e8836 / clear 0x089e87ee（sizeof 0x1c；dtor 平凡）。
// Import [NPC]:首个 int→m_npcIdx,第二 int→m_field04,[visible on dungeon clear]
// 置 m_visible,[left]=0/[right]=1→m_dir,末尾 x/y/z。
struct AssignNpc
{
    int m_npcIdx;    // +0x00 (clear=0)
    int m_field04;   // +0x04 (clear=-1)
    bool m_visible;  // +0x08 (clear=false)
    int m_dir;       // +0x0c (clear=0)
    int m_x;         // +0x10
    int m_y;         // +0x14
    int m_z;         // +0x18

    AssignNpc() { clear(); }
    void clear();
};

// Import [special passive object] 写 +0x00..0x0c 四个 int（id+3 参数），
// action 表在 +0x10；Export 按 '%d %d %d %d' 读出。
struct STAssignSpecialPassiveObj
{
    int m_0;    // +0x00
    int m_4;    // +0x04
    int m_8;    // +0x08
    int m_c;    // +0x0c
    std::vector<SPOBJ_ACTION_DATAS> m_vec10; // +0x10
    int m_1c;                                // +0x1c

    STAssignSpecialPassiveObj();   // ORIG C1Ev W 0x089e87a6（vector 构造 + Clear()）
    void Clear();
};


// ORIG TownPath ctor 0x089e828e / clear 0x089e8258 / copy 0x083d9b78 /
// operator= 0x089ea970（sizeof 0x30）：切片实证首 0x1c 字节为 MapArea 子对象
// （clear 首条 call _ZN7MapArea5clearEv、ctor call _ZN7MapAreaC1Ev、
//  copy call _ZN7MapAreaC1ERKS_、assign call _ZN7MapAreaaSERKS_ 均 this+0），
// 其后 2 int + vector<int>[quest condition]@0x24。以 MapArea 基类复现该子对象。
struct TownPath : MapArea
{
    int m_field1c;           // +0x1c（clear=0）
    int m_field20;           // +0x20（clear=0）
    std::vector<int> m_conds;    // +0x24（[quest condition] 列表）

    TownPath();                          // ORIG 0x089e828e
    TownPath(const TownPath& other);     // ORIG 0x083d9b78
    TownPath& operator=(const TownPath& other); // ORIG 0x089ea970
    void clear();                        // ORIG 0x089e8258
};

// ORIG STAssignPassiveObject ctor 0x089e8440 / Clear 0x089e8414（W）：
// Clear 置 +0=-1、+4/+8/+c=0；ctor 调 Clear（定义见 STMapScript.cpp）。
// Import [passive object] 写 4 个 int；Export 按 '%d %d %d %d' 读出。sizeof 0x10。
struct STAssignPassiveObject
{
    int m_0;     // +0x00
    int m_4;     // +0x04
    int m_8;     // +0x08
    int m_c;     // +0x0c

    STAssignPassiveObject();
    void Clear();
};

struct TournamentMonster
{
    int m_mobId;   // +0x00
    int m_rate;    // +0x04

    TournamentMonster();  // ORIG 0830e4d4（定义见 TournamentMonster.cpp）
    void Clear();         // ORIG 0830e4bc
};

// ---- STMapScript 支撑类型（布局均由 ORIG 类型方法反汇编实证；定义见 STMapScript.cpp）----

// ORIG STAssignAnimation ctor 0x089e836c / Clear 0x089e832c（sizeof 0x14）。
// Import [animation]:name、getDrawLayer→m_layer(缺省1)、随后 3 个 int。
struct STAssignAnimation
{
    std::string m_name;    // +0x00（Clear 置 ""）
    int m_layer;           // +0x04 (Clear=1)
    int m_field08;         // +0x08 (Clear=0)
    int m_field0c;         // +0x0c (Clear=0)
    int m_field10;         // +0x10 (Clear=0)

    STAssignAnimation();
    void Clear();
};

// ORIG STQuestAnimation ctor 0x089e83d2 / Clear 0x089e83ae（Clear 先调基类 Clear；
// sizeof 0x1c）。Import [quest animation]:基类字段后跟 4 int + 1 bool。
struct STQuestAnimation : STAssignAnimation
{
    int m_field14;   // +0x14 (Clear=0)
    bool m_flag18;   // +0x18 (Clear=true)

    STQuestAnimation();
    void Clear();
};

// ORIG BackAniScript ctor 0x089e8898 / clear 0x089e884a（sizeof 0x14）。
// Import [ani info] 段:[filename]/[order]([below]=0/[upper]=1)/[loop y axis]/
// [screen pos](两 int,-1 缺省)。
struct BackAniScript
{
    std::string m_filename;  // +0x00
    int m_layer;             // +0x04 (clear=1)
    bool m_orderUpper;       // +0x08 (clear=false)
    bool m_loopYAxis;        // +0x09 (clear=false)
    bool m_hasScreenPos;     // +0x0a (clear=false)
    int m_screenX;           // +0x0c (clear=0)
    int m_screenY;           // +0x10 (clear=0)

    BackAniScript();
    void clear();
};

// ORIG BackGroundEffect ctor 0x089e8936 / clear 0x089e88da（sizeof 0x20）。
// Import [background effect]:[swing](type=0,layer+5int+可选动画名) /
// [dust](type=1,layer+3int)。
struct BackGroundEffect
{
    int m_type;             // +0x00 (clear=2)
    int m_layer;            // +0x04 (clear=7)
    int m_field08;          // +0x08 (clear=-1;dust 分支不写)
    int m_field0c;          // +0x0c (clear=-1)
    int m_field10;          // +0x10 (clear=-1)
    int m_field14;          // +0x14 (clear=-1)
    int m_field18;          // +0x18 (clear=-1)
    std::string m_animName; // +0x1c（仅 swing 分支非空 token 时赋值）

    BackGroundEffect();
    void clear();
};

// ORIG BackGroundEffectBlock ctor 0x089e897e / clear 0x089e89c6（sizeof 0x10）。
// Import [block effect]:[swing]=0/[dust]=1 → m_type;[object](int)或
// [animation](名字) → m_mode/m_anim 或 m_val。
struct BackGroundEffectBlock
{
    int m_type;             // +0x00 (clear=2)
    int m_mode;             // +0x04 (clear=2;object/animation 选择)
    int m_val;              // +0x08 (clear=0;object 分支的 int)
    std::string m_animName; // +0x0c（animation 分支的名字）

    BackGroundEffectBlock();
    void clear();
};

// ORIG STDialog ctor 0x089e8a1c / clear 0x089e8a5e（sizeof 0x8）。
// Import [dialog]:名字串 + 子 token on start/on complete 置对应标志。
struct STDialog
{
    std::string m_name;       // +0x00
    bool m_onStart;           // +0x04
    bool m_onComplete;        // +0x05
    bool m_flag06;            // +0x06（clear 一并清零，用途未证）

    STDialog();
    void clear();
};

// ORIG TournamentStartPos ctor 0x089e8a94 / dtor 0x089f1fd0（sizeof 0xc）。
// Import [tournament start area]:slot(1..2) 决定进 vec@0x2d0 / vec@0x2dc。
struct TournamentStartPos
{
    int m_x;     // +0x00
    int m_y;     // +0x04
    int m_slot;  // +0x08（1=第一向量 / 2=第二向量）

    TournamentStartPos() { m_x = 0; m_y = 0; m_slot = 0; }
};

// ORIG StaticStartPos ctor 0x089e8ab6 / dtor 0x089f21ec（sizeof 0x10）。
// Import [static player start pos]:3 个坐标 int + player 序号([player N]-1)。
struct StaticStartPos
{
    int m_x;          // +0x00
    int m_y;          // +0x04
    int m_z;          // +0x08
    int m_playerIdx;  // +0x0c

    StaticStartPos() { m_x = 0; m_y = 0; m_z = 0; m_playerIdx = 0; }
};

// Export [item] 段生成信息对（ORIG vector<STItemGenerateInfo> 平凡元素，
// Export 按 +0x00/+0x04 读出 '%d %d'）。
struct STItemGenerateInfo
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
};

// ORIG STAssignItem copy 0x083ef750 / dtor 0x083b6a8c（9 个 int 拷贝 +
// vector<STItemGenerateInfo>@0x24；sizeof 0x30）。当前脚本无 [item] 导入段，
// 仅 Export 输出与 [special passive object] 的临时拷贝使用。
struct STAssignItem
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
    int m_field08;   // +0x08
    int m_field0c;   // +0x0c
    int m_field10;   // +0x10
    int m_field14;   // +0x14
    int m_field18;   // +0x18
    int m_field1c;   // +0x1c
    int m_field20;   // +0x20
    std::vector<STItemGenerateInfo> m_generateInfos;  // +0x24

    STAssignItem(const STAssignItem& other);   // ORIG 0x083ef750
    ~STAssignItem();                           // ORIG 0x083b6a8c（仅析构 +0x24 向量）
    // ORIG 无默认 ctor / operator= / clear 符号；其余特殊成员由编译器隐式生成。
};

// ---- STMapScript（sizeof 0x37c；ctor 0x0836cc94/dtor 0x0836d4bc/
// copy 0x0837e698/Clear 0x0836c688 全量转写见 STMapScript.cpp）----
 struct STMapScript
 {
     STMapScript();
     STMapScript(const STMapScript& other);
     ~STMapScript();
     void Clear();
 
    int m_index;                       // +0x000（Clear=0；Import 收尾 = 文件表索引）
    std::list<int> m_list4;            // +0x004（[dungeon] 段整数列表）
    int m_c;                           // +0x00c（[type] normal/boss/dummy）
    std::vector<std::pair<int,int> > m_recoveryObjects; // +0x010（[camera edge recovery object]）
    std::list<STAssignAnimation> m_listAnimation;        // +0x01c（[animation]）
    std::list<STQuestAnimation> m_listQuestAnimation;    // +0x024（[quest animation]）
    std::list<STAssignPassiveObject> m_listPassiveObject; // +0x02c（[passive object]）
    std::vector<STAssignMonster> m_vecMonster;           // +0x034（[monster]）
    std::vector<STAssignMonster> m_vecConditionMonster;  // +0x040（[monster condition]）
    int m_championRate;                // +0x04c（[champion]）
    std::list<STAssignItem> m_listItem;                  // +0x050（Export [item]）
    std::list<STAssignSpecialPassiveObj> m_listSpecialPassive; // +0x058（[special passive object]）
    std::vector<AssignNpc> m_vecNpc;                     // +0x060（[NPC]）
    int m_element;                     // +0x06c（Clear=4；[elemental property] getElement）
    std::vector<std::string> m_vecTile;                  // +0x070（[tile]）
    std::vector<std::string> m_vecExtendedTile;          // +0x07c（[extended tile]）
    int m_sightScroll[6];              // +0x088..0x9f（Clear 各=100；前三个为
                                       //   far/middle/near sight scroll，其余未用）
    std::vector<BackAniScript> m_vecBackAni;             // +0x0a0（[ani info]…[/background animation]）
    std::vector<BackGroundEffect> m_vecBackGroundEffect; // +0x0ac（[background effect]）
    std::vector<BackGroundEffectBlock> m_vecBlockEffect; // +0x0b8（[block effect]）
    std::vector<STDialog> m_vecDialog;                   // +0x0c4（[dialog]）
    int m_startPathDir;                // +0x0d0（Clear=4；[absolute start path] left/right/up/down）
    char m_blockPath[4];               // +0x0d4（[block path] 各方向 close/open）
    char m_tileOptionUpper;            // +0x0d8（[tile option][upper]）
    std::vector<TownPath> m_vecTownMovableArea;          // +0x0dc（[town movable area]）
    std::vector<MapArea> m_vecVirtualMovableArea;        // +0x0e8（[virtual movable area]）
    std::vector<TownPath> m_vecGuildAgitEntrance;        // +0x0f4（[guild agit entrance info]）
    int m_pathgatePos[8];              // +0x100（[pathgate pos] 4×(x,y)）
    int m_pathgateRange[8];            // +0x120（[pathgate recognize range] 4×(x,y)）
    MapArea m_dungeonStartArea;        // +0x140（[dungeon start area]，含 vector 成员）
    MapArea m_summonStartArea;         // +0x15c（[summon start area]）
    int m_178;                         // +0x178（Clear=0/copy 复制；set_map 读取 → CMap::m_110）
    int m_17c;                         // +0x17c（Clear=0/copy 复制；用途未证）
    std::vector<std::pair<int,int> > m_vec180;           // +0x180（copy 复制；无导入/导出段，推断为运行时暂存）
    std::string m_openingBgm;          // +0x18c（[opening bgm]）
    int m_openingBgmParam;             // +0x190（[opening bgm] 第二个 int）
    std::vector<std::string> m_vecSound;                 // +0x194（[sound]）
    std::string m_str1a0;              // +0x1a0（Clear 置 ""）
    std::string m_str1a4;              // +0x1a4（[show dust] 第一个子串 append）
    std::string m_str1a8;              // +0x1a8（[show dust] 第二个子串 append）
    char m_sortBottom;                 // +0x1ac（[sort bottom]==1）
    std::string m_str1b0;              // +0x1b0（Clear 赋 "PVP놀이게임" 后又清空 → ""；
                                       //   Import [map name] ScanStr 直接赋值）
    int m_playerCountMin;              // +0x1b4（Clear=2；[player number] 第一个 int）
    int m_playerCountMax;              // +0x1b8（Clear=8；[player number] 第二个 int）
    MapArea m_pvpStartArea[5];         // +0x1bc..0x247（[pvp start area] 填 0..2、
                                       //   [pvp practice start area] 填 3..4；
                                       //   未读 practice 时收尾把 [1]/[2] 复制到 [3]/[4]）
    char m_pvpByPartySlot;             // +0x248（[pvp start area by party slot]!=0）
    char m_keepStateAxisX;             // +0x249（[map move keep state axis pos]）
    char m_keepStateAxisY;             // +0x24a
    char m_keepStateAxisZ;             // +0x24b
    int m_24c;                         // +0x24c（Clear=0；[shadow type]：[mirror]→1 / [more darker]→2）
    std::vector<STRivalMapData> m_vecMonsterSpawnPos;    // +0x250（[monster spawn pos]）
    std::vector<STAICharacterMapArrangeData> m_vecAICharacter; // +0x25c（[ai character]）
    int m_apcRandomPoint;              // +0x268（[apc random point]）
    std::string m_mapLoadingImagePath; // +0x26c（[map loading image path]）
    int m_270;                         // +0x270（Clear=0/copy 复制；用途未证）
    int m_274;                         // +0x274（Clear=0/copy 复制；用途未证）
    std::vector<STRivalMapData> m_vecEventMonsterPos;    // +0x278（[event monster position]）
    int m_maxMonsterDropException;     // +0x284（[heroes mode map index] 与主链默认整数行；命名沿用旧版待证）
    std::vector<STAssignBloodSystem> m_vecBloodSystem;   // +0x288（[blood monster]/[ultimate monster]）
    std::vector<STAssignBloodPhaseTime> m_vecBloodPhaseTime; // +0x294（[blood phase time]/[ultimate phase time]）
    std::vector<STAssignDefenseSystem> m_vecDefenseSystem; // +0x2a0（[defense monster]）
    std::vector<STAssignDefensePhaseTime> m_vecDefensePhaseTime; // +0x2ac（[defense phase time]）
    std::map<char, std::vector<TournamentMonster> > m_tournamentMonsters; // +0x2b8（[tournament monster]）
    std::vector<TournamentStartPos> m_tournamentStartPos1; // +0x2d0（slot==1）
    std::vector<TournamentStartPos> m_tournamentStartPos2; // +0x2dc（slot==2）
    std::vector<StaticStartPos> m_vecStaticStartPos;     // +0x2e8（[static player start pos]）
    std::vector<StaticStartPos> m_vecStaticStartPos2;    // +0x2f4（ctor 存在；无导入段）
    std::map<int, MapArea> m_limitPlayerMoveAreas;       // +0x300（[limit player move area]）
    std::vector<STAttackedMonster> m_vecAttackedMonster; // +0x318（[attacked monster info]）
    int m_maxMonsterCountException;    // +0x324（Import [max monster drop exception] 写；命名沿用旧版待证）
    int m_328;                         // +0x328（Clear=0/copy 复制；Import [max monster count exception] 写）
    int m_32c;                         // +0x32c（Clear=0/copy 复制；用途未证）
    advancealtar::TimeLine m_timeLine; // +0x330（[time line] importPhase 目标）
    std::string m_str34c;              // +0x34c（[map dialog] ScanStr；copy 复制）
    int m_moveLayeredType;             // +0x350（Clear=4；[move layered map] left/right/up/down 同 start path 编码）
    int m_zposUnder;                   // +0x354（Clear=-1；[zpos under]）
    char m_allDeadCasePassable;        // +0x358（[all dead case passable]）
    int m_screenEdgeLeft;              // +0x35c（Clear=-1；[customized screen edge][left]）
    int m_screenEdgeRight;             // +0x360（Clear=-1；[right]）
    int m_screenEdgeTop;               // +0x364（Clear=-1；[top]）
    int m_screenEdgeBottom;            // +0x368（Clear=-1；[bottom]）
    char m_blockUseActiveSkill;        // +0x36c（[block use active skill]）
    char m_blockUseStackableItem;      // +0x36d（[block use stackable item]）
    char m_fixChampion;                // +0x36e（[fix champion]）
    char m_pad36f;                     // +0x36f
    std::string m_str370;              // +0x370（Clear 清空；[basic action] ScanStr；copy 复制）
    int m_visibleMinimap;              // +0x374（Clear=1；[visible minimap]）
    int m_visibleTownMinimap;          // +0x378（Clear=1；[visible town minimap]）
 };

// ---- CMap（sizeof 0x130，无虚表）----
class CMap
{
public:
    CMap();
    ~CMap();
    void destroy();
    void destory();
    int set_map(STMapScript script);
    int insert_monster(STAssignMonster monster, int area);
    void insert_condition_monster(STAssignMonster monster);
    int insert_item(int idx, int level, int prob, int cond);
    bool checkFitInPathObject(int x, int y) const;
    int box_item_index(std::list<std::pair<int, int> > itemList) const;
    int CheckEntrance(int flag) const;

    int get_index() const { return m_4; }
    int get_map_type() const { return m_8; }
    std::list<_mapMonster> get_monster_list() const { return m_monsterList; }
    std::list<_mapMonster> get_condition_monster_list() const
    {
        return m_conditionMonsterList;
    }
    std::list<_mapItem> get_item_list() const { return m_itemList; }
    int GetBossCount() const { return m_bossCount; }
    int getAssignedNPCCount() const { return m_assignedNpcCount; }
    const std::vector<STAICharacterMapArrangeData>* getAICharacters() const
    {
        return &m_aiCharacters;
    }
    int getAPCRandomPoint() const { return m_78; }
    std::list<int>* getDungeonLists() const
    {
        return const_cast<std::list<int>*>(&m_pathObjectList);
    }

    int m_bossCount;                // +0x00  (monster type==3 时累加)
    int m_4;                        // +0x04  (map index)
    int m_8;                        // +0x08  (map type)
    std::list<_mapMonster> m_monsterList;       // +0x0c
    std::list<_mapMonster> m_conditionMonsterList; // +0x14
    std::list<_mapItem> m_itemList; // +0x1c
    std::list<int> m_pathObjectList; // +0x24
    int m_pathArea1[8];             // +0x2c
    int m_pathArea2[8];             // +0x4c
    std::vector<STAICharacterMapArrangeData> m_aiCharacters; // +0x6c
    int m_78;                       // +0x78  (APC random point)
    int m_assignedNpcCount;         // +0x7c
    int m_80;                       // +0x80
    std::vector<std::pair<int, int> > m_entranceVec; // +0x84
    std::vector<STRivalMapData> m_rivalData1;  // +0x90
    std::vector<STRivalMapData> m_rivalData2;  // +0x9c
    std::vector<int> m_passiveObjectList;      // +0xa8
    int m_b4;                       // +0xb4
    std::vector<STAssignBloodSystem> m_bloodSystem; // +0xb8
    std::vector<STAssignBloodPhaseTime> m_bloodPhaseTime; // +0xc4
    char m_padd0[0xec - 0xd0];      // +0xd0
    int m_ec;                       // +0xec
    int m_f0;                       // +0xf0
    advancealtar::TimeLine m_timeLine; // +0xf4
    int m_110;                      // +0x110
    std::map<char, std::vector<TournamentMonster> > m_tournamentMap; // +0x114
    bool m_12c;                     // +0x12c（ORIG getAddChampionMobCnt 083010ee 以
                                    //   movzbl/xor $1/test 布尔判反读取 → bool 非 char；
                                    //   set_dungeon 自 script.m_36e 复制，仅作标志）
};
