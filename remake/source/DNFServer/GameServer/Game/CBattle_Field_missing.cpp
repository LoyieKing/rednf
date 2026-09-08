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
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CVillageMonsterMgr.h"   // 完整类型（reset_field 调用 OnKillVillageMonster 4 参重载）
#include "LogManager.h"          // logFormat（血轮日志）
#include "TimerQueue.h"          // G_TimerQueue / InsertTimerInMilisecond
#include "GameRand.h"           // get_rand_int

// ================= CBattle_Field::CBattle_Field() — ORIG 0x082a44fe =================
//
// 子对象（pair[2..9]、map/vector/Mutex/CRidable/CClearCondition/嵌套奖励等）
// 由成员声明序自动构造；此处仅复刻 ORIG 的标量初值与容器清理操作。
CBattle_Field::CBattle_Field()
{
    // [b28d-39 判词 · N82/N93] ORIG 0x082a44fe：ORIG 数组构造 3 元素×0x1c 步进、
    // 4.4 为 9 元素×0xc 步进（成员数组建模粒度差）+ 帧 0x20 vs 0x50；
    // 逐字段写入集合一致，语义等价（N82 槽族）。
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
    m_standardDimensionLevel   = 0;                 // +0x238
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

// ================= CBattle_Field::GetEnableHellDungeon() — ORIG 0822cf26 =================
// 定义置于本 TU（与调用者分离）：4.4 unit-at-a-time 对 TU 内可见 callee 省略
// outgoing 帧（sub $0x10/槽-0x4），ORIG 4.1 恒发 sub $0x18/槽-0xc（探针 p6/p7）。
char* CBattle_Field::GetEnableHellDungeon()
{
    // mov $0x93701ec,%eax; ret（POD 无 guard 的函数局部
    // static，V 符号 _ZZN13CBattle_Field20GetEnableHellDungeonEvE19enable_hell_dungeon）。
    static char enable_hell_dungeon;
    return &enable_hell_dungeon;
}

// ================= CBattle_Field::pickup_item(int) — ORIG 0x0830daee =================
// 在当前地图的 itemMap 中查找 uid；找到则 erase 并返回 0，否则返回 0x15(21)。
int CBattle_Field::pickup_item(int uid)
{
    MapInfo* mi = GetCurrentMapInfo();
    // ORIG 0x0830daee：iterator 先默认构造（C1Ev），find() 结果经独立 sret
    // 临时 -0x1c 中转再拷贝入 it@-0x14（P28 先声明后赋值；GetFieldMob 同法实证）。
    std::map<int, map_item>::iterator it;
    it = mi->m_itemMap.find(uid);
    if (it == mi->m_itemMap.end())
        return 0x15;
    mi->m_itemMap.erase(it);
    return 0;
}

// ================= CBattle_Field::reset_field() — ORIG 0x0830a1f6 =================
// 全字段清零链，逐字段按 ORIG 汇编映射到 CBattle_Field.h 具名成员。
void CBattle_Field::reset_field()
{
    // [b28d-39 判词 · N4/P30] ORIG 0x0830a1f6：残余仅循环内 4 处
    // `mov idx,%edx; add $K,%edx` vs 4.4 `mov idx,%eax; lea K(%eax),%edx`
    // （add-vs-lea 两步寻址固有差，P30 穷举到顶）；语义逐条等价。
    // —— 位置/尺寸链 ——
    m_dungeonIndex = 0;                     // +0x194
    m_mapHeight    = 0;                     // +0x190
    m_mapWidth     = m_mapHeight;           // +0x18c
    // 位置链：pair[1].second/+0x18 → ... → m_startX，最终全 0。
    // ORIG 为链式复制（0x18→0x14→0x64→0x60→0x10→0xc→0x8→0x4），多重赋值复现。
    m_startX = m_startY = m_pos[0].first = m_pos[0].second = m_curX =
        m_curY = m_pos[1].first = m_pos[1].second = 0;
    for (int i = 0; i < 4; ++i)             // pair[2..5] 与 pair[6..9] 清零
    {
        m_pos[i + 2].first  = 0;
        m_pos[i + 2].second = 0;
        m_pos[i + 6].first  = 0;
        m_pos[i + 6].second = 0;
    }
    m_firstEnterDungeon = 1;                // +0x5c

    // —— 难度/迷宫链 ——
    // ORIG 链式复制（0x1a0→0x19c→0x198→0x70），多重赋值复现。
    m_mapMobCount = m_field198 = m_questStageMap = m_dungeonDiff = 0;

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

    m_sealDoorItemsOk          = 0;                // +0x1c9
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
    m_apcAllKilledSec = 0;                         // +0x124
    m_eventMonsterStatus.clear();           // +0x17c

    // —— 血/奖励系统 ——
    reset_blood_values();                          // +0x00  reset_blood_values() 08306d86
    m_bloodRound.reset_blood_values();             // +0x2ac CBloodRound::reset_blood_values() 08306d66
    m_bloodState2 = (ENUM_BLOOD_STATE)0;           // +0x2b4
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
    // [b28d-39 判词 · N4/N107] ORIG 0x0830a57c：残余为成员偏移取址
    // `mov 0x4(%eax),%edx`（4.1 折叠）vs 4.4 `add $0x4,%eax; mov (%eax),%edx`
    // （N107 成员常量偏移 4.4 恒 add）；语义等价。
    // ORIG（reset_hell_party_value.md 0x0830a583..0x0830a6f7）P48 no-CSE：
    // 每个字段/数组元素各自 fresh G_CDataManager()->get_hellparty_script_values()，
    // 共 9 对 G_CDataManager/get_hellparty_script_values 调用（6 固定 + 3 循环整组）。
    // 前三个字段为整数 mov 拷贝 dword（script[0]/[+4]/[+0xc]），字段是 int；
    // 以 reinterpret 位拷贝保持 mov 语义（float→int 转换会引入 flds/fistp）。

    // —— 固定比例字段 ——
    m_hellPartyRatio0 = reinterpret_cast<const int*>(
        G_CDataManager()->get_hellparty_script_values())[0];   // +0x1e8
    m_hellPartyRatio1 = reinterpret_cast<const int*>(
        G_CDataManager()->get_hellparty_script_values())[1];   // +0x1ec
    m_hellPartyRatio3 = reinterpret_cast<const int*>(
        G_CDataManager()->get_hellparty_script_values())[3];   // +0x1f4
    m_hellPartyRatio4 = static_cast<const float*>(
        G_CDataManager()->get_hellparty_script_values())[4] / 10000.0f;  // +0x1f8
    m_hellPartyRatio5 = static_cast<const float*>(
        G_CDataManager()->get_hellparty_script_values())[5] / 10000.0f;  // +0x1fc
    m_hellPartyScriptRate = static_cast<const float*>(
        G_CDataManager()->get_hellparty_script_values())[19] / 10000.0f; // +0x234

    // —— m_hellPartyDungeonRate[4]（+0x200）= script[6..9]/10000 ——
    for (int i = 0; i < 4; ++i)
        m_hellPartyDungeonRate[i] = static_cast<const float*>(
            G_CDataManager()->get_hellparty_script_values())[6 + i] / 10000.0f;

    // —— m_hellPartyMapRate[5]（+0x210）= script[10..14]/10000 ——
    for (int i = 0; i < 5; ++i)
        m_hellPartyMapRate[i] = static_cast<const float*>(
            G_CDataManager()->get_hellparty_script_values())[10 + i] / 10000.0f;

    // —— m_hellPartyItemRate[4]（+0x224）= script[15..18]/10000 ——
    for (int i = 0; i < 4; ++i)
        m_hellPartyItemRate[i] = static_cast<const float*>(
            G_CDataManager()->get_hellparty_script_values())[15 + i] / 10000.0f;
}
// ============================================================================
// == 血轮（Blood Dungeon）链 —— ORIG 报告逐指令转写（2026-08-25 批次）      ==
// 符号清单：/tmp/df_batches/undef_link_syms_2026-08-25.txt 中 CBattle_Field 域；
// 依赖血轮辅助函数（同报告）一并实现，报告均在 docs/class_func_reports/CBattle_Field/。
// ============================================================================

// ---------------------------------------------------------------------------
// CBattle_Field::isQuestMaze —— ORIG T 0x0830e31e：返回 m_questMaze(+0x110)。
// ---------------------------------------------------------------------------
bool CBattle_Field::isQuestMaze()
{
    return m_questMaze;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_cnt_of_phase(short smallRound, short phase)
//   —— ORIG T 0x08306a62：阶段生成数 = STAssignBloodSystem.m_startCnt +
//      phase * m_cntInc（随机阶段经 m_bloodPhaseOrder 回退）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_cnt_of_phase(short smallRound, short phase)
{
    // [b28d-39 判词 · N82/N98] ORIG 0x08306a62：短整守卫/正写 size 比较已对齐，
    // 残余为具名 int cnt(-0x10) 预清位置与槽序（4.4 按首次使用分配，N98），
    // 帧 0x38 vs 0x34 属 N82 槽族；语义逐条等价。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x08306a62：`(short)size()` 与 short 形参 16 位比较
    // （cmp -0x1c(%ebp),%ax），避免 4.4 为 size_t 提升把 movswl 值缓存进 %ebx。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // [b28d-39] ORIG 0x08306a62：sysIdx 先声明（-0xc）、cnt 次声明（-0x10，
    // 预清 `movl $0x0,-0x10` 在 sysIdx 赋值前）；4.4 首声明占浅槽 ⇒ 此序复现。
    unsigned int sysIdx;
    int cnt = 0;
    sysIdx = (unsigned int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == 0xffffffffu)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (unsigned int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-39] ORIG 0x08306a62：`cmp -0xc(%ebp),%eax; setle` ⇒ 源为
    // `(int)size() <= (int)sysIdx` 正写（负写 sysIdx<size 会引 %ebx 缓存）。
    if ((int)m_bloodSystems.size() <= (int)sysIdx)
        return -1;
    const STAssignBloodSystem& sys = m_bloodSystems[sysIdx];
    for (int p = 0; p < (int)phase + 1; ++p)
        cnt = sys.m_startCnt + sys.m_cntInc * p;
    return cnt;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_time_of_phase(short smallRound)
//   —— ORIG T 0x0830690a：阶段时间 = STAssignBloodSystem.m_timeB（+0x18）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_time_of_phase(short smallRound)
{
    // [b28d-39 判词 · N82/N98] ORIG 0x0830690a：同上（cnt/result 预清位置 +
    // 槽序跨版本差）；语义逐条等价。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x0830690a：短整比较形（同 get_blood_spawn_cnt_of_phase）。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // sysIdx(-0xc) 先声明、result(-0x10, 预清 0) 次声明（ORIG 槽序）。
    unsigned int sysIdx;
    int result = 0;
    sysIdx = (unsigned int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == 0xffffffffu)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (unsigned int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-39] ORIG 0x0830690a：`cmp -0xc(%ebp),%eax; setle` 正写形（同上）。
    if ((int)m_bloodSystems.size() <= (int)sysIdx)
        return -1;
    result = m_bloodSystems[sysIdx].m_timeB;
    return result;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_phase_total_cnt_of_small_round(short)
//   —— ORIG T 0x08306c0e：阶段总数 = STAssignBloodSystem.m_totalCnt（+0x24）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_phase_total_cnt_of_small_round(short smallRound)
{
    // [b28d-39 判词 · N82/N98] ORIG 0x08306c0e：同 get_blood_spawn_time_of_phase。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x08306c0e：短整比较形（同 get_blood_spawn_cnt_of_phase）。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // sysIdx(-0xc) 先声明、result(-0x10, 预清 0) 次声明（ORIG 槽序）。
    unsigned int sysIdx;
    int result = 0;
    sysIdx = (unsigned int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == 0xffffffffu)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (unsigned int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-39] ORIG 0x08306c0e：`cmp -0xc(%ebp),%eax; setle` 正写形（同上）。
    if ((int)m_bloodSystems.size() <= (int)sysIdx)
        return -1;
    result = m_bloodSystems[sysIdx].m_totalCnt;
    return result;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_time_of_next_small_round(short)
//   —— ORIG T 0x0830676a：下一小轮时间 = m_timeA - m_totalCnt * m_timeB。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_time_of_next_small_round(short smallRound)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x0830676a：帧 push esi/ebx 与具名 result
    // 槽序跨版本相反（N93）；语义逐条等价。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x0830676a：短整比较形（同 get_blood_spawn_cnt_of_phase）。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // sysIdx(-0xc) 先声明、result(-0x10, 预清 0) 次声明（ORIG 槽序）。
    unsigned int sysIdx;
    int result = 0;
    sysIdx = (unsigned int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == 0xffffffffu)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (unsigned int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-39] ORIG 0x0830676a：`cmp -0xc(%ebp),%eax; setle` 正写形（同上）。
    if ((int)m_bloodSystems.size() <= (int)sysIdx)
        return -1;
    const STAssignBloodSystem& sys = m_bloodSystems[sysIdx];
    result = sys.m_timeA - sys.m_totalCnt * sys.m_timeB;
    return result;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_round_of_small_round(short)
//   —— ORIG T 0x083065f8：小轮对应大轮 = phaseTime.m_smallRoundOrder。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_round_of_small_round(short smallRound)
{
    // [b28d-39] ORIG 0x083065f8：`||` 短路链（empty / `(short)size()<=smallRound` /
    // `smallRound<0`）汇到 invalid 岛，`mov $1/%eax`+`mov $0` 物化后 test；具名 int
    // result（尾 mov %eax,-0xc; mov -0xc,%eax）。
    if (m_bloodPhaseTimes.empty() ||
        smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int result = m_bloodPhaseTimes[smallRound].m_smallRoundOrder;
    return result;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_round_difficylty_of_small_round(short)
//   —— ORIG T 0x083062fe：小轮难度 = phaseTime.m_difficulty（+0x18 char）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_round_difficylty_of_small_round(short smallRound)
{
    // [b28d-39] ORIG 0x083062fe：同 get_blood_round_of_small_round 的 `||` 短路形。
    if (m_bloodPhaseTimes.empty() ||
        smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int result = (int)(signed char)m_bloodPhaseTimes[smallRound].m_difficulty;
    return result;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_small_round_interval(short)
//   —— ORIG T 0x083066ea：小轮间隔 = phaseTime.m_interval（+0x08）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_small_round_interval(short smallRound)
{
    // [b28d-39] ORIG 0x083066ea：empty() 走独立早返（`je` 后直落 size 检查），
    // 仅 size/负值两条件走 `||` 短路物化（与另三个 getter 的合并短路形不同）。
    if (m_bloodPhaseTimes.empty())
        return -1;
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int result = m_bloodPhaseTimes[smallRound].m_interval;
    return result;
}

// ---------------------------------------------------------------------------
// CBattle_Field::checkBloodSpawnOrder(short) —— ORIG T 0x08306e32：
//   小轮序在 [0,size) 且前 smallRound 个阶段均已清除（m_bloodPhaseClear 前段全 true）。
// ---------------------------------------------------------------------------
bool CBattle_Field::checkBloodSpawnOrder(short smallRound)
{
    // [b28d-39] ORIG 0x08306e32：先 `(short)size() < smallRound`（16 位 setl），
    // 再 empty()；具名 `int n=(int)smallRound`（-0x10）+ `int i`（-0xc）的
    // `i<n` 有符号循环。负数 smallRound 在非空时返 1（原码语义，照抄）。
    if ((short)m_bloodPhaseClear.size() < smallRound)
        return 0;
    if (m_bloodPhaseClear.empty())
        return 0;
    int n = (int)smallRound;
    for (int i = 0; i < n; ++i)
    {
        if (!m_bloodPhaseClear[i])
            return 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodMonsterSpawnPacket(int cnt, int time, short smallRound,
//                                        short nextRound)
//   —— ORIG T 0x08305992（_ZN13CBattle_Field23BloodMonsterSpawnPacketEiiss）：
//   遍历当前图怪物表，收集 bloodSmallRoundOrder==smallRound 且未参战（killFlag!=1）
//   的怪物组包（header 0xd2）发送；数量不足则 logFormat 错误。
// ---------------------------------------------------------------------------
void CBattle_Field::BloodMonsterSpawnPacket(int cnt, int time, short smallRound,
                                            short nextRound)
{
    // [b28d-39 判词 · N82/N91] ORIG 0x08305992：ORIG 帧 0x3a0（含 0x3xx 栈上
    // 组包缓冲）vs 4.4 帧 0x50；大局部对象槽布局跨版本不可控（N82），
    // 组包内容与发送次数逐条对齐；语义等价。
    PacketGuard packet;
    packet.put_header(0, 0xd2);
    packet.put_short(cnt);
    MapInfo* mapInfo = GetCurrentMapInfo();
    if (mapInfo == 0)
        return;
    int putCount = 0;
    std::map<int, map_monster>::iterator it = mapInfo->m_monsterMap.begin();
    for (; it != mapInfo->m_monsterMap.end(); ++it)
    {
        if (putCount == cnt)
            break;
        const map_monster& mob = it->second;
        if (mob.m_bloodSmallRoundOrder != smallRound || mob.m_bloodKillFlag == 1)
            continue;
        packet.put_byte((int)mob.m_bloodDifficulty);
        packet.put_short((int)(short)mob.m_instId);
        packet.put_int(mob.m_mobIndex);
        if (time == -1)
            packet.put_byte(3);
        else
            packet.put_byte(0);
        if (m_pDungeon != 0 && m_pDungeon->m_bloodDungeonType == 2)
            packet.put_byte(0x46);
        else
            packet.put_byte(0x3c);
        packet.put_short((int)mob.m_bloodPhaseTime);
        packet.put_short((int)mob.m_bloodRoundOrder);
        packet.put_short((int)mob.m_bloodSpawnIdx);
        packet.put_short((int)mob.m_bloodPhaseIdx);
        const_cast<map_monster&>(mob).m_bloodKillFlag = 1;
        ++putCount;
    }
    if (putCount == cnt)
    {
        packet.put_short(time);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              "void CBattle_Field::BloodMonsterSpawnPacket(int, int, short, short)",
                              0xe43, "BLOOD_LOG : EROOR BLOOD SPAWN MONSTER COUNT!!\n");
    }
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodPhaseSpawn(short smallRound, short& nextRound,
//                                int timerKey, int phaseIdx)
//   —— ORIG T 0x0830553a（报告 BloodPhaseSpawn.md 逐指令转写）。
// ---------------------------------------------------------------------------
void CBattle_Field::BloodPhaseSpawn(short smallRound, short& nextRound,
                                    int timerKey, int phaseIdx)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x0830553a：帧 0x6c vs 0x8c、全帧槽号平移
    // （N82 lifetime 槽复用差）+ 形参 spill 槽方向（N93）；语义逐条等价。
    const short spawnCnt = (short)get_blood_spawn_cnt_of_phase(smallRound, nextRound);
    int spawnTime = get_blood_spawn_time_of_phase(smallRound);
    const short roundOrder = (short)m_bloodRound.getBloodRoundOrder();
    const short smallRoundOrder = (short)m_bloodRound.getBloodSmallRoundOrder();
    const short lastSmallRoundOrder = (short)m_bloodRound.getBloodLastSmallRoundOrder();

    if (spawnCnt < 0 || spawnTime < 0)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                              0xdb8,
                              "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
                              (int)smallRound);
        return;
    }
    if (m_pMap != 0 && lastSmallRoundOrder == smallRound)
    {
        if (m_pMap->get_map_type() == 1)
            spawnTime = -1;
        else
            spawnTime = 0;
    }
    // ORIG 0x08305672：spawnTime 调整后立即发包 BloodMonsterSpawnPacket(cnt, time, smallRound, nextRound)
    BloodMonsterSpawnPacket(spawnCnt, spawnTime, smallRound, nextRound);
    const int totalPhase = get_blood_spawn_phase_total_cnt_of_small_round(smallRound);
    if (totalPhase < 0)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                              0xdcc,
                              "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
                              (int)smallRound);
        return;
    }
    if ((int)nextRound < totalPhase - 1)
    {
        const unsigned int key = (unsigned int)m_party->gen_timer_key((TIMER_MESSAGE)timerKey);
        int hi = spawnTime >> 31;
        TimerQueue* tq = G_TimerQueue();
        tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                    m_party->GetPartyIndex(),
                                    (TIMER_MESSAGE)timerKey,
                                    (long long)spawnTime,
                                    (int)key, 0);
    }
    else
    {
        m_bloodSpawnTime[phaseIdx] = 0;
        m_bloodPhaseClear[nextRound] = true;
        if (checkBloodSpawnOrder(smallRoundOrder))
        {
            if (lastSmallRoundOrder < smallRoundOrder)
            {
                m_bloodRound.incBloodRoundOrder();
                m_bloodRound.setPreMapBloodRoundOrder();
                return;
            }
            int curRound = get_blood_round_of_small_round(smallRoundOrder);
            int prevRound = get_blood_round_of_small_round(smallRoundOrder - 1);
            int curDiff = get_blood_round_difficylty_of_small_round(smallRoundOrder);
            int prevDiff = get_blood_round_difficylty_of_small_round(smallRoundOrder - 1);
            if (curRound < 0 || prevRound < 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                    0xdee,
                    "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ROUND!!\n",
                    (int)smallRoundOrder);
                return;
            }
            int nextTime = get_blood_spawn_time_of_next_small_round(smallRound);
            if (nextTime < 1)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                    0xdf5,
                    "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
                    (int)smallRoundOrder);
                return;
            }
            if (prevRound == curRound && prevDiff == curDiff)
            {
                const unsigned int key =
                    (unsigned int)m_party->gen_timer_key((TIMER_MESSAGE)0x16);
                int hi = nextTime >> 31;
                TimerQueue* tq = G_TimerQueue();
                tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                            m_party->GetPartyIndex(),
                                            (TIMER_MESSAGE)0x16,
                                            (long long)nextTime,
                                            (int)key, 0);
            }
            else
            {
                m_bloodRound.incBloodRoundOrder();
                m_bloodRound.setPreMapBloodRoundOrder();
            }
        }
    }
    nextRound = (short)(nextRound + 1);
    (void)roundOrder;
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodPhaseSpawnTimerResgister(short) —— ORIG T 0x083053d2：
//   为当前小轮注册各阶段定时器（0x17..0x20），疯狂轮额外发包。
// ---------------------------------------------------------------------------
int CBattle_Field::BloodPhaseSpawnTimerResgister(short smallRound)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x083053d2：帧 0x4c vs 0x5c + 具名局部槽号
    // 整体平移；语义逐条等价。
    int cnt = get_blood_spawn_cnt_of_small_round(smallRound);
    if (cnt < 0 || 10 < cnt)
        return -1;
    if (cnt == 0)
        cnt = 1;
    short curRound = smallRound;
    for (int i = 0; i < cnt; ++i)
    {
        int interval = get_blood_spawn_small_round_interval(curRound);
        if (interval < 1)
            return -1;
        if (m_bloodCrazyRound == curRound)
            BloodCrazyMonsterSpawnPacket();
        m_bloodSpawnCnt[i] = curRound;
        m_bloodSpawnTime[i] = 0;
        const unsigned int key =
            (unsigned int)m_party->gen_timer_key((TIMER_MESSAGE)(0x17 + i));
        int hi = interval >> 31;
        TimerQueue* tq = G_TimerQueue();
        tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                    m_party->GetPartyIndex(),
                                    (TIMER_MESSAGE)(0x17 + i),
                                    (long long)interval,
                                    (int)key, 0);
        ++curRound;
        m_bloodRound.incBloodSmallRoundOrder();
    }
    return 0;
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodRoundIntervalPacket(short round, int interval)
//   —— ORIG T 0x08305e9c：header 0xd7 = 大轮+1、间隔。
// ---------------------------------------------------------------------------
void CBattle_Field::BloodRoundIntervalPacket(short round, int interval)
{
    PacketGuard packet;
    packet.put_header(0, 0xd7);
    packet.put_byte((int)round + 1);
    packet.put_int(interval);
    packet.finalize(true);
    m_party->send_to_party(packet);
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodCrazyMonsterSpawnPacket() —— ORIG T 0x08305d0c：
//   header 0xd5 = 刷怪计+m_bloodCrazyType。
// ---------------------------------------------------------------------------
void CBattle_Field::BloodCrazyMonsterSpawnPacket()
{
    PacketGuard packet;
    packet.put_header(0, 0xd5);
    packet.put_int(m_bloodSpawnCntCur);
    packet.put_byte((int)(signed char)m_bloodCrazyType);
    packet.finalize(true);
    m_party->send_to_party(packet);
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodSpawnTimerManager(bool a, bool b) —— ORIG T 0x08304dd2：
//   检查血轮顺序→发间隔包→注册阶段定时器。
// ---------------------------------------------------------------------------
void CBattle_Field::BloodSpawnTimerManager(bool onStart, bool isMinimal)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x08304dd2：形参 spill 槽 -0x14/-0x12/-0x10
    // vs 4.4 -0x18/-0x16/-0x14 + 帧差；语义逐条等价。
    const short roundOrder = (short)m_bloodRound.getBloodRoundOrder();
    const short smallRoundOrder = (short)m_bloodRound.getBloodSmallRoundOrder();
    const short lastSmallRoundOrder = (short)m_bloodRound.getBloodLastSmallRoundOrder();
    (void)lastSmallRoundOrder;
    if (smallRoundOrder > lastSmallRoundOrder)
        return;
    if (!checkBloodSpawnOrder(smallRoundOrder))
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0xd0c,
            "BLOOD_LOG : round_order_ (%d), small_round_order_ (%d) : SET ORDER ERROR!!\n",
            (int)roundOrder, (int)smallRoundOrder);
        return;
    }
    bool isFirst = false;
    if (smallRoundOrder == 0)
        isFirst = true;
    int interval = get_blood_spawn_small_round_interval(smallRoundOrder);
    if (interval < 1)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0xd19,
            "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
            (int)smallRoundOrder);
        return;
    }
    if ((onStart && !isMinimal) || isFirst)
    {
        m_bloodReward.onStartBloodRound();
        BloodRoundIntervalPacket(roundOrder, interval);
    }
    int r = BloodPhaseSpawnTimerResgister(smallRoundOrder);
    if (r == -1)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0xd2b,
            "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
            (int)smallRoundOrder);
    }
}

