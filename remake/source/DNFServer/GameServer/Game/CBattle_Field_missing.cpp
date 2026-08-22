// df_game_r CBattle_Field 缺失成员函数实现（新建 TU，宿主登记 CMake）。
//
// 本 TU 实现 CBattle_Field 的 5 个成员函数，取代 GameStubs.cpp 的 asm 桥接
// （宿主登记新 TU 后应删除 GameStubs.cpp:859-869 的 5 个 extern asm 别名 +
// 5 个空实现，避免重复符号 _ZN13CBattle_Field*）。CParty.cpp:45-52 仅是对这
// 些 mangled 符号的 extern 声明（无定义），无需改动，将本 TU 的真实定义解析。
//
// 依据：
//   - ctor  0x082a44fe `_ZN13CBattle_FieldC1Ev`      （子对象构造序 + 标量初值）
//   - dtor  0x082a499c `_ZN13CBattle_FieldD1Ev`      （子对象析构序 = 构造逆序）
//   - pickup_item          0x0830daee  docs/class_func_reports/CBattle_Field/pickup_item.md
//   - reset_field          0x0830a1f6  docs/class_func_reports/CBattle_Field/reset_field.md
//   - reset_hell_party_value 0x0830a57c docs/class_func_reports/CBattle_Field/reset_hell_party_value.md
//
// 说明：
//   * 构造/析构的成员顺序依赖编译器按成员声明序执行的默认行为；内嵌的
//     CBloodRound / CBloodClearRewardData 在 ORIG 与当前布局下均为平凡析构
//     （Inven_Item 无析构函数），ORIG dtor 不为其发出析构调用，故 dtor 无需显式处理。
//   * reset_hell_party_value 中分母 0x08c23d74 == 10000.0f（ORIG .rodata 实证），
//     脚本值经 G_CDataManager()->get_hellparty_script_values() 取得（float 数组）。

#include "CBattle_Field.h"

// ================= CBattle_Field::CBattle_Field() — ORIG 0x082a44fe =================
//
// 子对象（pair[2..9]、map/vector/Mutex/CRidable/CClearCondition/嵌套奖励等）
// 由成员声明序自动构造；此处仅复刻 ORIG 的标量初值与容器清理操作。
CBattle_Field::CBattle_Field()
{
    // —— 位置链：起点/终点槽对夫妇构完后按 ORIG 置初值 ——
    // ORIG：m_lastY(+0x6c) = -1，再沿 +0x68/+0x64/+0x60/+0x18/+0x14/+0x10/+0x0c/
    // +0x08/+0x04 级联，故 m_startX/m_startY、m_curX/m_curY、m_lastX/m_lastY、
    // pair[0].first/.second、pair[1].first/.second 均为 -1。
    m_startX = -1;
    m_startY = -1;
    m_lastY  = -1;
    m_lastX  = -1;
    m_curY   = -1;
    m_curX   = -1;
    m_pos[0] = std::make_pair(-1, -1);   // +0x0c
    m_pos[1] = std::make_pair(-1, -1);   // +0x14
    // ORIG 用 pair ctor 构造 pair[2..9]（默认 0）后再置 0；此处复刻最终值。
    for (int i = 2; i < 10; ++i)
        m_pos[i] = std::make_pair(0, 0);

    // —— 标量初值（逐字段对应 ORIG ctor 汇编）——
    m_party            = NULL;              // +0x00  movl $0x0,(%eax)
    m_firstEnterDungeon = 1;                // +0x5c  movb $0x1,0x5c
    m_mapHeight        = 0;                 // +0x190
    m_mapWidth         = m_mapHeight;       // +0x18c = (+0x190)
    m_ultimateDifficulty = 0;               // +0x2ab
    m_villageMonsterId = 0;                 // +0x128  movw $0x0（ushort）
    m_hellPartyLevel   = 0;                 // +0x238
    m_bloodState       = 0;                 // +0x1b4
    m_worldMapTowerArea = 0;                // +0x23c

    // —— 容器清理 ——
    m_tournamentRoundMap.clear();           // +0x13c
    m_tournamentReward.ResetReward();       // +0xc98
    m_tournamentCurRound = 0;               // +0x154
    m_layeredMapIndex    = -1;              // +0x10c
    m_questMaze          = 0;               // +0x110
    SetCurSeatNo(0xff);                     // 082a43cc SetCurSeatNo(0xff)
}

