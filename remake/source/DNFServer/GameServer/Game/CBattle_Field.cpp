// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBattle_Field/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CBattle_Field（G2-3 第一批）。
// 逐函数对照 docs/class_func_reports/CBattle_Field.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径，分支/调用目标→<T>、
// 大绝对地址→<A>、去@plt、剥尾部 nop、C2/D2 同址别名良性）。
// 已并入原 CBattle_Field_map.cpp（地图/移动小簇）；草稿文件已删除。
#include <cstring>

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"
#include "PacketGuard.h" // 权威 PacketGuard(0xc)
#include "CQuickPartyRewardManager.h"
#include "GameRand.h"  // get_rand_int

// ============================================================================

// ============================================================================
// 外部方法（权威头已提供声明：MazeScript::getGridR 见 CDungeon.h，
// CSystemTime::getCurTickCount 见 CSystemTime.h，CRidable::CheckAppearRidableObject
// 见 CRidable.h；GlobalData::s_systemTime_ 见 CUserCharacInfo.h）
// ============================================================================
CDataManager* G_CDataManager();

// CDungeon 局部布局代理（真实 CDungeon.h 已含 +0x8a0 字段布局的完整类型）：
// 本代理仅用于 +0x8a0 字段直接位移访问，保持 ORIG 直接位移加载形态。
struct CDungeon_8a0_proxy
{
    char m_pad[0x8a0];
    int m_8a0;
};

// G_CDataManager() +0x6170 处的 int 表视图（incUltimateRoundClearPoint 08307096
// 实证 add $0x185c,%edx + (%eax,%edx,4) 寻址 = 元素索引 v+0x185c 的成员数组
// 折算形态；视图仅本 TU 使用）
struct CDataManager_UltimateArr_proxy
{
    char m_pad[0x6170];
    int m_arr[1];
};

// ============================================================================
// 小访问器（ORIG 弱符号，直接读反汇编实现）
// ============================================================================

#include "CBattle_Field/SetTournamentCurRound.inc"
}

#include "CBattle_Field/SetParty.inc"
}

#include "CBattle_Field/SetCurSeatNo.inc"
}

#include "CBattle_Field/GetMapInfoKeyValue.inc"
}

#include "CBattle_Field/GetCurrentMapInfo.inc"
}

// [b28d-solo-CBattle_Field] 已修 AE（178=178；旧判「N95 槽基差族」作废）：
// ORIG 第二次 key 落**独立槽 -0x204**（首算 key 在 -0x1fc）⇒ P0 槽-局部
// 双向对应律：原码是新的具名局部，不是复用 `key`。改成 `int insertKey = ...`
// 后 4.4 逐条复现（info 槽 -0x2f0、key -0x204 同时归位；帧 sub $0x310 不变）。
// m_layeredMap bool 化后 0x0830cd42 段 setne 伪差已消（DIFF→NEAR→AE）。
#include "CBattle_Field/GetMapInfoFromPos.inc"
}

#include "CBattle_Field/getMapWidth.inc"
}

#include "CBattle_Field/getMapHeight.inc"
}

#include "CBattle_Field/check_start_point.inc"
}

#include "CBattle_Field/check_end_point.inc"
}

#include "CBattle_Field/get_cur_map.inc"
}

#include "CBattle_Field/GetMapInfo.inc"
}

#include "CBattle_Field/getMaze.inc"
}

#include "CBattle_Field/GetCurrentMapIndex.inc"
}

#include "CBattle_Field/IsClearDungeon.inc"
}

#include "CBattle_Field/get_dungeon_index.inc"
}

#include "CBattle_Field/get_dungeon_diff.inc"
}

#include "CBattle_Field/GetDungeon.inc"
}

#include "CBattle_Field/GetMonsterMap.inc"
}

#include "CBattle_Field/get_map_index.inc"
}

#include "CBattle_Field/GetCurrentMap.inc"
}

#include "CBattle_Field/ClearCondition.inc"
}

#include "CBattle_Field/IsVisitedCurrentMap.inc"
}

#include "CBattle_Field/setWorldMapTowerArea.inc"
}

#include "CBattle_Field/isWorldMapTowerArea.inc"
}

