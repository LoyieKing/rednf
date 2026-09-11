// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDeathTower/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r 还原 —— WongWork::CDeathTower（死亡之塔）
// 基于 ORIG 反汇编逐函数实现语义。
//
// [b28d-solo-CDeathTower 泳道判词] 本轮修 5 件至 identical/AE（证据 = ORIG 指令形，p167 全 CLEAR）：
//   _onStartDeathTower（0x08467bec enum 视角 m_bloodStateE）、CDeathTowerRanking::init
//   （0x084680ab 去 register + 0x084681d6 去 rec 缓存 + P66 unsigned 形）、handleDieUser
//   （0x08465a53 break 出循环 + 0x08465989 P225 嵌套正向 if）、onLeaveUser（0x084637b1
//   getMemberNames 返回值内联实参 + 0x0846382f P52 链式 + 去 pad5）、CStage::makeStagePacket
//   （0x0846184x P226 裸语句表达式 + 0x08461982 ++it 移体尾）；另 C1/D1（Main 代落
//   CPlayData throw() + CStage 去 throw()，0x82a763f landing 只析构 ~CDungeonMgr）
//   与 CDeathTowerRanking::C1（Main 代落 stTowerBlock 二维数组化，ORIG 0x082a787c
//   内层倒计数 5）同归 AE。余件判词见各函数头（N82/N93/N95/N104/N115 族封顶）。
#include "CGameManager.h"
#include "CUser.h"
#include "CParty.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "TimerQueue.h"
#include "LogManager.h"
#include "CInventory.h"
#include "CCirculationStatistic.h"
#include "CDataManager.h"
#include "CMap.h"
#include "CBattle_Field.h"    // handleStartGame 读取内嵌 BF 随机 buff 类型
#include "GameWorld.h"
#include "CBossStage.h"
#include "CDungeonEntranceLog.h"
#include "CDeathTowerRanking.h"
#include "CMonster.h"
#include "CItemGenerator.h"    // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CDeathTowerWinPointEvent.h"
#include "Secu_HackLogCheckByParty.h"   // MSG_MONSTER_DIE 完整定义
#include "CStatisticServerProxy.h"    // statistc_proxy::add（handlePickupItem 随机选项统计）
#include "CMonitorServerProxy.h"      // updateTowerRank（_beginTowerClearProcess）
#include "GlobalData.h"               // s_monitor_proxy_mgr
#include "CServerProxyMgr.h"          // GetServerProxy 完整定义
#include "MonsterIndependentDrop.h"        // CAICharacter 完整镜像（destroy 需 delete）

// ---- 死亡塔统计/记录接口（定义见 Store.cpp / OnlyOrigBatch14.cpp / W8LT_DBTask.cpp）----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_1 = 1,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};
class CDeathTowerValueStatistic
{
public:
    // ORIG 0x0860f342 返回 bool（PRETTY_FUNCTION "bool ..."，true/false 分支）
    bool SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field,
                                      char a, char b, unsigned int c);
};
extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic();
#include "CDeathTowerPlayDataStatistic.h"          // SendDeathTowerPlayInfo 完整类型
extern CDeathTowerPlayDataStatistic* GetInstanceDeathTowerPlaydataStatistic();
class DB_UpdateDeathTowerBestRecord
{
public:
    static void makeRequest(unsigned int characNo, unsigned int accId,
                            unsigned int memberCount,
                            const stDeathTowerRecordMemberInfo_t* members,
                            unsigned int stage, unsigned int playTime);
};

#include "CDeathTower/makePickupItemHeader.inc"


// ---- CAICharacter / CAICharacterList ----
// CAICharacter 权威定义见 MonsterIndependentDrop.h（+0x6c m_name / +0x70 m_rarity /
// sizeof 0x74 + static_assert）。本 TU 原 #ifndef 兜底镜像已删（§4.4 单声明点：
// 该头先于本处 include 且已定义 DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED，
// 原镜像长期为失效死代码，删除零 codegen 影响）。