// ================= CBattle_Field::~CBattle_Field() — ORIG 0x082a499c =================
//
// ORIG 析构顺序 = 构造逆序（与成员声明序相反，编译器默认执行）：
//   +0xc98 CTournamentDungeonReward → +0xc84 CTowerOfDespairRewardData →
//   +0x264 vector<bool> → +0x258 vector<short> → +0x24c vector<STAssignBloodPhaseTime> →
//   +0x240 vector<STAssignBloodSystem> → +0x17c vector<stEventMonsterStatus_t> →
//   +0x170 vector<stConditionEnterBossRoomStatus_t> → +0x13c tournamentRoundMap →
//   +0x118 vector<int> apcList → +0xf0 CClearCondition → +0xd0 CRidable →
//   +0xc0 vector<pair> dummyMapPos → +0xa8 Mutex event → +0x90 Mutex field → +0x78 mapInfo。
// CBloodRound(+0x2ac)/CBloodClearRewardData(+0x2b8) 为平凡析构，ORIG 未对其发调用。
CBattle_Field::~CBattle_Field()
{
}

// ================= CBattle_Field::pickup_item(int) — ORIG 0x0830daee =================
// 在当前地图的 itemMap 中查找 uid；找到则 erase 并返回 0，否则返回 0x15(21)。
int CBattle_Field::pickup_item(int uid)
{
    MapInfo* mi = GetCurrentMapInfo();
    if (mi == NULL)
        return 0x15;

    std::map<int, map_item>::iterator it = mi->m_itemMap.find(uid);
    if (it == mi->m_itemMap.end())
        return 0x15;
    mi->m_itemMap.erase(it);
    return 0;
}