// [b28d-solo-CBattle_Field] N4/P30 封顶（28=28，唯 add-vs-lea + 索引寄存器落点）：
// ORIG 0x085bf364 `mov 0x114(%eax),%edx; add $0x2,%edx` vs 4.4 `mov mem,%eax;
// lea 0x2(%eax),%edx`。复核：D43 六形 + 本波字段类型全变体探针（/tmp/s18/p5.cpp
// int/uint/short/ushort/long/ulong 六型恒 lea）⇒ 4.4 ISel 恒 lea，语义等价。
#include "CBattle_Field/GetCurPosX.inc"
}

// [b28d-solo-CBattle_Field] N4/P30 同 GetCurPosX（28=28，+2 索引段两处；ORIG 0x085bf3ba 区）。
#include "CBattle_Field/GetCurPosY.inc"
}

// N81 附族：m_lastPos[m_curSeatNo] 索引段同上（add vs lea 直落，不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 封顶（49=49，+6 索引段两处）：ORIG
// `mov 0x114(%eax),%edx; add $0x6,%edx` vs 4.4 `mov mem,%eax; lea 0x6(%eax),%edx`
// （ORIG 0x0830eac8 段）。
#include "CBattle_Field/GetLastMapInfo.inc"
}

// N81 附族：m_curPos[m_curSeatNo] 索引段 `mov mem,%eax`+`lea` 合成 vs ORIG
// `mov mem,%edx`+`add` 直落（同 GetCurPosX/Y，4.1 索引 regmove 落点，合法源码
// 不可复现——负面结论同列）。
// [b28d-solo-CBattle_Field] N4/P30 同 GetCurPosX（51=51，+2 索引段两处；ORIG 0x08300da6）。
#include "CBattle_Field/getCurPosXY.inc"
}

// N81 附族：m_lastPos[m_curSeatNo] 索引段同上（add vs lea 直落，不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 同族（47=47，+6 索引段两处；ORIG 0x08300e44）。
#include "CBattle_Field/getLastPosXY.inc"
}

// N81 附族：m_lastPos[...]/m_curPos[...] 索引段 + make_pair 传参同理（add vs lea 直落，
// 不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 同族（76=76，+6/+2 索引段 + make_pair<int&,int&> 段；ORIG 0x0830878e）。
#include "CBattle_Field/swap_curpos_xy.inc"
}

// ============================================================================
// 小型访问器（读 ORIG 反汇编逐个实现）
// ============================================================================

#include "CBattle_Field/getBloodState.inc"
}

#include "CBattle_Field/SetEnableHellDungeon.inc"
}

#include "CBattle_Field/setUltimateDifficulty.inc"
}

#include "CBattle_Field/getUltimateDifficylty.inc"
}

#include "CBattle_Field/IsAssignNPCByPotion.inc"
}

#include "CBattle_Field/IsEnableHellDungeon.inc"
}

// [b28d-solo-CBattle_Field] N97/P230 封顶（12/14）：4.4 char 形参恒 spill（`mov
// %al,-0x4`，ORIG 4.1 直读 `movzbl 0xc(%ebp),%edx`）+ if-conversion `cmova`
// （-march=i486 才消，本波不改路由表）+ N7 store 寄存器对调（ORIG 0x0830ed1e）。
#include "CBattle_Field/set_dungeon_diff.inc"
}

#include "CBattle_Field/setBloodState.inc"
}

#include "CBattle_Field/GetTournamentCurRound.inc"
}

// [E05 R2] P321 T3 消费端判据：bool（ORIG 0x08307656 `movzbl; ret` + 调用点直判）。
#include "CBattle_Field/IsFirstEnterDungeon.inc"
}

#include "CBattle_Field/ResetFirstEnterDungeon.inc"
}

#include "CBattle_Field/getLayeredMapIndex.inc"
}

#include "CBattle_Field/setLayeredMapIndex.inc"
}

#include "CBattle_Field/getLayeredMapKeyValue.inc"
}

#include "CBattle_Field/getLayeredMapInfoIndex.inc"
}

#include "CBattle_Field/getCurrentLayeredMapInfo.inc"
}

#include "CBattle_Field/IsKilledAllHellGruoups.inc"
}

#include "CBattle_Field/SetRidableObjectState.inc"
}

#include "CBattle_Field/CheckAppearRidableObject.inc"
}