#include "CDeathTower/get.inc"

#include "CDeathTower/dtor_CAICharacterList.inc"

#include "CDeathTower/destroy.inc"


// ---- stMapMonsterKillChecker_t / map_item / map_monster ----
// 权威定义见 WarField.h（经 CGameManager.h → PvP_Room.h → PvP_deps.h 链包含），
// 本地重复定义已删除（§7 单声明点）。方法定义保留在下方（归属 WarField.h 声明）。

#include "CDeathTower/getUseSkillMaterialTime.inc"

#include "CDeathTower/dieMonster.inc"
{
    // ORIG 0x830e74e..0x830e78a 逐条核对：m_lastMobDieTime=tick 仅在超限
    // 重置分支内（0x830e77d），cnt++ 分支 fall 后直 return；<= 极性使 4.4
    // 复现 cnt++ fall / 重置块后置的块序。
    if (tick - m_lastMobDieTime <= limit)
    {
        m_uncontinuallyMobDieCnt++;
    }
    else
    {
        m_uncontinuallyMobDieCnt = 0;
        m_lastMobDieTime = tick;
    }
    return m_uncontinuallyMobDieCnt;
}

#include "CDeathTower/initMonsterInfo.inc"
{
    // ORIG 0x830e71c：仅清零 +0x08
    m_uncontinuallyMobDieCnt = 0;
}

class TimerQueue;
TimerQueue* G_TimerQueue();

// CDungeonEntranceLog（CDungeonEntranceLog.h 权威声明）/ WongWork::CDeathTowerRanking
// （CDeathTowerRanking.h 权威声明）直接调用。