// ---------------------------------------------------------------------------
// CBattle_Field::getNextRoundDifficulty(short) —— ORIG T 0x083061b6：
//   下一轮难度 = getUltimateDifficylty()；最后一轮或第 0 轮=1。
// [b24F] DONE：if 内 diff=1 改 return 1（ORIG mov $0x1,%eax 直返）。
// ---------------------------------------------------------------------------
int CBattle_Field::getNextRoundDifficulty(short round)
{
    int diff = (int)getUltimateDifficylty();
    if (round == getBloodMaxRound() - 1 || round == 0)
        return 1;
    return diff;
}

// ---------------------------------------------------------------------------
// CBattle_Field::forceValuesToChangeRound(short round, short smallRound, int diff)
//   —— ORIG T 0x08305f50：末轮强制推进（置全阶段清除、移除当前轮旧怪、写新小轮序）。
// ---------------------------------------------------------------------------
bool CBattle_Field::forceValuesToChangeRound(short round, short smallRound, int diff)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x08305f50：具名 int 槽 -0x18 与实参装载序
    // 跨版本相反（N93/N82）；语义逐条等价。
    const short nextOrder = (short)getNextSamllRoundOrder(round, diff);
    if (nextOrder == 0)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            "bool CBattle_Field::forceValuesToChangeRound(short, short, int)", 0xe7e,
            "ULTIMATE_LOG : blood_round_order (%d) forceValuesToChangeRound ERROR!!\n",
            (int)round);
        return 0;
    }
    if (smallRound < nextOrder)
    {
        for (int i = 0; i < nextOrder; ++i)
            m_bloodPhaseClear[i] = true;
        MapInfo* mapInfo = GetCurrentMapInfo();
        if (mapInfo != 0)
        {
            std::map<int, map_monster>::iterator it = mapInfo->m_monsterMap.begin();
            while (it != mapInfo->m_monsterMap.end())
            {
                if ((int)(unsigned short)it->second.m_bloodRoundOrder2 > (int)round)
                    break;
                if (it->second.m_bloodRoundOrder2 == round &&
                    (int)(signed char)it->second.m_bloodSpawned == diff)
                    break;
                std::map<int, map_monster>::iterator victim = it;
                ++it;
                mapInfo->m_monsterMap.erase(victim);
            }
        }
    }
    m_bloodRound.setBloodSmallRoundOrder(nextOrder);
    return 1;
}

