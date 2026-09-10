// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDeathTower/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ---- P327 GI 键名键控：本 TU 第一个函数定义 ----
// ORIG `_GLOBAL__I__ZN8WongWork11CDeathTower14CPacketHandler20makePickupItemHeaderEtt`
// @0x0846991d 的静态初始化体 0x084698dd 仅 `std::ios_base::Init __ioinit`（无用户
// 全局动态初始化对象）⇒ 键名回退到 TU 内第一个函数定义。ORIG 该函数即
// CPacketHandler::makePickupItemHeader（T 0x0845fc6c），故其定义必须位于本文件
// 所有函数定义之前（原定义在 WongWorkDeathTower_A.cpp，P327「定义放错 TU」迁回）。
// ORIG T 0x0845fc6c：header(0,0x27)；short(b)；short(a)；不 finalize。
#include "CDeathTower/makePickupItemHeader.inc"
}

// ---- CAICharacter / CAICharacterList ----
// CAICharacter 权威定义见 MonsterIndependentDrop.h（+0x6c m_name / +0x70 m_rarity /
// sizeof 0x74 + static_assert）。本 TU 原 #ifndef 兜底镜像已删（§4.4 单声明点：
// 该头先于本处 include 且已定义 DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED，
// 原镜像长期为失效死代码，删除零 codegen 影响）。

#include "CDeathTower/get.inc"
}
// ORIG dtor 0x08373d9e：destroy() 后按逆序析构各子对象（成员析构由编译器生成）。
#include "CDeathTower/dtor_CAICharacterList.inc"
#include "CDeathTower/destroy.inc"
}

// ---- stMapMonsterKillChecker_t / map_item / map_monster ----
// 权威定义见 WarField.h（经 CGameManager.h → PvP_Room.h → PvP_deps.h 链包含），
// 本地重复定义已删除（§7 单声明点）。方法定义保留在下方（归属 WarField.h 声明）。

#include "CDeathTower/getUseSkillMaterialTime.inc"

#include "CDeathTower/dieMonster.inc"
}

#include "CDeathTower/initMonsterInfo.inc"
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
}

#include "CDeathTower/getRankTable.inc"
}

#include "CDeathTower/checkRenewMyRecord.inc"
}

#include "CDeathTower/checkRenewTopRecord.inc"
}

#include "CDeathTower/getVirtualRank.inc"
}

#include "CDeathTower/unregistBestRecord.inc"
}

#include "CDeathTower/unregistRanking.inc"
}

// ORIG _ZN8WongWork18CDeathTowerRanking4initEj @ 0x084680a4（0x1ec 字节）。
// this = getRankTable(type) 表；对 5 座塔 × 槽位 1..5 重置 stBestRecord_t：
//   m_flag90(+0x90) 清字节（ORIG movb）；m_memberData[0](+0x04) memcpy 默认名
//   "AAAAAA"/.../"EEEEEE"（ORIG 0x8c63370..0x8c6338d，7 字节含 NUL）；
//   m_field94(+0x94)=槽序号、m_playTime(+0x8c)=0、m_stage(+0x88)=200-槽序号
//   （ORIG 三条 dword 写，顺序 0x94→0x8c→0x88）；
// （this=表对象，ORIG Init 0x082a1f69..0x082a1f75 实证 getRankTable 结果作
// this 传入；体内不调 getRankTable）最后按 ORIG 顺序 m_tableType(+0x70f0)=type、
// m_tableBest.m_stage(+0x70e0)=200（ORIG 0x1d3 先写 type、0x1dc 再写 200）。
#include "CDeathTower/init.inc"
}

#include "CDeathTower/CDeathTower.inc"
}

// [b27 封顶] D1 残差（33 vs 43）= CStage 析构 EH 双份 epilogue（4.4 恒发
// ~CStage cleanup 块 + ebx/esi 保存，4.1 merge 为单块）——N91 同根（C1 同判
// 见 ctor 注），无源码解。
// [b28d-solo-CDeathTower] verdict=DIFF（orig 33 / our 41，P167：EXTRA CStage::D1 ×1）：
// 同 C1 根因（CPlayData dtor 未 throw()，定义在 WongWorkDeathTower_A.cpp）；
// throw() 实测负例已回退（见 C1 判词）⇒ 4.4 在 CPlayData D1 调用点布 CStage D1
// cleanup、ORIG 无 ⇒ N91 封顶。
#include "CDeathTower/dtor_CDeathTower.inc"