// CBattle_Field::GetFieldMob —— ORIG 0x0830a708（0x10a 字节）
//   报告：docs/class_func_reports/CBattle_Field/GetFieldMob.md
//   迭代器先默认构造（P28：it = find(...) 经临时中转拷贝）；两次独立
//   GetCurrentMapInfo()（P48 无 CSE）；找到则经块作用域临时 map_monster
//   拷贝给 out（tmp 析构先于 return true，对齐 ORIG 的 mov $1 在 dtor 后）。
//   【b28 封顶】指令数 72=72、帧 sub $0x380 全等；残差为局部槽基深差：4.1 把
//   迭代器/临时对象顶到帧底（-0x35c/-0x36c/-0x358…），4.4 恒自 ebp-8 起
//   （-0x10/-0xc/-0x18…）⇒ 全帧槽号平移 0x34c（N95/N82 族，P182 不强凑；
//   double framePad 仅调 ±4 无法位移 0x34c）。语义闭合，N 封顶勿再耗轮。
// [b28d-solo-CBattle_Field] NEAR/N82附 槽分区封顶（72=72，帧 sub $0x380 全等）：
// 4.4 聚合对象优先低槽（map_monster 临时 -0x35c）+标量分区（it/end -0x10/-0xc）
// vs 4.1 生命周期穿插（-0x354/-0x35c/-0x358）；P190 double framePad 仅 ±4
// 无法位移 0x34c（ORIG 0x0830a708）。
#include "CBattle_Field/GetFieldMob.inc"
}

#include "CBattle_Field/getBloodMaxRound.inc"
}

// CBattle_Field::BloodRoundMaxTimePacket —— ORIG 0x08305dc4（0xd8 字节）
//   报告：docs/class_func_reports/CBattle_Field/BloodRoundMaxTimePacket.md
//   header 0xd6 = 大轮+1、最大时间、秒数；经 m_party 广播。
#include "CBattle_Field/BloodRoundMaxTimePacket.inc"
}

// CBattle_Field::getNextRoundRandomDifficulty —— ORIG 0x08306156（0x5f 字节）
//   报告：docs/class_func_reports/CBattle_Field/getNextRoundRandomDifficulty.md
//   大轮序==最大轮-1 → 1；否则掷 100，<=49 → 2，否则 1。
#include "CBattle_Field/getNextRoundRandomDifficulty.inc"
}

// ============================================================================
// 嵌套类型方法（ORIG 弱符号）
// ============================================================================

#include "CBattle_Field/TournamentInfo.inc"
}

#include "CBattle_Field/Clear.inc"
}

bool CBattle_Field::TournamentInfo::operator<(const TournamentInfo& other) const
{
    if (m_group < other.m_group)
        return true;
    return false;
}

#include "CBattle_Field/setBloodRoundOrder.inc"
}

#include "CBattle_Field/setPreMapBloodRoundOrder.inc"
}

#include "CBattle_Field/incBloodRoundOrder.inc"
}

#include "CBattle_Field/getBloodRoundOrder.inc"
}

#include "CBattle_Field/setBloodSmallRoundOrder.inc"
}

#include "CBattle_Field/incBloodSmallRoundOrder.inc"
}

#include "CBattle_Field/getBloodSmallRoundOrder.inc"
}

#include "CBattle_Field/setBloodLastSmallRoundOrder.inc"
}

#include "CBattle_Field/getBloodLastSmallRoundOrder.inc"
}

#include "CBattle_Field/reset_blood_values.inc"
}

// CBattle_Field::reset_blood_values @ 0x08306d86
// 清空血条分配向量、重生计数/时间数组并复位血条状态字段。
#include "CBattle_Field/reset_blood_values_1.inc"
}

#include "CBattle_Field/stRewardItem_t.inc"
}

#include "CBattle_Field/reset.inc"
}

#include "CBattle_Field/CBloodClearRewardData.inc"
}

#include "CBattle_Field/reset_1.inc"
}

#include "CBattle_Field/onStartBloodRound.inc"
}

#include "CBattle_Field/addPlayTime.inc"
}

#include "CBattle_Field/getPlayTime.inc"
}

#include "CBattle_Field/getBloodRoundClearTime.inc"
}

#include "CBattle_Field/getCurrentBloodRound.inc"
}

#include "CBattle_Field/setCurrentBloodRound.inc"
}

#include "CBattle_Field/getRewardItemCount.inc"
}

#include "CBattle_Field/getRewardItem.inc"
}

#include "CBattle_Field/getRewardExp.inc"
}