// ---------------------------------------------------------------------------
// CBattle_Field::sendUltimateRandomDifficulty(int a, int b) —— ORIG T 0x08306436：
//   header 0x126 = 两字节难度信息。
// ---------------------------------------------------------------------------
void CBattle_Field::sendUltimateRandomDifficulty(int a, int b)
{
    PacketGuard packet;
    packet.put_header(0, 0x126);
    packet.put_byte(a);
    packet.put_byte(b);
    packet.finalize(true);
    m_party->send_to_party(packet);
}

// ---------------------------------------------------------------------------
// CBattle_Field::sendUltimateRandomDifficulty(int a, int b, int c)
//   —— ORIG T 0x0830637c：header 0x126 = 两字节+short。
// ---------------------------------------------------------------------------
void CBattle_Field::sendUltimateRandomDifficulty(int a, int b, int c)
{
    PacketGuard packet;
    packet.put_header(0, 0x126);
    packet.put_byte(a);
    packet.put_byte(b);
    packet.put_short(c);
    packet.finalize(true);
    m_party->send_to_party(packet);
}

// ---------------------------------------------------------------------------
// CBattle_Field::startUltimateRound() —— ORIG T 0x083064de：
//   终局轮开始：下一轮难度非法→日志；旧轮清理后重注册血轮定时器。
// ---------------------------------------------------------------------------
void CBattle_Field::startUltimateRound()
{
    const short roundOrder = (short)m_bloodRound.getBloodRoundOrder();
    const short smallRoundOrder = (short)m_bloodRound.getBloodSmallRoundOrder();
    // ORIG 第三次 call 结果死存槽 -0xe（-O0 无条件发射具名局部写入，P204 口径）。
    const short lastSmallRoundOrder = (short)m_bloodRound.getBloodLastSmallRoundOrder();
    (void)lastSmallRoundOrder;
    int nextDiff = getNextRoundDifficulty(roundOrder);
    if (nextDiff != 1)
    {
        if (nextDiff == 0)
        {
            LogManager::logFormat(1, "battle_field.cpp",
                                  __PRETTY_FUNCTION__, 0xf04,
                                  "ULTIMATE_LOG : ULITMATE ROUND (%d) startUltimateRound() ERROR!!\n",
                                  (int)roundOrder);
            return;
        }
        if (!forceValuesToChangeRound(roundOrder, smallRoundOrder, nextDiff))
        {
            LogManager::logFormat(1, "battle_field.cpp",
                                  __PRETTY_FUNCTION__, 0xf0a,
                                  "ULTIMATE_LOG : startUltimateRound() ERROR!!\n",
                                  (int)roundOrder);
            return;
        }
    }
    BloodSpawnTimerManager(true, false);
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::getUltimateRewardItem()
//   —— ORIG T 0x083070ca：按 m_ultimateClearPoint 查 CDataManager
//      m_bloodDungeon.m_map358 概率表；rand<field4 → 0x4e4，
//      field4<=rand<field0+field4 → 0x4e3，否则 -1。
// ---------------------------------------------------------------------------
int CBattle_Field::CBloodClearRewardData::getUltimateRewardItem()
{
    // [b28d-39 判词 · N86/N93] ORIG 0x083070ca：ORIG 具名临时槽 -0x18/-0x14/-0x10
    // vs 4.4 -0x20/-0x1c/-0x24（槽向相反）+ G_CDataManager 调用链装载序；
    // 语义逐条等价。
    const int clearPoint = getUltimateRoundClearPoint();
    std::map<int, STUltimateRewardProb>::const_iterator it =
        G_CDataManager()->m_bloodDungeon.m_map358.find(clearPoint);
    if (it == G_CDataManager()->m_bloodDungeon.m_map358.end())
        return -1;
    int roll = get_rand_int(100);
    const STUltimateRewardProb& p = it->second;
    if (roll < p.m_field4)
        return 0x4e4;
    if (p.m_field0 + p.m_field4 < roll)
        return -1;
    return 0x4e3;
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::setRewarItemCopy(Inven_Item, int, int)
//   —— ORIG T 0x085bf176：m_reward[rewardIdx].m_item[itemIdx].setCopy(item)。
// ---------------------------------------------------------------------------
void CBattle_Field::CBloodClearRewardData::setRewarItemCopy(Inven_Item item,
                                                            int rewardIdx,
                                                            int itemIdx)
{
    m_reward[rewardIdx].m_item[itemIdx].setCopy(item);
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::setRewardItemAddInfo(float, int, int)
//   —— ORIG T 0x085bf1a8：m_item[itemIdx].set_add_info(add_info * rate)。
// ---------------------------------------------------------------------------
void CBattle_Field::CBloodClearRewardData::setRewardItemAddInfo(float rate,
                                                                int rewardIdx,
                                                                int itemIdx)
{
    // P183：ORIG 0x85bf1ae/0x85bf1f3 两段完整 imul 链重算地址，无引用缓存。
    // ORIG fildl/fistpl 复用同槽 -0xc：无中间局部嵌套形。
    m_reward[rewardIdx].m_item[itemIdx].set_add_info(
        (int)((float)m_reward[rewardIdx].m_item[itemIdx].get_add_info() * rate));
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
//     bool isClear, short round, CParty* party, CDungeon const* dungeon)
//   —— ORIG T 0x08306fc4：结算血轮（记录时长并写历史）。
// ---------------------------------------------------------------------------
void CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
    bool isClear, short round, CParty* party, const CDungeon* dungeon)
{
    // [b28d-39 判词 · N7/N82] ORIG 0x08306fc4：callee-saved 角色互换
    // （push esi+ebx vs 仅 push ebx）+ 帧 0x40 vs 0x54（N82）；语义逐条等价。
    if (isClear)
        setBloodClear(true);
    m_bloodRound = (int)round;
    m_endTick = GlobalData::s_systemTime_.getCurTickCount();
    // P48 去缓存每用点重取：ORIG 0x8306fc4 对 getBloodRoundClearTime 两次独立
    // 重调（+0x50 addPlayTime / +0x94 historyBloodRoundClear），不缓存。
    addPlayTime(getBloodRoundClearTime());
    const char* dungeonName = dungeon->GetDungeonName();
    unsigned int playTime = getPlayTime();
    party->historyBloodRoundClear(m_bloodRound, getBloodRoundClearTime() / 1000,
                                  playTime / 1000, dungeonName);
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_cnt_of_small_round(short)
//   —— ORIG T 0x08306670：小轮刷怪数 = phaseTime.m_spawnCnt（+0x14）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_cnt_of_small_round(short smallRound)
{
    // [b28d-39] ORIG 0x08306670：同 get_blood_round_of_small_round 的 `||` 短路形。
    if (m_bloodPhaseTimes.empty() ||
        smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int result = m_bloodPhaseTimes[smallRound].m_spawnCnt;
    return result;
}

// ---------------------------------------------------------------------------
// CBattle_Field::getNextSamllRoundOrder(short round, int diff)
//   —— ORIG T 0x08306212：找当前大轮+难度的第一个怪，取其血小轮序；无则 0。
// ---------------------------------------------------------------------------
short CBattle_Field::getNextSamllRoundOrder(short round, int diff)
{
    // [b28d-39 判词 · N86/N93] ORIG 0x08306212：ORIG 返回槽预清 `movw $0x0,-0xa`
    // + map 迭代器槽阶梯（-0x18/-0x1c/-0x20 vs 4.4 -0x14/-0x18/-0x10）；
    // N86 copy-elision/槽向族；语义逐条等价。
    MapInfo* mapInfo = GetCurrentMapInfo();
    if (mapInfo == 0)
        return 0;
    std::map<int, map_monster>::iterator it = mapInfo->m_monsterMap.begin();
    for (; it != mapInfo->m_monsterMap.end(); ++it)
    {
        const map_monster& mob = it->second;
        if (mob.m_bloodRoundOrder2 == round &&
            (int)(signed char)mob.m_bloodSpawned == diff)
            return (short)mob.m_bloodSmallRoundOrder;
    }
    return 0;
}