#include "CDeathTower/getIdx.inc"
}

#include "CDeathTower/getTowerState.inc"
}

#include "CDeathTower/setIdx.inc"
}
#include "CDeathTower/setTowerState.inc"
}

#include "CDeathTower/reset.inc"
}

#include "CDeathTower/_destroy.inc"
}

#include "CDeathTower/getCDungeonMGr.inc"
}

#include "CDeathTower/getCStage.inc"
}

#include "CDeathTower/attachParty.inc"
}

#include "CDeathTower/genTimerKey.inc"
}

#include "CDeathTower/_genTimerKey.inc"
}

#include "CDeathTower/_checkTimerKey.inc"
}

#include "CDeathTower/_checkMemberDie.inc"
}

// ORIG rodata 0x8c635f0/0x8c635f4（nm：_ZL37CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT /
// _ZL36CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT，各 4B r）：4.1 以内存读引用
// （_checkMemberReady 0x8467122/0x8467129 `mov 0x8c635f0/4,%eax`）⇒ volatile 阻止
// 4.4 常量化为立即数；值 120/90 沿用既有转写。
static volatile const int CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT = 120;
static volatile const int CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT = 90;

#include "CDeathTower/_checkMemberReady.inc"
}

#include "CDeathTower/onLeaveUser.inc"
}

// [b28d-solo-CDeathTower] verdict=DIFF（orig 608 / our 603，P167 CLEAR 103/103）。
// [b28d-69] P167 CLEAR（ORIG calls=103 = OUR 103）：P183 每使用点重发
// getDungeon()->get_index()/getCurCharacNo()/getStartMemberCnt()（ORIG 11/9/6/2 次实证）
// + P0 去 partyState/key/mgrIdx 缓存 + P197③ playTime 预清 + P184 内层死块照抄
// + 2×min<unsigned>+1×min<int>（P211/P265）。ndiff 566→407，orig 608 / our 603；
// 残余仅 4.4 帧 0xdc vs ORIG 0xcc 的槽号整体 -0x10 平移 + 实参跨 call 落槽 vs
// ORIG callee-saved（N82/N95/N7 族）⇒ 语义完成、byte 层 N 封顶。
#include "CDeathTower/_beginTowerClearProcess.inc"
}
// [b28d-solo-CDeathTower] verdict=DIFF（orig 1041 / our 1065，P167 CLEAR 149/149）：
// 语义完成、byte 层 N 封顶（残差 = 4.4 帧 0x5a4 vs ORIG 0x53c 槽号平移 + 实参跨
// call 落槽，N82/N95/N7 族）。P167 CLEAR（ORIG calls=149 = OUR 149，0 missing/0 extra）。
// 本轮修 11 项语义缺口：GetPremiumInfoList 改按值返回版（ORIG 0x0846496e）；premium
// 循环三次独立 operator[]（P157）；dropItemIds 改 unsigned long 左值 push_back(const&)
// + const_iterator 具名对（0x08464d63）；掉落项存 stGenerateResult_t::m_dropItems
// （C1 0x08464ae8 / D1 x2）；dropList/result 声明提前到 a5 赋值前；PacketGuard 改嵌套
// CPacketHandler（C1 0x08469a8c）；put_binary 改 const char* 重载；emblemInfo 循环外
// 单次 init 复用；get_party_type 改 m_battleField.m_bloodState（真语义修正，
// 0x08464dXX `*(int*)(*(int*)this+0xcd8)!=1`）；去掉 dungeon 判空（P140/P171）。
// 剩余 ndiff 596 = 4.4 帧 0x56c vs ORIG 0x53c 的槽号整体平移 + 实参跨 call 落槽
// （N82/N95/N7 族）⇒ 语义完成、byte 层 N 封顶。
#include "CDeathTower/handleKillMonster.inc"
}
#include "CDeathTower/_onClear.inc"
}
#include "CDeathTower/_onFinishDeathTower.inc"
}