// b28 P211 翻案（DIFF 6→IDENTICAL）：字段 int→unsigned int 与 exp 形参同型化
// 后 store 展开翻为 LHS-first（this 于 idx 之后即载，`lea 0x4(%edx),%ecx` 并入
// scaled 寻址），与 ORIG 0x085bf144 逐条全等；旧「add vs lea」判词系字段异型
// 所致的装载方向差，非 ISel 伪差。
#include "CBattle_Field/setRewardExp.inc"
}

#include "CBattle_Field/setRewardItem.inc"
}

#include "CBattle_Field/setBloodClear.inc"
}

#include "CBattle_Field/isBloodClear.inc"
}

#include "CBattle_Field/getUltimateRoundClearPoint.inc"
}

#include "CBattle_Field/incUltimateRoundClearPoint.inc"
}

#include "CBattle_Field/CTowerOfDespairRewardData.inc"
}

#include "CBattle_Field/dtor_CTowerOfDespairRewardData.inc"
}

#include "CBattle_Field/reset_tod.inc"
}

#include "CBattle_Field/stConditionEnterBossRoomStatus_t.inc"
}

#include "CBattle_Field/stConditionEnterBossRoomStatus_t_1.inc"
}

#include "CBattle_Field/dtor_stConditionEnterBossRoomStatus_t.inc"
}

CBattle_Field::stConditionEnterBossRoomStatus_t&
CBattle_Field::stConditionEnterBossRoomStatus_t::operator=(
    const stConditionEnterBossRoomStatus_t& other)
{
    m_monsterIdx = other.m_monsterIdx;
    m_pos = other.m_pos;
    m_curKillCount = other.m_curKillCount;
    m_maxKillCount = other.m_maxKillCount;
    return *this;
}

// [b28d-Main 代落·S324 P299/P317] 用户 ctor/dtor 定义删除（声明端同步）：
// 隐式 ctor 由使用点发射 W（含 m_pos 的 pair 默认构造），隐式 trivial dtor 的
// D1/D2 由 new_allocator::destroy 发射 W——与 ORIG 0830ee4c / 08316adc 同形。

// ============================================================================
// 外部依赖（CGameManager.h 与本头组合存在重定义冲突，按 ORIG 符号做最小声明，
// 链接解析到真实定义 —— 与 QuickParty.cpp 同一模式）
// ============================================================================
class CGameManager
{
public:
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
};
CGameManager* G_CGameManager();

// WongWork::CDeathTower 最小完成类型（CUser.h 仅前置声明；非虚方法，仅用 this）。
namespace WongWork
{
class CDeathTower
{
public:
    int genTimerKey(TIMER_MESSAGE msg);   // ORIG 0x8142c46 域内（见 CGameManager.h 声明）
    int getIdx();
};
}

// ============================================================================
// CBattle_Field::getRandomBuffDungeonType —— ORIG 0x0830aaa4（0x1d5 字节）
//   报告：docs/class_func_reports/CBattle_Field/getRandomBuffDungeonType.md
// ============================================================================
// [b28d-solo-CBattle_Field] 已修 AE（138=138，call 目标/槽号/分支全等）：
// ①P48/P13 两次独立 G_CGameManager 链调用 + end 具名缓存；②const 引用视图
// 取 const begin/end（P96/P270，消转换构造）；③P0/P48 两次独立
// get_blood_dungeon_type（去具名 int blood）；④(signed char) cast 复现
// `setg`（ORIG 0x0830aaa4）。
#include "CBattle_Field/getRandomBuffDungeonType.inc"
}

// ============================================================================
// CBattle_Field::setEplpWaitingTimer —— ORIG 0x0830ac7a（0x17b 字节）
//   报告：docs/class_func_reports/CBattle_Field/setEplpWaitingTimer.md
//   type 3/6 → 绝望之塔 timer(0x4d)；4/5 → party timer(0x25)；其余 → party
//   timer(0x15)。秒数均 0x1e。
//   （N 封顶：ndiff 93 残差为分支判定调度（cmp $4,jge vs $3,jle）+ 三段
//   InsertTimer 链 esi/ebx 跨调用保存与帧差异，源码已按 ORIG 比较边界写 >=4，
//   GCC 仍规范化，无法逐字节复现——见报告）
// ============================================================================
// [b28d-solo-CBattle_Field] ORIG 0x0830ac7a：帧 sub $0x30（push esi/ebx）已对齐；
// key 直落 %esi（无 ebx 中转）⇒ 去 static_cast（P41 变体）与形参 register（P248）。
#include "CBattle_Field/setEplpWaitingTimer.inc"
}
