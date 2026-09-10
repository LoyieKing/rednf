// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBattle_Field_missing/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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

#include "DataManagerInitDepends.h"   // stHellParty_t（get_hellparty_script_values 返回值）

// [b28d-round5-CBattle_Field_missing] 复现 ORIG 脚本源的「基址 + 常数索引」两步寻址（P30）：
// ORIG 三段源装载为 `add $0x4,%edx; flds 0x8(%eax,%edx,4)` / `add $0x8,%edx; flds 0x8(...)` /
// `add $0xc,%edx; flds 0xc(...)`（基址 +0x8/+0x8/+0xc 的 float 数组视角，索引 i+4/i+8/i+0xc）；
// 具名成员直访问 `m_levelDiffRates[i]` 只能折单 disp（disp 0x18 + 索引 i），指令数不同 ⇒
// 以只读视图表达同一内存的数组视角（sizeof/偏移零影响，TU 局部匿名类型，不发射符号）。
namespace
{
// stHellParty_t 的 +0x08 起 float 数组视角（0x8 + (i+4)*4 == m_levelDiffRates[i]，
// 0x8 + (i+8)*4 == m_difficultyRates[i]）。
struct stHellPartyRateSrcView8
{
    int m_maxBaseValue;                 // +0x00
    int m_minBaseValue;                 // +0x04
    float m_rates[0x24];                // +0x08（覆盖 +0x08..+0x97）
};
// stHellParty_t 的 +0x0c 起 float 数组视角（0xc + (i+0xc)*4 == m_partyRates[i]）。
struct stHellPartyRateSrcViewC
{
    int m_maxBaseValue;                 // +0x00
    int m_minBaseValue;                 // +0x04
    int m_baseValue;                    // +0x08
    float m_rates[0x23];                // +0x0c（覆盖 +0x0c..+0x97）
};
}

// ================= CBattle_Field::CBattle_Field() — ORIG 0x082a44fe =================
//
// 子对象（pair[2..9]、map/vector/Mutex/CRidable/CClearCondition/嵌套奖励等）
// 由成员声明序自动构造；此处仅复刻 ORIG 的标量初值与容器清理操作。
#include "CBattle_Field_missing/CBattle_Field.inc"
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
#include "CBattle_Field_missing/dtor_CBattle_Field.inc"
}

// ================= CBattle_Field::GetEnableHellDungeon() — ORIG 0822cf26 =================
// 定义置于本 TU（与调用者分离）：4.4 unit-at-a-time 对 TU 内可见 callee 省略
// outgoing 帧（sub $0x10/槽-0x4），ORIG 4.1 恒发 sub $0x18/槽-0xc（探针 p6/p7）。
#include "CBattle_Field_missing/GetEnableHellDungeon.inc"
}

// ================= CBattle_Field::pickup_item(int) — ORIG 0x0830daee =================
// 在当前地图的 itemMap 中查找 uid；找到则 erase 并返回 0，否则返回 0x15(21)。
#include "CBattle_Field_missing/pickup_item.inc"
}

// ================= CBattle_Field::reset_field() — ORIG 0x0830a1f6 =================
// 全字段清零链，逐字段按 ORIG 汇编映射到 CBattle_Field.h 具名成员。
#include "CBattle_Field_missing/reset_field.inc"
}

// ================= CBattle_Field::reset_hell_party_value() — ORIG 0x0830a57c =================
// 从 CDataManager 的 hellparty 脚本值（float 数组）拷贝各组比例；分母 10000.0f。
#include "CBattle_Field_missing/reset_hell_party_value.inc"
}
// ============================================================================
// == 血轮（Blood Dungeon）链 —— ORIG 报告逐指令转写（2026-08-25 批次）      ==
// 符号清单：/tmp/df_batches/undef_link_syms_2026-08-25.txt 中 CBattle_Field 域；
// 依赖血轮辅助函数（同报告）一并实现，报告均在 docs/class_func_reports/CBattle_Field/。
// ============================================================================