namespace WongWork {

// CDeathTowerRanking 方法实现（权威声明见 CDeathTowerRanking.h；本 TU 为唯一定义点，
// CUser.cpp 的本地同名类/桩已删除）。表对象即本类布局（m_towers[5][6] 每塔 0x390
// =6×0x98，槽 1 = 榜首；m_tableBest@+0x7058、m_tableType@+0x70f0），一律具名字段访问。

#include "CDeathTower/makeDungeonIdx2TowerIdx.inc"


#include "CDeathTower/getRankTable.inc"


#include "CDeathTower/checkRenewMyRecord.inc"


#include "CDeathTower/checkRenewTopRecord.inc"


#include "CDeathTower/getVirtualRank.inc"


#include "CDeathTower/unregistBestRecord.inc"


#include "CDeathTower/unregistRanking.inc"


#include "CDeathTower/init.inc"


#include "CDeathTower/CDeathTower.inc"


#include "CDeathTower/dtor_CDeathTower.inc"

#include "CDeathTower/getIdx.inc"


#include "CDeathTower/getTowerState.inc"


#include "CDeathTower/setIdx.inc"

#include "CDeathTower/setTowerState.inc"


#include "CDeathTower/reset.inc"


#include "CDeathTower/_destroy.inc"


#include "CDeathTower/getCDungeonMGr.inc"


#include "CDeathTower/getCStage.inc"


#include "CDeathTower/attachParty.inc"


#include "CDeathTower/genTimerKey.inc"


#include "CDeathTower/_genTimerKey.inc"


#include "CDeathTower/_checkTimerKey.inc"


#include "CDeathTower/_checkMemberDie.inc"


// ORIG rodata 0x8c635f0/0x8c635f4（nm：_ZL37CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT /
// _ZL36CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT，各 4B r）：4.1 以内存读引用
// （_checkMemberReady 0x8467122/0x8467129 `mov 0x8c635f0/4,%eax`）⇒ volatile 阻止
// 4.4 常量化为立即数；值 120/90 沿用既有转写。
static volatile const int CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT = 120;
static volatile const int CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT = 90;

#include "CDeathTower/_checkMemberReady.inc"


#include "CDeathTower/onLeaveUser.inc"


#include "CDeathTower/_beginTowerClearProcess.inc"

#include "CDeathTower/handleKillMonster.inc"

#include "CDeathTower/_onClear.inc"

#include "CDeathTower/_onFinishDeathTower.inc"


#include "CDeathTower/_onFinishStage.inc"


#include "CDeathTower/_onPrepareFinishTower.inc"


#include "CDeathTower/_onStartDeathTower.inc"


#include "CDeathTower/_onStartStage.inc"


#include "CDeathTower/CStage.inc"


#include "CDeathTower/dtor_CStage.inc"


#include "CDeathTower/reset_1.inc"


#include "CDeathTower/onStartStage.inc"


#include "CDeathTower/onFinishStage.inc"


#include "CDeathTower/getStageClearTime.inc"


#include "CDeathTower/getStageMapIndex.inc"


#include "CDeathTower/getCurrentStage.inc"


#include "CDeathTower/_pickupItemMoney.inc"

#include "CDeathTower/_pickupItem.inc"


#include "CDeathTower/deathTowerCardStatistic.inc"

#include "CDeathTower/_makeDropItem.inc"


#include "CDeathTower/_checkRenewMyRecord.inc"


#include "CDeathTower/_checkRenewTopRecord.inc"


#include "CDeathTower/_checkStartGameCondition.inc"

#include "CDeathTower/handleStartGame.inc"


#include "CDeathTower/reset_2.inc"


#include "CDeathTower/initDungeonMgr.inc"


#include "CDeathTower/getStageMap.inc"


#include "CDeathTower/getEndStage.inc"


#include "CDeathTower/handleFinishLoading.inc"


#include "CDeathTower/handleDieUser.inc"


#include "CDeathTower/handleDropItem.inc"


#include "CDeathTower/killMonster.inc"


#include "CDeathTower/peekItem.inc"


#include "CDeathTower/handleLeaveUser.inc"


#include "CDeathTower/handleMoveMap.inc"


#include "CDeathTower/handlePickupItem.inc"


#include "CDeathTower/handleSelectEPLPCommand.inc"

#include "CDeathTower/handleStageCommand.inc"


#include "CDeathTower/getState.inc"
#include "CDeathTower/checkClearStage.inc"



#include "CDeathTower/setState.inc"


#include "CDeathTower/moveNextStage.inc"


#include "CDeathTower/pickupItem.inc"


#include "CDeathTower/dropItem.inc"


#include "CDeathTower/consistMap.inc"


#include "CDeathTower/handleUseStackable.inc"


#include "CDeathTower/onTimer.inc"


#include "CDeathTower/getRewardItem.inc"


#include "CDeathTower/getRewardItemCount.inc"


#include "CDeathTower/getRewardExp.inc"


#include "CDeathTower/getLastRoutedTurn.inc"


#include "CDeathTower/setLastRoutedTurn.inc"


#include "CDeathTower/getStartMemberInfo.inc"


#include "CDeathTower/getDungeon.inc"


#include "CDeathTower/resetCurrentStage.inc"


#include "CDeathTower/makeStagePacket.inc"


#include "CDeathTower/reset_3.inc"


#include "CDeathTower/reset_4.inc"


#include "CDeathTower/getStartMemberCnt.inc"


#include "CDeathTower/makeStartMemberInfo.inc"


#include "CDeathTower/resetMemberReady.inc"


#include "CDeathTower/setMemberAlive.inc"


#include "CDeathTower/isMemberAlive.inc"


#include "CDeathTower/setMemberReady.inc"


#include "CDeathTower/isMemberReady.inc"


#include "CDeathTower/checkEnterStartMap.inc"


#include "CDeathTower/checkAllMemberReady.inc"


#include "CDeathTower/addPlayTime.inc"


#include "CDeathTower/getPlayTime.inc"


#include "CDeathTower/getLastPlayTime.inc"


} // namespace WongWork