// ================= CBattle_Field::reset_field() — ORIG 0x0830a1f6 =================
// 全字段清零链，逐字段按 ORIG 汇编映射到 CBattle_Field.h 具名成员。
void CBattle_Field::reset_field()
{
    // —— 位置/尺寸链 ——
    m_dungeonIndex = 0;                     // +0x194
    m_mapHeight    = 0;                     // +0x190
    m_mapWidth     = m_mapHeight;           // +0x18c
    // 位置链：pair[1].second/+0x18 → ... → m_startX，最终全 0
    m_pos[1].second = 0;                    // +0x18
    m_pos[1].first  = 0;                    // +0x14
    m_curY          = 0;                    // +0x64
    m_curX          = 0;                    // +0x60
    m_pos[0].second = 0;                    // +0x10
    m_pos[0].first  = 0;                    // +0x0c
    m_startY        = 0;                    // +0x08
    m_startX        = 0;                    // +0x04
    for (int i = 0; i < 4; ++i)             // pair[2..5] 与 pair[6..9] 清零
    {
        m_pos[i + 2].first  = 0;
        m_pos[i + 2].second = 0;
        m_pos[i + 6].first  = 0;
        m_pos[i + 6].second = 0;
    }
    m_firstEnterDungeon = 1;                // +0x5c

    // —— 难度/迷宫链 ——
    m_dungeonDiff   = 0;                    // +0x1a0
    m_questStageMap = 0;                    // +0x19c = (+0x1a0)
    m_field198      = 0;                    // +0x198 = (+0x19c)
    m_maze          = NULL;                 // +0x70 = (+0x198)

    m_apcKillCount  = 0;                    // +0x1a8
    m_pDungeon      = NULL;                 // +0x188
    m_hellPartyMode = 0;                    // +0x1b0
    m_bloodState    = 0;                    // +0x1b4
    m_hellPartyActive = 0;                  // +0x1b8
    m_hellMapIdx    = -1;                   // +0x1bc
    m_hellPosX      = -1;                   // +0x1c0
    m_hellPosY      = -1;                   // +0x1c4
    m_visitedGotoFlag = 0;                  // +0x1c8

    m_ridable.Clear();                      // +0xd0  CRidable::Clear()

    m_field1c9          = 0;                // +0x1c9
    m_hellPartyRandom   = 0;                // +0x1ce
    m_hellPartyDropFlag = 0;                // +0x1cf
    std::memset(m_hellItemStatistic, 0, sizeof(m_hellItemStatistic)); // +0x1d0 (0x18)
    m_field1cb           = 0;               // +0x1cb
    m_hellPartyDifficulty = 0;              // +0x1cc
    m_killedAllHellGroups = 0;              // +0x1ca

    // —— 容器清空 ——
    m_mapInfo.clear();                      // +0x78
    m_bossRoomConditions.clear();           // +0x170
    m_apcList.clear();                      // +0x118
    m_field124 = 0;                         // +0x124
    m_eventMonsterStatus.clear();           // +0x17c

    // —— 血/奖励系统 ——
    reset_blood_values();                          // +0x00  reset_blood_values() 08306d86
    m_bloodRound.reset_blood_values();             // +0x2ac CBloodRound::reset_blood_values() 08306d66
    m_bloodState2 = 0;                             // +0x2b4
    m_bloodReward.reset();                         // +0x2b8 CBloodClearRewardData::reset() 08306ee0
    m_todReward.reset_tod();                       // +0xc84 CTowerOfDespairRewardData::reset_tod() 083071a6

    // —— 村庄怪物 ——
    if (m_villageMonsterId != 0)
    {
        GlobalData_s_villageMonsterMgr->OnKillVillageMonster(
            m_villageMonsterCount,          // +0x12c  arg1
            m_field130,                     // +0x130  arg2
            m_villageMonsterId,             // +0x128  arg3 (ushort)
            false);                         // arg4=false
    }
    m_villageMonsterId   = 0;               // +0x128
    m_villageMonsterCount = 0;              // +0x12c
    m_field130           = 0;               // +0x130
    m_field134           = 0;               // +0x134

    // —— 电梯/地狱 party/布局 ——
    m_elevatorState = 2;                    // +0x138
    m_elevatorTimer = 0;                    // +0x139
    m_elevatorFlag  = 1;                    // +0x13a
    m_hellPartyEventType = 0;               // +0x1cd
    m_worldMapTowerArea  = 0;               // +0x23c
    m_layeredMapIndex    = -1;              // +0x10c
    m_questMaze          = 0;               // +0x110
    SetCurSeatNo(0xff);                     // 082a43cc SetCurSeatNo(0xff)
}

// ================= CBattle_Field::reset_hell_party_value() — ORIG 0x0830a57c =================
// 从 CDataManager 的 hellparty 脚本值（float 数组）拷贝各组比例；分母 10000.0f。
void CBattle_Field::reset_hell_party_value()
{
    const float* s = static_cast<const float*>(
        G_CDataManager()->get_hellparty_script_values());

    // —— 固定比例字段 ——
    m_hellPartyRatio0 = s[0];               // +0x1e8 = script[0]
    m_hellPartyRatio1 = s[1];               // +0x1ec = script[4]
    m_hellPartyRatio3 = s[3];               // +0x1f4 = script[0xc]
    m_hellPartyRatio4 = s[4] / 10000.0f;    // +0x1f8 = script[0x10]/10000
    m_hellPartyRatio5 = s[5] / 10000.0f;    // +0x1fc = script[0x14]/10000
    m_hellPartyScriptRate = s[19] / 10000.0f; // +0x234 = script[0x4c]/10000

    // —— m_hellPartyDungeonRate[4]（+0x200）= script[6..9]/10000 ——
    for (int i = 0; i < 4; ++i)
        m_hellPartyDungeonRate[i] = s[6 + i] / 10000.0f;

    // —— m_hellPartyMapRate[5]（+0x210）= script[10..14]/10000 ——
    for (int i = 0; i < 5; ++i)
        m_hellPartyMapRate[i] = s[10 + i] / 10000.0f;

    // —— m_hellPartyItemRate[4]（+0x224）= script[15..18]/10000 ——
    for (int i = 0; i < 4; ++i)
        m_hellPartyItemRate[i] = s[15 + i] / 10000.0f;
}