// ---------------------------------------------------------------------------
// CBattle_Field::isQuestMaze —— ORIG T 0x0830e31e：返回 m_questMaze(+0x110)。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/isQuestMaze.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_cnt_of_phase(short smallRound, short phase)
//   —— ORIG T 0x08306a62：阶段生成数 = STAssignBloodSystem.m_startCnt +
//      phase * m_cntInc（随机阶段经 m_bloodPhaseOrder 回退）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_spawn_cnt_of_phase.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_time_of_phase(short smallRound)
//   —— ORIG T 0x0830690a：阶段时间 = STAssignBloodSystem.m_timeB（+0x18）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_spawn_time_of_phase.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_phase_total_cnt_of_small_round(short)
//   —— ORIG T 0x08306c0e：阶段总数 = STAssignBloodSystem.m_totalCnt（+0x24）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_spawn_phase_total_cnt_of_small_round.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_time_of_next_small_round(short)
//   —— ORIG T 0x0830676a：下一小轮时间 = m_timeA - m_totalCnt * m_timeB。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_spawn_time_of_next_small_round.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_round_of_small_round(short)
//   —— ORIG T 0x083065f8：小轮对应大轮 = phaseTime.m_smallRoundOrder。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_round_of_small_round.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_round_difficylty_of_small_round(short)
//   —— ORIG T 0x083062fe：小轮难度 = phaseTime.m_difficulty（+0x18 char）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_round_difficylty_of_small_round.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_small_round_interval(short)
//   —— ORIG T 0x083066ea：小轮间隔 = phaseTime.m_interval（+0x08）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_spawn_small_round_interval.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::checkBloodSpawnOrder(short) —— ORIG T 0x08306e32：
//   小轮序在 [0,size) 且前 smallRound 个阶段均已清除（m_bloodPhaseClear 前段全 true）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/checkBloodSpawnOrder.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodMonsterSpawnPacket(int cnt, int time, short smallRound,
//                                        short nextRound)
//   —— ORIG T 0x08305992（_ZN13CBattle_Field23BloodMonsterSpawnPacketEiiss）：
//   遍历当前图怪物表，收集 bloodSmallRoundOrder==smallRound 且未参战（killFlag!=1）
//   的怪物组包（header 0xd2）发送；数量不足则 logFormat 错误。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/BloodMonsterSpawnPacket.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodPhaseSpawn(short smallRound, short& nextRound,
//                                int timerKey, int phaseIdx)
//   —— ORIG T 0x0830553a（报告 BloodPhaseSpawn.md 逐指令转写）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/BloodPhaseSpawn.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodPhaseSpawnTimerResgister(short) —— ORIG T 0x083053d2：
//   为当前小轮注册各阶段定时器（0x17..0x20），疯狂轮额外发包。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/BloodPhaseSpawnTimerResgister.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodRoundIntervalPacket(short round, int interval)
//   —— ORIG T 0x08305e9c：header 0xd7 = 大轮+1、间隔。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/BloodRoundIntervalPacket.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodCrazyMonsterSpawnPacket() —— ORIG T 0x08305d0c：
//   header 0xd5 = 刷怪计+m_bloodCrazyType。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/BloodCrazyMonsterSpawnPacket.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::BloodSpawnTimerManager(bool a, bool b) —— ORIG T 0x08304dd2：
//   检查血轮顺序→发间隔包→注册阶段定时器。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/BloodSpawnTimerManager.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::getNextRoundDifficulty(short) —— ORIG T 0x083061b6：
//   下一轮难度 = getUltimateDifficylty()；最后一轮或第 0 轮=1。
// [b24F] DONE：if 内 diff=1 改 return 1（ORIG mov $0x1,%eax 直返）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/getNextRoundDifficulty.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::forceValuesToChangeRound(short round, short smallRound, int diff)
//   —— ORIG T 0x08305f50：末轮强制推进（置全阶段清除、移除当前轮旧怪、写新小轮序）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/forceValuesToChangeRound.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::sendUltimateRandomDifficulty(int a, int b) —— ORIG T 0x08306436：
//   header 0x126 = 两字节难度信息。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/sendUltimateRandomDifficulty.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::sendUltimateRandomDifficulty(int a, int b, int c)
//   —— ORIG T 0x0830637c：header 0x126 = 两字节+short。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/sendUltimateRandomDifficulty_1.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::startUltimateRound() —— ORIG T 0x083064de：
//   终局轮开始：下一轮难度非法→日志；旧轮清理后重注册血轮定时器。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/startUltimateRound.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::getUltimateRewardItem()
//   —— ORIG T 0x083070ca：按 m_ultimateClearPoint 查 CDataManager
//      m_bloodDungeon.m_map358 概率表；rand<field4 → 0x4e4，
//      field4<=rand<field0+field4 → 0x4e3，否则 -1。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/getUltimateRewardItem.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::setRewarItemCopy(Inven_Item, int, int)
//   —— ORIG T 0x085bf176：m_reward[rewardIdx].m_item[itemIdx].setCopy(item)。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/setRewarItemCopy.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::setRewardItemAddInfo(float, int, int)
//   —— ORIG T 0x085bf1a8：m_item[itemIdx].set_add_info(add_info * rate)。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/setRewardItemAddInfo.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
//     bool isClear, short round, CParty* party, CDungeon const* dungeon)
//   —— ORIG T 0x08306fc4：结算血轮（记录时长并写历史）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/onFinishBloodRound.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_cnt_of_small_round(short)
//   —— ORIG T 0x08306670：小轮刷怪数 = phaseTime.m_spawnCnt（+0x14）。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/get_blood_spawn_cnt_of_small_round.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::getNextSamllRoundOrder(short round, int diff)
//   —— ORIG T 0x08306212：找当前大轮+难度的第一个怪，取其血小轮序；无则 0。
// ---------------------------------------------------------------------------
#include "CBattle_Field_missing/getNextSamllRoundOrder.inc"
}