#include "CDeathTower/_onFinishStage.inc"
}

#include "CDeathTower/_onPrepareFinishTower.inc"
}

#include "CDeathTower/_onStartDeathTower.inc"
}

#include "CDeathTower/_onStartStage.inc"
}

// ---- CStage ----
// ORIG W 0x82a74ca：MapInfo ctor（编译器生成）后 resetCurrentStage()。
// [b28d-solo-CDeathTower] 去 throw()（Main 代落头 CDeathTower.h:88/91 同步）：
// ORIG C1 landing 0x82a763f 只调 ~CDungeonMgr（= CStage ctor 抛出时的 cleanup）
// ⇒ ORIG 的 CStage ctor/dtor 可抛，加 throw() 会把该 landing 一并消掉（净负）。
#include "CDeathTower/CStage.inc"
}

// ORIG W 0x82a74ec：仅 MapInfo dtor（编译器生成）。
#include "CDeathTower/dtor_CStage.inc"
}

#include "CDeathTower/reset_1.inc"
}

#include "CDeathTower/onStartStage.inc"
}

#include "CDeathTower/onFinishStage.inc"
}

#include "CDeathTower/getStageClearTime.inc"
}

#include "CDeathTower/getStageMapIndex.inc"
}

#include "CDeathTower/getCurrentStage.inc"
}

#include "CDeathTower/_pickupItemMoney.inc"

}
#include "CDeathTower/_pickupItem.inc"
}

// ORIG T 0x0846966c：卡片统计（每卡 AddValueStatistic 0xb/0xc + 死亡之塔
// ValueStatistic 上报，字段 8/2/7/10；isNotTower/level 参与上报参数）。
// [b28d-solo-CDeathTower] verdict=AE（orig 173 / our 173，P167 CLEAR 24/24）：修法 =
// price 预清 + GetSellItemPrice 移到 citem 判空之前（ORIG 0x0846978X 为无条件调用，
// 判空在其后）；P184 死块 = 两个独立 `cmpl $0,-0x1c(%ebp); je`（外层/内层各一）。
#include "CDeathTower/deathTowerCardStatistic.inc"
}
#include "CDeathTower/_makeDropItem.inc"
}

#include "CDeathTower/_checkRenewMyRecord.inc"
}

#include "CDeathTower/_checkRenewTopRecord.inc"
}

#include "CDeathTower/_checkStartGameCondition.inc"
}
#include "CDeathTower/handleStartGame.inc"
}

// ---- CDungeonMgr ----
#include "CDeathTower/reset_2.inc"
}

#include "CDeathTower/initDungeonMgr.inc"
}

#include "CDeathTower/getStageMap.inc"
}

#include "CDeathTower/getEndStage.inc"
}

#include "CDeathTower/handleFinishLoading.inc"
}

#include "CDeathTower/handleDieUser.inc"
}

// [b28d-solo-CDeathTower] verdict=AE（orig 130 / our 130，ndiff 0）：P167 CLEAR 17/17。
// 修法 = P0 内联 `(unsigned short)dropIdx`（去 idx16 具名槽）：ORIG 0x08465a.. 为
// `mov -0xc(%ebp),%eax; movzwl %ax,%ebx` 直读截断，具名槽令 4.4 多一次 mov+槽写
// 并使全帧槽号平移 4（N82 族）⇒ 内联后逐条全等。
#include "CDeathTower/handleDropItem.inc"
}

#include "CDeathTower/killMonster.inc"
}

#include "CDeathTower/peekItem.inc"
}

#include "CDeathTower/handleLeaveUser.inc"
}

