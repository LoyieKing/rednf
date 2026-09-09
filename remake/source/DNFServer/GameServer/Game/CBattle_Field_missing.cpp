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
#include "GlobalData.h"          // GlobalData::s_villageMonsterMgr（ORIG B 0x0941f77c）
#include "LogManager.h"          // logFormat（血轮日志）
#include "TimerQueue.h"          // G_TimerQueue / InsertTimerInMilisecond
#include "GameRand.h"           // get_rand_int

// ================= CBattle_Field::CBattle_Field() — ORIG 0x082a44fe =================
//
// 子对象（pair[2..9]、map/vector/Mutex/CRidable/CClearCondition/嵌套奖励等）
// 由成员声明序自动构造；此处仅复刻 ORIG 的标量初值与容器清理操作。
CBattle_Field::CBattle_Field()
{
    // [b28d-solo-CBattle_Field_missing 判词 · N91] ORIG 0x082a44fe：CBattle_Field.h
    // 已按 ORIG 修正（m_endX/m_endY/m_field14/m_field18 + m_curPos[4] +
    // m_lastPos[4]，R2 已广播）后，前 190 条指令与 ORIG 逐条一致（两组 4 次
    // pair C1Ev、m_party=0 首写、链式 -1、清零循环、标量初值、SetCurSeatNo 全对齐，
    // 帧同为 sub $0x20）。残余 = 4.4 为 ctor 内非平凡成员恒发的 EH cleanup 残段
    //（_Unwind_Resume 链，ORIG 4.1 零 cleanup）；P216 突破口需给成员类 ctor/dtor
    // 批量加 throw()（跨多文件），本波未做。
    // —— 位置链：起点/终点槽对夫妇构完后按 ORIG 置初值 ——
    // ORIG：m_lastY(+0x6c) = -1，再沿 +0x68/+0x64/+0x60/+0x18/+0x14/+0x10/+0x0c/
    // +0x08/+0x04 级联，故 m_startX/m_startY、m_curX/m_curY、m_lastX/m_lastY、
    // m_endX/m_endY(+0xc/+0x10)、m_field14/m_field18(+0x14/+0x18) 均为 -1。
    // [b28d-solo-CBattle_Field_missing] 4.4 链式赋值自**最右端**起 store ⇒ 把
    // ORIG 首写端放最右：m_lastY(0x6c)→0x68→0x64→0x60→0x18→0x14→0x10→0xc→
    // 0x8→0x4。m_curPos/m_lastPos 由隐式 C1 构造（pair 默认 0），ctor 体不写
    //（ORIG 亦只发两组 C1Ev）。
    m_party = NULL;                        // +0x00（ORIG 首写，在链式 -1 之前）
    m_startX = m_startY = m_endX = m_endY = m_field14 = m_field18 =
        m_curX = m_curY = m_lastX = m_lastY = -1;
    // ORIG C1Ev 构造后再显式清零两组 pair[4]（0x0830a4xx 段 `movl $0x0,
    // 0xc/0x10(%eax,%edx,8)`，索引 edx=i+2 / i+6，循环 i<=3）。
    for (int i = 0; i < 4; ++i)
    {
        m_curPos[i].first  = 0;
        m_curPos[i].second = 0;
        m_lastPos[i].first  = 0;
        m_lastPos[i].second = 0;
    }

    // —— 标量初值（逐字段对应 ORIG ctor 汇编）——
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
bool* CBattle_Field::GetEnableHellDungeon()
{
    // mov $0x93701ec,%eax; ret（POD 无 guard 的函数局部
    // static，V 符号 _ZZN13CBattle_Field20GetEnableHellDungeonEvE19enable_hell_dungeon）。
    static bool enable_hell_dungeon;
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
    // [b28d-solo-CBattle_Field_missing 判词 · AE] ORIG 0x0830a1f6：头建模修正
    //（m_curPos[4]/m_lastPos[4] 具名数组）后与 ORIG 逐条一致——原 m_pos[i+2]/
    // [i+6] 的 add-vs-lea 残差消除（ORIG 的 add $0x2/$0x6 + disp 0xc 与数组基址
    // +0x1c/+0x3c 索引 seat 地址等价）。
    // —— 位置/尺寸链 ——
    m_dungeonIndex = 0;                     // +0x194
    m_mapHeight    = 0;                     // +0x190
    m_mapWidth     = m_mapHeight;           // +0x18c
    // 位置链：pair[1].second/+0x18 → ... → m_startX，最终全 0。
    // ORIG 为链式复制（0x18→0x14→0x64→0x60→0x10→0xc→0x8→0x4），多重赋值复现。
    // [b28d-solo-CBattle_Field_missing] 4.4 链式赋值自最右端起 store ⇒ 把 ORIG
    // 首写端 m_field18(0x18) 放最右：0x18→0x14→0x64→0x60→0x10→0xc→0x8→0x4。
    m_startX = m_startY = m_endX = m_endY = m_curX = m_curY =
        m_field14 = m_field18 = 0;
    for (int i = 0; i < 4; ++i)             // m_curPos[0..3] 与 m_lastPos[0..3] 清零
    {
        m_curPos[i].first  = 0;
        m_curPos[i].second = 0;
        m_lastPos[i].first  = 0;
        m_lastPos[i].second = 0;
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
        GlobalData::s_villageMonsterMgr->OnKillVillageMonster(
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
    // [b28d-solo-CBattle_Field_missing 判词 · N107/N94] ORIG 0x0830a57c：残余为
    // 成员常量偏移取址（`flds 0x10(%eax)` vs 4.4 `add $0x10,%eax; flds (%eax)`，
    // N107 六形探针全产 add）+ 数组元素 `0x8(%eax,%edx,4)` vs 4.4 shl+add
    // （N94 scaled-index 拆分约束）；语义等价。
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306a62：
    // ①声明序 cnt(-0x18)/startCnt(-0x14)/sysIdx(-0x10)/p(-0xc)（4.4 同声明序
    //   先声明深，P212 全头部声明）；
    // ②sysIdx 为 int ⇒ signed mem 直比（P209）；
    // ③startCnt 循环外取一次、m_cntInc 每轮重调 operator[]（P157 逐字段重读）。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x08306a62：`(short)size()` 与 short 形参 16 位比较
    // （cmp -0x1c(%ebp),%ax），避免 4.4 为 size_t 提升把 movswl 值缓存进 %ebx。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int cnt = 0;
    int startCnt = 0;
    int sysIdx;
    int p;
    sysIdx = (int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == -1)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306a62：`cmp -0x10(%ebp),%eax;
    // setle` 正写 + signed 内存直比。
    if ((int)m_bloodSystems.size() <= sysIdx)
        return -1;
    startCnt = m_bloodSystems[sysIdx].m_startCnt;
    for (p = 0; p < (int)phase + 1; ++p)
        cnt = startCnt + m_bloodSystems[sysIdx].m_cntInc * p;
    return cnt;
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_time_of_phase(short smallRound)
//   —— ORIG T 0x0830690a：阶段时间 = STAssignBloodSystem.m_timeB（+0x18）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_time_of_phase(short smallRound)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x0830690a：result 预清在 -0x10、
    // sysIdx 在 -0xc ⇒ 原码 result 先声明（P212 同作用域「先声明深」）；sysIdx
    // 为 int（signed）⇒ `(int)size() <= sysIdx` 4.4 走 mem 直比（P209）。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x0830690a：短整比较形（同 get_blood_spawn_cnt_of_phase）。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // result(-0x10, 预清 0) 先声明、sysIdx(-0xc) 次声明（ORIG 槽序）。
    int result = 0;
    int sysIdx;
    sysIdx = (int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == -1)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-solo-CBattle_Field_missing] ORIG 0x0830690a：`cmp -0xc(%ebp),%eax;
    // setle` 正写 + signed 内存直比。
    if ((int)m_bloodSystems.size() <= sysIdx)
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306c0e：同 get_blood_spawn_time_of_phase
    // （result 先声明占 -0x10、sysIdx int 占 -0xc，signed 比较 mem 直比）。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x08306c0e：短整比较形（同 get_blood_spawn_cnt_of_phase）。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // result(-0x10, 预清 0) 先声明、sysIdx(-0xc) 次声明（ORIG 槽序）。
    int result = 0;
    int sysIdx;
    sysIdx = (int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == -1)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306c0e：`cmp -0xc(%ebp),%eax;
    // setle` 正写 + signed 内存直比。
    if ((int)m_bloodSystems.size() <= sysIdx)
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x0830676a：同 get_blood_spawn_time_of_phase
    // （result 先声明占 -0x10、sysIdx int 占 -0xc，signed 比较 mem 直比）。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x0830676a：短整比较形（同 get_blood_spawn_cnt_of_phase）。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // result(-0x10, 预清 0) 先声明、sysIdx(-0xc) 次声明（ORIG 槽序）。
    int result = 0;
    int sysIdx;
    sysIdx = (int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == -1)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-solo-CBattle_Field_missing] ORIG 0x0830676a：`cmp -0xc(%ebp),%eax;
    // setle` 正写 + signed 内存直比。
    if ((int)m_bloodSystems.size() <= sysIdx)
        return -1;
    // [b28d-solo-CBattle_Field_missing] P157/P183：ORIG 三次独立 operator[]
    // （+0x14 timeA → +0x18 timeB → +0x24 totalCnt），timeA/timeB 各跨后续调用
    // 保活 %ebx/%esi ⇒ 逐字段表达式重读，禁引用缓存；乘法书写序 timeB*totalCnt
    // 复现 `imul %esi,%eax`（totalCnt 落 %eax、timeB 落 %esi）。
    result = m_bloodSystems[sysIdx].m_timeA
           - m_bloodSystems[sysIdx].m_timeB * m_bloodSystems[sysIdx].m_totalCnt;
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
    // [b28d-solo-CBattle_Field_missing 判词 · N82/P157] ORIG 0x08305992：ORIG 帧
    // 0x3a0 含 map_monster 栈对象（-0x370，调 map_monster::C1Ev）与逐字段
    // operator->() 重调；本仓实现无该局部、走 mapInfo 缓存 + it->second 引用，
    // 帧/结构差未闭合（需按 ORIG 完整重写，本波未做）；组包内容与发送次数
    // 语义等价。
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
    // [b28d-solo-CBattle_Field_missing 判词 · N82/N93] ORIG 0x0830553a：删 hi/tq
    // 具名局部后帧 0x7c vs ORIG 0x6c（仍差 0x10 槽基 + 3 条指令），N82 lifetime
    // 槽复用/槽基族；逐字段语义等价。
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
        const int key = (int)m_party->gen_timer_key((TIMER_MESSAGE)timerKey);
        G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                m_party->GetPartyIndex(),
                                                (TIMER_MESSAGE)timerKey,
                                                (long long)spawnTime,
                                                key, 0);
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
                const int key = (int)m_party->gen_timer_key((TIMER_MESSAGE)0x16);
                G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                        m_party->GetPartyIndex(),
                                                        (TIMER_MESSAGE)0x16,
                                                        (long long)nextTime,
                                                        key, 0);
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x083053d2：
    // ①无 curRound/hi/tq 具名局部——smallRound 形参 spill 槽 `addw $1` 自增、
    //   long long 高半字由 %esi 承载、receiver 直接 G_TimerQueue()（P52 链式）；
    // ②`cnt==0` 写 `++cnt`（ORIG addl 形，P38）；
    // ③循环体用 smallRound 本身（非副本），`++smallRound` 在
    //   incBloodSmallRoundOrder 之前。
    int cnt = get_blood_spawn_cnt_of_small_round(smallRound);
    if (cnt < 0 || 10 < cnt)
        return -1;
    if (cnt == 0)
        ++cnt;
    for (int i = 0; i < cnt; ++i)
    {
        int interval = get_blood_spawn_small_round_interval(smallRound);
        if (interval < 1)
            return -1;
        if (m_bloodCrazyRound == smallRound)
            BloodCrazyMonsterSpawnPacket();
        m_bloodSpawnCnt[i] = smallRound;
        m_bloodSpawnTime[i] = 0;
        const int key = (int)m_party->gen_timer_key((TIMER_MESSAGE)(0x17 + i));
        G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                m_party->GetPartyIndex(),
                                                (TIMER_MESSAGE)(0x17 + i),
                                                (long long)interval,
                                                key, 0);
        ++smallRound;
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08304dd2：
    // ①receiver = m_party->m_battleField.m_bloodReward（CParty+0xb24 内嵌
    //   CBattle_Field + m_bloodReward+0x2b8 = m_party+0xddc，ORIG 实证）；
    // ②局部集 round(-0x14)/smallRound(-0x12)/last(-0x10)/isFirst(-0xd)/
    //   interval(-0xc)，无具名 r（ORIG 直比 %eax，sete 物化）。
    // ③残余 1 条：ORIG BODY 内 `cmpb $0,-0x1c(%ebp)` 为死比较（无分支消费者，
    //   4.1 逐条发射），4.4 的 `if (onStart)` 必产 je ⇒ P184/N184 死块变体封顶。
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
        if (onStart)
            m_party->m_battleField.m_bloodReward.onStartBloodRound();
        BloodRoundIntervalPacket(roundOrder, interval);
    }
    if (BloodPhaseSpawnTimerResgister(smallRoundOrder) == -1)
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08305f50：
    // ①`int nextDiff = diff` 具名槽 -0x18（P0 槽往返）仅供首次调用；
    // ②short result(-0x12) 预清 0；③GetCurrentMapInfo 每用点重调（begin/end/
    //   erase 后重 begin），无判空（P140/P157）；④循环内 it->second 逐字段重读。
    // ⑤残余 N82/N93：本仓 -0x24/-0x28 槽基 vs ORIG -0x20/-0x24，且 4.4 把
    //   continue 处的循环条件内联（`end()` 每轮在尾块求值）vs ORIG 回跳 cond 块
    //   （for(;;)+break 已试，仍差 3 条）；槽基/循环布局跨版本族封顶。
    int nextDiff = diff;
    const short nextOrder = (short)getNextSamllRoundOrder(round, nextDiff);
    short result = 0;
    if (nextOrder == 0)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            "bool CBattle_Field::forceValuesToChangeRound(short, short, int)", 0xe7e,
            "ULTIMATE_LOG : blood_round_order (%d) forceValuesToChangeRound ERROR!!\n",
            (int)round);
        return 0;
    }
    if (nextOrder > smallRound)
    {
        for (int i = 0; i < nextOrder; ++i)
            m_bloodPhaseClear[i] = true;
        MapInfo* mapInfo = GetCurrentMapInfo();
        std::map<int, map_monster>::iterator it =
            GetCurrentMapInfo()->m_monsterMap.begin();
        for (;;)
        {
            if (it == GetCurrentMapInfo()->m_monsterMap.end())
                break;
            if ((int)(unsigned short)it->second.m_bloodRoundOrder2 > (int)round)
                break;
            if (it->second.m_bloodRoundOrder2 == round &&
                (int)(signed char)it->second.m_bloodSpawned == diff)
                break;
            mapInfo->m_monsterMap.erase(it);
            it = GetCurrentMapInfo()->m_monsterMap.begin();
            result = 0;
        }
    }
    (void)result;
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x083070ca：
    // ①声明序 it(-0x20)/itEnd(-0x1c)/clearPoint(-0x18)/f4(-0x14)/f8(-0x10)
    //   （4.1 先声明深；语句序 clearPoint→f4=0→f8=0→itEnd=end()→it=find()）；
    // ②非 const iterator（ORIG `_ZNKSt17_Rb_tree_iterator…eq` 非 const_iterator，P96）；
    // ③P157 两次独立 operator->() 取 second.m_field4/second.m_field8，禁引用缓存。
    int clearPoint = getUltimateRoundClearPoint();
    int f0 = 0;
    int f4 = 0;
    std::map<int, STUltimateRewardProb>::iterator itEnd =
        G_CDataManager()->m_bloodDungeon.m_map358.end();
    std::map<int, STUltimateRewardProb>::iterator it =
        G_CDataManager()->m_bloodDungeon.m_map358.find(clearPoint);
    if (it == itEnd)
        return -1;
    int roll = get_rand_int(100);
    f0 = it->second.m_field0;
    f4 = it->second.m_field4;
    // [b28d-solo-CBattle_Field_missing] ORIG 布局：`jg then(在后)` + else 直落
    // ⇒ 否定早返形（P222/P59）。
    if (roll <= f4)
        return 0x4e4;
    f0 += f4;
    if (roll <= f0)
        return 0x4e3;
    return -1;
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306fc4：
    // ①无 dungeonName/playTime 具名局部——实参右到左求值：GetDungeonName 落
    //   %ebx 跨 getPlayTime/getBloodRoundClearTime 保活（P52/P226 内联实参）；
    // ②两次 `/1000` 共用编译器临时槽 -0x1c（无源码局部）；
    // ③addPlayTime(getBloodRoundClearTime()) 每用点重调（P48 去缓存）。
    if (isClear)
        setBloodClear(true);
    m_bloodRound = (int)round;
    m_endTick = GlobalData::s_systemTime_.getCurTickCount();
    addPlayTime(getBloodRoundClearTime());
    party->historyBloodRoundClear(m_bloodRound, getBloodRoundClearTime() / 1000,
                                  getPlayTime() / 1000, dungeon->GetDungeonName());
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306212：
    // ①P140 无 GetCurrentMapInfo 判空（ORIG 直 lea 0xc(%eax)）；
    // ②P0 无 mapInfo 具名局部——循环增量处再次 call GetCurrentMapInfo，
    //   end() 每轮重求（P13 反向）；
    // ③P157/P183 it->second 逐字段重读（两次 operator->()），禁引用缓存；
    // ④short result 预清 `movw $0x0,-0xa(%ebp)`，尾 movzwl 直返。
    // ⑤残余：ORIG 增量段 `ppEi` 后置迭代器（sret 临时 -0x10）构成三槽阶梯，
    //   4.4 对结果未使用的后置 ++ 恒折 ppEv 免临时槽 ⇒ 迭代器槽整体浅 4
    //   （N113 封顶）。
    short result = 0;
    std::map<int, map_monster>::iterator it = GetCurrentMapInfo()->m_monsterMap.begin();
    for (; it != GetCurrentMapInfo()->m_monsterMap.end(); ++it)
    {
        if (it->second.m_bloodRoundOrder2 == round &&
            (int)(signed char)it->second.m_bloodSpawned == diff)
        {
            result = (short)it->second.m_bloodSmallRoundOrder;
            break;
        }
    }
    return result;
}