// [b28d-solo-CDeathTower] verdict=DIFF（orig 252 / our 249，ndiff 223）：P167 报 MISSING
// _Unwind_Resume ×4，按符号边界计数（compare_tu 口径）**ORIG 6 处 vs 我们 2 处**。
// ORIG 的 4 个额外 landing 各自先调 CPacketHandlerD1Ev（0x8465cfe/0x8465d80/0x8465f8b/
// 0x8466004）再 _Unwind_Resume；4.4 把 try 块内多个 CPacketHandler 局部合并为共享
// cleanup 出口（N104 EH 块合并/相对序族）。Main A/B 实测：CPacketHandler ctor/dtor
// 加 throw() 对本函数零影响（两态均同计数），throw() 已回退 ⇒ N91/N104 封顶。
// 语义/异常路由（catch(int) → makeCommonError(0x30,e)）与 ORIG 一致。
#include "CDeathTower/handleMoveMap.inc"
}

// [b28d-solo-CDeathTower] verdict=DIFF（orig 400 / our 418，NDIFF=145），P167 CLEAR（44/44）：残差 = 4.4 帧
// 0x… 槽号整体平移 + _pickupItem/appendPickupItem 实参跨 call 落槽 vs ORIG
// callee-saved（N82/N95/N7 族）。语义（金钱/物品/avatar 三路 + 统计）全等。
#include "CDeathTower/handlePickupItem.inc"
}

#include "CDeathTower/handleSelectEPLPCommand.inc"
}
#include "CDeathTower/handleStageCommand.inc"
}

#include "CDeathTower/getState.inc"
#include "CDeathTower/checkClearStage.inc"
}


#include "CDeathTower/setState.inc"
}

#include "CDeathTower/moveNextStage.inc"
}

#include "CDeathTower/pickupItem.inc"
}

#include "CDeathTower/dropItem.inc"
}

// [b28d-69] P167 CLEAR（ORIG calls=134 = OUR 134）：补 m_mapInfo.m_mapIndex =
// map->get_index()（ORIG 0x0846063f）；去 aiList 缓存（P183：G_CDataManager 每使用点重调）；
// push_back 右值化（P270 EOj）；find 第三参 cast unsigned；getAppearancePoint /
// getStandardLevel / getItemGenerator 重调（P48 无 CSE）；mons/items 去 const
// （非 const begin/end + _List_const_iterator 转换 C1×2）；specialItems 改 const 指针
// （const at）；第四段循环条件改 m_party->m_battleField.m_bloodState!=1
//（消 get_party_type EXTRA，ORIG 0x0846136d 实证）+ _mapItem src 具名局部（C1 符号）。
#include "CDeathTower/consistMap.inc"
}

#include "CDeathTower/handleUseStackable.inc"
}

#include "CDeathTower/onTimer.inc"
}

#include "CDeathTower/getRewardItem.inc"
}

#include "CDeathTower/getRewardItemCount.inc"
}

#include "CDeathTower/getRewardExp.inc"
}

#include "CDeathTower/getLastRoutedTurn.inc"
}

#include "CDeathTower/setLastRoutedTurn.inc"
}

#include "CDeathTower/getStartMemberInfo.inc"
}

#include "CDeathTower/getDungeon.inc"
}

#include "CDeathTower/resetCurrentStage.inc"
}

#include "CDeathTower/makeStagePacket.inc"
}

#include "CDeathTower/reset_3.inc"
}

// ORIG 0x08461cb4：m_itemCnt=0；10 个 Inven_Item reset 后 itemIdx(+0x02) 置 -1。
#include "CDeathTower/reset_4.inc"
}

#include "CDeathTower/getStartMemberCnt.inc"
}

#include "CDeathTower/makeStartMemberInfo.inc"
}

#include "CDeathTower/resetMemberReady.inc"
}

#include "CDeathTower/setMemberAlive.inc"
}

#include "CDeathTower/isMemberAlive.inc"
}

#include "CDeathTower/setMemberReady.inc"
}

#include "CDeathTower/isMemberReady.inc"
}

#include "CDeathTower/checkEnterStartMap.inc"
}

#include "CDeathTower/checkAllMemberReady.inc"
}

#include "CDeathTower/addPlayTime.inc"
}

#include "CDeathTower/getPlayTime.inc"
}

#include "CDeathTower/getLastPlayTime.inc"
}

} // namespace WongWork
