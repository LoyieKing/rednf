// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyDepsD2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CPartyDepsD2.cpp —— CParty 第二层剩余方法独占恢复（2026-08-24，PartyDepsD2）
//
// 覆盖 26 个 ORIG 符号（除注明 objdump 转写者外，全部经
// docs/class_func_reports/CParty/ 对应报告 ## 汇编 + ## 反编译 C 完整读取后转写）：
//   CheckEnterDimensionDungeon(Rhc)   T 0x0859f36a（CheckEnterDimensionDungeon_0859f36a.md）
//   CheckHackAverageHitCount          T 0x085bb0ac（CheckHackAverageHitCount.md）
//   GetFirstValidMemberState          T 0x085ba574（GetFirstValidMemberState.md）
//   HasAllMemberSelection             T 0x085ba1c0（HasAllMemberSelection.md）
//   OnFailedDungeonClear              W 0x085bf7da（无报告：objdump 转写，转发 OnDungeonClear(user,false)；
//                                                已先读同类调用者 giveup_game/onBloodClear/
//                                                TournamentDungeonClear/dungeon_clear/ClearDungeon 报告）
//   OnSuccessedDungeonClear           W 0x085bf7fc（同上，转发 OnDungeonClear(user,true)）
//   OnStartMapFinishLoading           T 0x085b170a（OnStartMapFinishLoading.md）
//   OnUsedSkill                       T 0x085b5f8e（OnUsedSkill.md）
//   isCheckUseSkill                   T 0x085bb5b0（isCheckUseSkill.md）
//   SaveCoin                          T 0x085b3520（SaveCoin.md）
//   changeBattleDataOnMoveMap         T 0x085a8ee0（changeBattleDataOnMoveMap.md）
//   changePartyMemberInfo             T 0x085a8d1e（changePartyMemberInfo.md）
//   canFatigueUp                      T 0x085a8c88（canFatigueUp.md）
//   changePartyMemberInfoOnMoveMap    T 0x085a8e3c（changePartyMemberInfoOnMoveMap.md）
//   checkMoveMapHackCnt               T 0x085a83a2（checkMoveMapHackCnt.md）
//   check_allmember_loading           T 0x085b17a4（check_allmember_loading.md）
//   check_dungeon_start(CDungeon,Ri)  T 0x0859f7fa（check_dungeon_start.md）
//   getDungeonDropTestFlag            T 0x085bdc76（getDungeonDropTestFlag.md）
//   send_dungeon_fail_msg             T 0x085b35f4（send_dungeon_fail_msg.md）
//   send_to_party_cmd_error           T 0x0859d1aa（send_to_party_cmd_error.md）
//   setStandardDimensionLevel         T 0x0859f612（setStandardDimensionLevel.md）
//   sub_hellparty_value               T 0x085b690a（sub_hellparty_value.md）
//   send_hellParty_value              T 0x085b697c（send_hellParty_value.md；sub_hellparty_value
//                                                的被调方，原全工程无定义，一并闭环）
//   GetMapPlayingTime                 T 0x085b6768（GetMapPlayingTime.md）
//
// 待其他批次闭环的被调方（本 TU 仅按 CParty.h 既有声明调用）：
//   CParty::OnDungeonClear(CUser*,bool)   —— 报告 OnDungeonClear.md 已就绪
//   CParty::do_after_dungeon_start(...)   —— 报告 do_after_dungeon_start.md 已就绪
//
// 头文件类型注记（2026-09-03 已修正）：
//   CBattle_Field+0x1e8/+0x1ec/+0x1f4 = m_hellPartyRatio0/1/3 现为 int
//   （reset_hell_party_value 整数 mov + send/sub/cal/clear_reward fildl 实证）。
//   （原 +0x94 注记经 ORIG 反汇编复核为误标：check_allmember_loading 比较的是
//     CParty+0xcb8 = CBattle_Field+0x194（m_dungeonIndex），已改 cbfField94。）
// ============================================================================

#include <string.h>
#include "TowerOfDespairStatistic_Key.h"  // 权威 Key（CDungeon.cpp 定义 ctor）
#include <vector>

// ORIG T 0x08484a51（InitEnvHelpers.cpp 定义，无权威头声明）
#include "CPartyDepsD2/IsCollectDungeonStatistic.inc"

#include "QuickParty.h"       // 必须先于 CDungeon.h/CParty.h（守卫惯例，与 CParty.cpp 同）
#include "CParty.h"
#include "CGameManager.h"
#include "CUserProc.h"        // APSystem::CUserProc::ClearActionAndSendtoUser
#include "CSecretShop.h"      // secretshop::CSecretShop::LotteryItems
#include "SECRET_SHOP_DATA.h" // SECRET_SHOP_DATA::IsOpen
#include "CItemGenerator.h"   // ::CLuckPoint 完整类型（GetLuckPoint/AddUser）
#include "CStatisticServerProxy.h" // sendPremiumGoldCardStatistic
#include "Quest.h"               // Quest 完整类型（MakeQuestRewardPacket）
#include "GameResultSet.h"     // GameResultSet 完整类型（SetPlayResult）
#include "DisPatcherCore.h"    // DisPatcher_SetPlayResult::SendResult
#include "GoldCardEventStatistic.h"  // Packet_Goldcard_Event_Statistic_GTS m_stat 完整类型
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "Secu_HackLogCheckByParty.h"
#include "PacketTypes.h"          // MSG_MOVE_MAP 完整类型
#include "CCommonStatisticsMgr.h"
#include "CDungeon.h"
#include "CDataManager.h"
#include "W9_EventSmall.h"   // CCharacBloodDungeon::GetEnterCount（check_dungeon_start）
#include "CBoosterGage.h"     // charac_expand::CData（ClearLayer alter）
#include "BestClearTime.h"    // makeBestClearTimePacket
#include "CServerProxyMgr.h"      // CServerProxyMgr<CStatisticServerProxy>
#include "TowerOfDespairMgr.h"    // TOD 域权威（TOD_Layer/APC_Mgr/TowerOfDespairMgr）
#include "DataManagerInitDepends.h" // TOD_UserState（ClearLayer）
#include "CInventory.h"
#include "CWorldMap.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"
#include "GlobalData.h"
#include "RDARScriptStringManager.h"
#include "CBattle_Field.h"
#include "BattleData.h"
#include "CharacManageScript.h"
#include "CPowerManager.h"
#include "CVillageMonsterMgr.h"
#include "WarField.h"             // MapInfo 完整类型
#include "LogManager.h"

namespace
{

// ---- 子对象视图（与 CParty.cpp 同惯例） ----
// 用函数式宏而非 inline 函数：-O0 下 inline 函数参数必分配栈槽，always_inline
// 会先 spill this 再 reload，比 ORIG 直接偏移算术更差（实测 0x826 -> 0x95c）。
// 宏文本替换为 &p->m_memberOffset，直接产生 ORIG 的偏移寻址（check_allmember_loading 报告）。
#define bd(p)         ((p)->m_battleData)   // ORIG DWARF CParty+0x32c 具名化
#define bf(p)         (*reinterpret_cast<CBattle_Field*>(&(p)->m_battleField))
// [b28d-solo-CParty2] CParty.h 字段化：pad 视图 cast → 具名成员。
#define traceHack(p)  ((p)->m_traceMobDieHack)
#define memberUser(p,i) ((p)->m_member[(i)].m_pUser)

// FISTP 取整惯例：ORIG 汇编为 fldcw 0xC（RC=11，truncate 向零截断）后 fistpl，
// 无 +0.5f 的 fadd（实测 checkHackAverageHitCount/sub_hellparty_value/
// send_hellParty_value 三处 ORIG 均为纯截断）。旧定义 +0.5f 系误解，修正。
#define D2_ROUND(x) ((int)(x))

// ---- CCharacBloodDungeon 只读视图（类未建模，同 CPartyDepsC2.cpp 惯例）----
// GetEnterCount W 0x085bfe88 = *(int*)(this+0x18)（loadData 报告证实计数区）。
struct BloodDungeonCountsViewD2
{
    char m_pad0[0x18];
    int m_enterCount;   // +0x18（GetEnterCount 读取）
};

}  // namespace

// ---------------------------------------------------------------------------
// CalcurateManageBonus —— ORIG T 0x085ada30
// 报告：docs/class_func_reports/CParty/CalcurateManageBonus.md
// 返回 st0 浮点；user 为空/角色为空/等级>=最大等级 → 0。
// type=0 → CharacManageScript::GetExpBonus(manageLevel)；
// type=1 → GetGoldBonus(manageLevel) * (double)(uint)param（fildll 无符号扩展）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（67=67）：P0/P200 去 characLevel 具名 short（内联
// movzwl+movswl %ax,%ebx）、P24/P52 去 manageLevel/dataMgr 缓存、P243 乘序改
// (float)param * GetGoldBonus（fildll+fstps 先落槽）⇒ 帧 0x24 逐条全等。
#include "CPartyDepsD2/CalcurateManageBonus.inc"
}

// ---------------------------------------------------------------------------
// CheckEnterDimensionDungeon(CDungeon const*, unsigned char&, bool) —— T 0x0859f1ce
// 报告：docs/class_func_reports/CParty/CheckEnterDimensionDungeon.md
// 错误码：0xab 人数不足且非 GM；0xac 等级不足（写出槽号）；0xad 维度入场券缺失
// （characR+0xeb8+possible 字节 <1 且 partyType!=1 且非 GM）；遍历完返回 1。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（135=135）：(signed char)get_dimension_possible()-1
// 与 (signed char)get_dimension_min_partymem（movsbl 两处）+ P311 具名成员数组
// m_demensionInout[dimIdx]（单条 scaled disp 0xeb9）⇒ 槽 -0x1c/-0x14/-0x10/-0xc 全等。
#include "CPartyDepsD2/CheckEnterDimensionDungeon.inc"
}

// ---------------------------------------------------------------------------
// CheckEnterDimensionDungeon(CDungeon const*, unsigned char&, char) —— T 0x0859f36a
// 报告：docs/class_func_reports/CParty/CheckEnterDimensionDungeon_0859f36a.md
// 以 (m_battleField.m_bloodState, mode, -1) 构造 DimensionPartyCount（ctor W 0x085beabe）询问
// 可容纳入场；可容纳时要求队伍人数 > 计数字节，否则置 false 转发 bool 重载。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/CheckEnterDimensionDungeon_1.inc"
}

// ---------------------------------------------------------------------------
// CheckHackAverageHitCount —— ORIG T 0x085bb0ac
// 报告：docs/class_func_reports/CParty/CheckHackAverageHitCount.md
// 常量实证（objdump rodata）：0x08cbbbfc=10.0f（乘）、0x08cbbc00=0.1f（加）。
// 三组命中样本带外统计过半 → 全体有效成员 addServerHackCnt 0xd9/0xda/0xdb。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 N82/N95（361=361）：已修 ①skip 改 ORIG 短路链
// 寄存器形 `A || !B`（A=pDungeon!=0&&(m_89c!=0||possible>0)，消 skip 槽）
// ②avg/band 三组交错 + float 预清两句式（ORIG -0x5c/-0x54/-0x50 族）。
// 残余：帧 0x94 vs 0x84、avg1 与 int 中转槽互换（ORIG avg1=-0x5c/int=-0x6c vs
// 我方 -0x58/-0x5c）——4.4 槽分配序差异（N82/N95 族）。语义/调用集齐。
#include "CPartyDepsD2/CheckHackAverageHitCount.inc"
}

// ---------------------------------------------------------------------------
// GetFirstValidMemberState —— ORIG T 0x085ba574
// 报告：docs/class_func_reports/CParty/GetFirstValidMemberState.md
// 第一个有效成员的 CUser::get_state()；无有效成员返回 0。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/GetFirstValidMemberState.inc"
}

// ---------------------------------------------------------------------------
// HasAllMemberSelection —— ORIG T 0x085ba1c0
// 报告：docs/class_func_reports/CParty/HasAllMemberSelection.md
// 所有有效成员均完成物品路由选择（GetMemberRoutingState!=0）→ true。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/HasAllMemberSelection.inc"
}

// ---------------------------------------------------------------------------
// OnFailedDungeonClear —— ORIG W 0x085bf7da
// 无独立报告（objdump 转写，已读同类调用者 giveup_game/onBloodClear/
// TournamentDungeonClear/dungeon_clear/ClearDungeon 报告确认消费形态）：
// 3 条指令 thunk → OnDungeonClear(user, false)。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/OnFailedDungeonClear.inc"
}

// ---------------------------------------------------------------------------
// OnSuccessedDungeonClear —— ORIG W 0x085bf7fc
// 无独立报告（objdump 转写，依据同上）：thunk → OnDungeonClear(user, true)。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/OnSuccessedDungeonClear.inc"
}

// ---------------------------------------------------------------------------
// OnStartMapFinishLoading —— ORIG T 0x085b170a
// 报告：docs/class_func_reports/CParty/OnStartMapFinishLoading.md
// 有副本且 do_after_dungeon_start 成功处理 → giveup_game 并返回 false；
// 否则回发 0x28 cmd ok 包并返回 true。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/OnStartMapFinishLoading.inc"
}

// ---------------------------------------------------------------------------
// OnUsedSkill —— ORIG T 0x085b5f8e
// 报告：docs/class_func_reports/CParty/OnUsedSkill.md
// 匹配成员槽累计 BattleData.m_arr7d0[slot]（this+(i+0x2bc)*4+0xc）技能使用
// 次数；isCheckUseSkill(job, skillIdx) 通过再累加 m_field1aa4[slot]。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/OnUsedSkill.inc"
}

// ---------------------------------------------------------------------------
// isCheckUseSkill —— ORIG T 0x085bb5b0
// 报告：docs/class_func_reports/CParty/isCheckUseSkill.md
// 职业×技能黑名单：(0,0x2e)(1,5)(2,4)(3,0xc)(4,1)(5,4)(6,1)(7,5) 及
// skillIdx>0xa8 一律 false。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/isCheckUseSkill.inc"
}

// ---------------------------------------------------------------------------
// SaveCoin —— ORIG T 0x085b3520
// 报告：docs/class_func_reports/CParty/SaveCoin.md
// eCoinSubReason 参数未消费（ORIG 形态）；全员 SaveMoneyCoin。
// ---------------------------------------------------------------------------
// [X2·技巧22 精化形（Main 批准，ORIG 0x085b3520 O@9-12）] callee 结果
// xor-化：绑 %eax 具名 uchar + ^=1 → `call; xorl $1,%eax; testb; jne`。
#include "CPartyDepsD2/SaveCoin.inc"
}

// ---------------------------------------------------------------------------
// changeBattleDataOnMoveMap —— ORIG T 0x085a8ee0
// 报告：docs/class_func_reports/CParty/changeBattleDataOnMoveMap.md
// （BD+0x18/0x1c ↔ CParty+0x344/0x348）；非 firstVisit 时累加 BD.m_field68c
// （CParty+0x9b8）。两次独立取时（ORIG 形态）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/changeBattleDataOnMoveMap.inc"
}

// ---------------------------------------------------------------------------
// canFatigueUp —— ORIG T 0x085a8c88
// 报告：docs/class_func_reports/CParty/canFatigueUp.md
// partyType==1 或无副本 → false；疲劳关闭(m_bloodDungeonType!=0)/血图(m_villageAttackEventFlag==1)/免疲劳副本
// → false；否则 true。flag 参数未消费（ORIG 形态）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/canFatigueUp.inc"
}

// ---------------------------------------------------------------------------
// changePartyMemberInfo —— ORIG T 0x085a8d1e
// 报告：docs/class_func_reports/CParty/changePartyMemberInfo.md
// 存活且未进图者：enterMapFlag(+0x384)/装载标志(+0x388) 任一置位则清
// enterMapFlag，否则累加 BattleData.m_arr64[slot]（this+(slot+0xe0)*4+0x10）；
// 清装载标志后按 msg.m_field17 判定 FatigueUp(0)。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/changePartyMemberInfo.inc"
}

// ---------------------------------------------------------------------------
// changePartyMemberInfoOnMoveMap —— ORIG T 0x085a8e3c
// 报告：docs/class_func_reports/CParty/changePartyMemberInfoOnMoveMap.md
// 非 firstVisit 时：逐张进入型副本仅刷新座位对应成员，否则逐成员刷新。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/changePartyMemberInfoOnMoveMap.inc"
}

// ---------------------------------------------------------------------------
// checkMoveMapHackCnt —— ORIG T 0x085a83a2
// 报告：docs/class_func_reports/CParty/checkMoveMapHackCnt.md
// 反汇编分支边界（+0x57d..+0x5fc）语义：
//   c==0 → 直接到任务豁免段；sec%5==0 → c>sec/5 记疑；sec%5!=0 → c>=sec/5 记疑。
// §6.2 巨件核销：P167 33/33 [0缺0多]；gap 645→629（修前 611）；真修 1 处：
//   任务豁免三段各自独立 call getCurCharacQuestR（修前具名 UserQuest* 缓存
//   合并为 1 call → P167 MISSING 2，P48 反向实证）。
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：N82 全帧
//   槽复用差（0x6c vs 0x8c）、P183 反向 m_mapHitCount 重读 vs -0x50 缓存、
//   kill 的 -0x46 槽缓存 vs ORIG 逐次 movzwl 重读、N-88/setne 物化假象
//   （IsVisitedCurrentMap xor$1 vs setne）、P196 折向（info.m_39 movsbl vs
//   movzbl：槽位值域 0..3 无 ≥0x80 分叉，非真反）——语义与调用序列零缺口。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 13→-1（645 vs 646），p167 33/33 CLEAR。本轮真修：
// ①[真 bug] 技能节奏阈值 q/rem 的基准错 1000 倍——旧码用原始 tick（playTime/5），
//   ORIG 0x085a88e0 读 -0x20 = m_playedTime/1000（0x085a8897 段写入）⇒ 改
//   `secQ = bd(this).m_field8/1000; q = secQ/5; rem = secQ - q*5`；边界比较
//   （rem==0 → c>q 记疑 / rem!=0 → c>=q）经汇编复核与原码一致，保留。
// ②P0/P329 去 lastMap/playTime 具名局部（MapClear 实参内联，ORIG 0x085a8591
//   `mov 0x4(%eax),%esi` 直解引用 + 每次重算 /1000）。③CDungeon::m_89c
//   uchar→bool（SweepCPartyDepsD2-2）后，本函数 0x085a876d 处 `m_89c != 1`
//   由 cmp 形转为 ORIG 的 `movzbl; xor$1; test; je`（指令 645→646）。残余：
//   帧 0x6c vs 0x8c（N82 槽复用差）+ 4.4 continue 中继岛（N-P223）。
//   语义/调用集齐，byte 层封顶。
#include "CPartyDepsD2/checkMoveMapHackCnt.inc"
}

// ---------------------------------------------------------------------------
// check_allmember_loading —— ORIG T 0x085b17a4
// 报告：docs/class_func_reports/CParty/check_allmember_loading.md
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 16→-7（454 vs 461），p167 真缺口归零（仅余
// C1/C2、D1/D2 同址别名 + 1 处 _Unwind_Resume，P93/N104）。本轮真修：
// ①[真缺口] trace 第 2 参由 `m_partyIndex` 直读改 out-of-line `GetPartyIndex()`
//   （ORIG 0x085b17d4 call，非 mov 0x64）；②[真缺口] GetDungeon 单次调用（旧码
//   `if(bf(this).GetDungeon()!=0){CDungeon* d=bf(this).GetDungeon();}` 调两次，
//   ORIG 0x085b1d04 单次存 -0x24）。形态（P0/P24/P227/P292）：首/次循环改
//   `if(!_checkValidUser) continue;` 否定早退、pcRoomTrace/user 具名局部删除、
//   两处 trace 改全表达式临时链（ORIG ctor 前求 varargs）、三处 key 内联实参、
//   acc-tick 表达式内联（LHS-first）、GetPCRoomNo 改空 if 体复现 `test;jmp`。
// 残余：帧 0x9c vs 0xac、flag 形参 4.1 cmpb 直比 vs 4.4 movzbl+xor（P209/N）、
// EH 尾块相对序（N104）、PCRoom trace 两 vararg 寄存器 esi↔ebx 互换（N7）。
#include "CPartyDepsD2/check_allmember_loading.inc"
}

// ---------------------------------------------------------------------------
// check_dungeon_start(CDungeon const*, int&) —— ORIG T 0x0859f7fa
// 报告：docs/class_func_reports/CParty/check_dungeon_start.md
// 返回错误码并写出违规成员槽：血图 10/0x5f/0xf5；m_bloodDungeonType==2 图 0xf4~0xf8；
// 其余 0xf3（人数超限）；通过返回 0。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（242=242）：P183 去 user 指针缓存（逐点
// m_member[i].m_pUser 寻址）、CCharacBloodDungeon::GetEnterCount（include
// W9_EventSmall.h）、(signed char)limit/party_count、三路共享尾 return 0 ⇒
// 槽 -0x24/-0x20/-0x1c/-0x18/-0x14/-0x10/-0xc 与帧 0x34 全等。
#include "CPartyDepsD2/check_dungeon_start.inc"
}

// ---------------------------------------------------------------------------
// getDungeonDropTestFlag —— ORIG T 0x085bdc76
// 报告：docs/class_func_reports/CParty/getDungeonDropTestFlag.md
// 任一 GM 成员在存图模式 → true。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/getDungeonDropTestFlag.inc"
}

// ---------------------------------------------------------------------------
// send_dungeon_fail_msg —— ORIG T 0x085b35f4
// 报告：docs/class_func_reports/CParty/send_dungeon_fail_msg.md
// 插入 TIMER_MESSAGE 0xe 定时器：m_battleField.m_villageMonsterId（CParty+0xc4c
// = BF+0x128）非 0 → 3 秒，否则 10 秒。ORIG 直读 this+0xc4c（无 bf() 调用）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/send_dungeon_fail_msg.inc"
}

// ---------------------------------------------------------------------------
// send_to_party_cmd_error —— ORIG T 0x0859d1aa
// 报告：docs/class_func_reports/CParty/send_to_party_cmd_error.md
// put_header(1, cmd) + byte 0 + byte error + finalize(true) 后全队发送。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/send_to_party_cmd_error.inc"
}

// ---------------------------------------------------------------------------
// setStandardDimensionLevel —— ORIG T 0x0859f612
// 报告：docs/class_func_reports/CParty/setStandardDimensionLevel.md
// 仅 GM 有效成员参与平均：ORIG 中 isGMUser() 结果为 0 时 je 跳到循环尾
// （不累加等级/计数）；成员数为 0 时 ORIG idivl 除零同样崩溃，忠实保留。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（62=62）：五槽照抄（char -0x19 / sum -0x18 /
// avg -0x14 / count -0x10 / i -0xc）+ P292 bool callee 裸 `if(!_checkValidUser)
// continue`（xor$1;test;jne）+ P223 岛形 + isGMUser 正极性（test;je 跳过 movb）
// + avg 具名 ⇒ 帧 0x38 全等。
#include "CPartyDepsD2/setStandardDimensionLevel.inc"
}

// ---------------------------------------------------------------------------
// sub_hellparty_value —— ORIG T 0x085b690a
// 报告：docs/class_func_reports/CParty/sub_hellparty_value.md
// hellValue *= ratio4(CBF+0x1f8)；ROUND 后低于最小值(int@CBF+0x1ec)则钳制；
// 写回 BattleData.m_hellPartyValueTotal(+0xb0c) 后广播。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/sub_hellparty_value.inc"
}

// ---------------------------------------------------------------------------
// send_hellParty_value —— ORIG T 0x085b697c
// 报告：docs/class_func_reports/CParty/send_hellParty_value.md
// put_header(0,0xcf) + put_int(ROUND(hellValue * K / int@CBF+0x1e8)) + finalize。
// 常量 _DAT_08cbbbcc（objdump rodata 8cbbbfc..8cc00）：0x42c80000 = 100.0f。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/send_hellParty_value.inc"
}

// ---------------------------------------------------------------------------
// GetMapPlayingTime —— ORIG T 0x085b6768
// 报告：docs/class_func_reports/CParty/GetMapPlayingTime.md
// 64 位差值 EDX:EAX 返回；调用方（DisPatcher_MoveMap/CheckOneMapPlayTimeHackCnt、
// DisPatcher_SetPlayResult/dispatch_sig）均只消费低 32 位 /1000，
// 故按 CParty.h 既定 unsigned int 声明返回低 32 位。
// ---------------------------------------------------------------------------

#include "CPartyDepsD2/GetMapPlayingTime.inc"
}

// ============================================================================
// == 组队/战场链接缺口批（2026-08-25）——符号清单：
//   /tmp/df_batches/undef_link_syms_2026-08-25.txt 中 CParty 域 9 个 +
//   直接依赖（SetPlayResult 统计三辅助 / historyBloodRoundClear /
//   cal_hellparty_value / get_rate_level_difference / CLuckPoint::AddUser /
//   TOD ClearLayer / TowerOfDespairMgr::Get_APCNameByLayer）。
// 报告均在 docs/class_func_reports/CParty/ 与 objdump（无报告者标注地址）。
// ============================================================================

// ---------------------------------------------------------------------------
// ::CLuckPoint::AddUser —— ORIG T 0x08550932：user 非空时 push_back 到 m_users。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/AddUser.inc"
}

// ---------------------------------------------------------------------------
// CParty::GetLuckPoint —— ORIG T 0x085b61be（报告 GetLuckPoint.md）：
//   有效成员按 flag（false=全部 / true=仅 m_battleData.m_arr54==1）收集到幸运点对象。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/GetLuckPoint.inc"
}

// ---------------------------------------------------------------------------
// CParty::InsertTournamentDungeonTimer —— ORIG T 0x085bd6de（报告
//   InsertTournamentDungeonTimer.md）：0x2e→0x28ms、0x2f→4ms 注册轮转定时器。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/InsertTournamentDungeonTimer.inc"
}

// ---------------------------------------------------------------------------
// CParty::GetMemberSlotNo(unsigned int) —— ORIG T 0x0859acc2：按角色编号
//   （CUser::get_charac_no(-1)）返回成员 CUser*；无匹配返回 0。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/GetMemberSlotNo.inc"
}

// ---------------------------------------------------------------------------
// CParty::VoteLackUser —— ORIG T 0x085b3c90（报告 VoteLackUser.md）：
//   缺人投票（cElection<int,4,4>），投票成功后 0xf 定时器 0x9c4ms。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（91=91）：P221 全表达式临时链翻 sibling 块 trace 槽
// （ORIG -0x30/-0x20 vs 具名形 -0x24/-0x34）+ P24 gen_timer_key 内联实参（消 -0xc
// 槽，key 升格 %ebx 跨 G_TimerQueue call）。语义/调用集齐。
#include "CPartyDepsD2/VoteLackUser.inc"
}

// ---------------------------------------------------------------------------
// CParty::OnTimerWalkOutLackUser —— ORIG T 0x085b3e12（报告
//   OnTimerWalkOutLackUser.md）：按选举结果循环踢出缺人成员。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/OnTimerWalkOutLackUser.inc"
}

// ---------------------------------------------------------------------------
// CParty::GetSetPlayResultCount —— ORIG T 0x085b231e（报告
//   GetSetPlayResultCount.md）：已回传结算成员数。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/GetSetPlayResultCount.inc"
}

// ---------------------------------------------------------------------------
// CParty::GetNoSendSetPlayResultUser —— ORIG T 0x085b2384（报告
//   GetNoSendSetPlayResultUser.md）：未回传成员的昵称；无效返回 ""。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/GetNoSendSetPlayResultUser.inc"
}

// ---------------------------------------------------------------------------
// CParty::SetPlayResult —— ORIG T 0x085b2412（报告 SetPlayResult.md）：
//   结算一个成员：写排行/最佳时间、发送结算、清奖励、统计缺口告警。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 N（ORIG 0x085b2412，N80/N82 族）：
// 已修：①重复 get_dungeon_clear_state 删（ORIG 单次）；②m_field42→m_field48
// 真 bug（ORIG 0x085b251b 写 +0x48=0x183c，旧码写 +0x42 错位）；③resultSet
// 指针局部改 P235 具名视角（消 add $0x17f4）；④trace 改 P227 临时链；
// ⑤SendResult 返回 (unsigned char) 收窄复现 ORIG 0x085b2595 `movzbl %al,%eax`。
// 残余（193 vs 192，gap 1）：
//  ① ORIG 0x085b2457 `mov 0x330(%eax),%edx` 为**死读**（m_battleData.m_field4，
//     值随即被 0x085b245d 的 m_field0 装载覆盖、全函数无消费者）⇒ 4.1 对
//     dead local 保留 load 但不落槽；4.4 三形实测均不可造（探针
//     /tmp/probe_d2.cpp fA/fD 裸表达式+`(void)` 被 DCE；fB/fC 具名/volatile
//     局部多 store；/tmp/probe_d2b.cpp fK/fL volatile 取址多 `add $0x4`）⇒ 封 N。
//  ② SendResult 段 ORIG `lea 0x17f4(%eax),%edx` vs 4.4 `add $0x17f4,%eax`
//     （同址同指令数，union 内无 GameResultSet 完整类型视角可直访；P235 需改
//     CParty.h union 加非平凡成员，风险 > 收益）⇒ N。
// [b28d-SweepCPartyDepsD2-2] 真 bug 修复：trace 的 GetNoSend 实参序原写
// (3,2,1,0) ⇒ (%s,%s,%s,%s) 打印整体颠倒；ORIG 0x085b26a6 段证明 arg3..arg6
// = (0,1,2,3)（-0x64=(3)→arg6 / edi=(0)→arg3）。改源码为 (0,1,2,3) 后
// 4.4 右起求值产 call 3→2→1→0、槽 -0x64/-0x60/-0x5c/edi 与 ORIG 逐条一致
// （原判「varargs 求值序不可翻」作废——两侧同为 cdecl 右起，差异实为实参序）。
// 语义/调用集齐，byte 层封顶（仅余上述残余 ①②）。
// [b28d-round1-L24] **DIFF→AE**（193=193）：残余②的 dead 读真因 = 原码是 64 位
// union 成员的 int 截断 `(int)bd(this).m_clearPlayedTime`（探针 /tmp/probe_ll.cpp
// A 形：`mov 0x4(%eax),%edx; mov (%eax),%eax`，高半进 edx 后丢弃）；旧码
// `(int)bd(this).m_field0` 只读低半 ⇒ 少 1 条。改用 union 视角即逐条全等。
#include "CPartyDepsD2/SetPlayResult.inc"
}

// ---------------------------------------------------------------------------
// CParty::MakeQuestRewardPacket —— ORIG T 0x085bd7ae（报告
//   MakeQuestRewardPacket.md）：任务清图奖励物品包（20 任务槽 × 清图物品表）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 0→-1（295 vs 296），p167 40/40 CLEAR。本轮真修
// （三处 p167 真缺口）：①循环内取任务由 `getCurCharacQuestR`（18 字）改
//   `getCurCharacQuestRefR`（21 字，ORIG `_ZNK5CUser21getCurCharacQuestRefREv`）；
// ②去 dungeonIdx 缓存：ORIG 0x085bd878/0x085bd8e3 两次独立 call
//   get_dungeon_index（P183）；③`itemList[i]`→`itemList.at(i)`（ORIG 调
//   `_ZNSt6vectorISt4pairIiiESaIS1_EE2atEj`，P175；补发 at 实例，AE +2）。
// 既有修复保留：needCount 头部预清、itemList/pairValue 分步声明+拷贝赋值
// （N86 反向）、quest==0 死块照抄（P184）、m_itemList 成员直访。
// 残余：4.4 槽基恒 ebp-8 起 vs ORIG 大局部顶到 ebp ⇒ 全帧槽号 +8 平移
// （ORIG -0x89/-0x44/-0x4c vs 我方 -0x91/-0x4c/-0x54）；N95 定论（double
// framePad 反致下移、int pad/声明序不可上移）。语义/调用集齐，byte 层封顶。
#include "CPartyDepsD2/MakeQuestRewardPacket.inc"
}

// ---------------------------------------------------------------------------
// CParty::OnTimeoutBossDie —— ORIG T 0x085aa5d4（报告 OnTimeoutBossDie.md）：
//   超时击杀当前 Boss（BF.m_apcAllKilledSec）：遍历怪物→击杀→失败按队伍类型收尾。
// [b28d-solo-CPartyDepsD2] gap -1→0（369=369），p167 真缺口归零（仅余 C1/C2、
// D1/D2 同址别名，P93）。本轮真修：①[真缺口] 两处 InsertTimer 的 index 实参由
// `m_partyIndex` 直读改 out-of-line `GetPartyIndex()`（ORIG 0x085aa90d/0x085aaa49
// call）；②[真缺口] 两循环增量 `++it`→`it++` 复现 ORIG ppEi（P28）。
// 形态（P24/P171/P227）：两处 key 内联实参、trace 改全表达式临时链、
// 删两处 `m_pDungeon != 0` 过度防御（ORIG 0x085aa9d7/0x085aaa9b 直接解引用
// 0xcac，无判空）。残余：map_monster 0x34c 临时 vs dieBuf 0xa3f 槽复用方向差
// （ORIG 帧 0xac0；4.4 不复用 ⇒ 0xe10，N82/N86）。语义/调用集齐，byte 层封顶。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/OnTimeoutBossDie.inc"
}

// ---------------------------------------------------------------------------
// CParty::sendPremiumGoldCardStatistic —— ORIG T 0x085aca0a（报告
//   sendPremiumGoldCardStatistic.md）：金卡事件统计包（0x1f46/0x13）上报。
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Packet_Goldcard_Event_Statistic_GTS —— ORIG ctor 0x085bee96 W：
//   PacketHeader(0x1f46,0x13)[10] + GoldCardEventStatistic(+0xa, size 0x9, 总 0x13)。
//   （与 Game/Packet_Goldcard_Event_Statistic_GTS.h 同布局；本 TU 自包含定义。）
// ---------------------------------------------------------------------------
struct Packet_Goldcard_Event_Statistic_GTS
{
    PacketHeader m_header;          // +0x00（packed 0xa）
    GoldCardEventStatistic m_stat;  // +0x0a（ORIG GoldCardEventStatisticC1 构造）

    Packet_Goldcard_Event_Statistic_GTS()
        : m_header(0x1f46, 0x13), m_stat()
    {
    }
} __attribute__((packed));

#include "CPartyDepsD2/sendPremiumGoldCardStatistic.inc"
}

// ---------------------------------------------------------------------------
// CParty::historyBloodRoundClear —— ORIG T 0x085b7db6：逐有效成员写
//   cUserHistoryLog::BloodRoundClear（CUser+0x79700）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（72=72）：真 bug 修正 char[0x40]→char[0x100]
// （P116/P253：ORIG rep stos 的 %edx=0x40 是 dword 计数 = 0x100 字节，旧码少清
// 0xc0 字节且 getMemberNames 写 256B 溢出）+ P0 去 cUserHistoryLog 指针缓存
// （lea 0x79700(%eax),%edx 直压参）⇒ 帧 0x130 全等。
#include "CPartyDepsD2/historyBloodRoundClear.inc"
}

// ---------------------------------------------------------------------------
// CParty::cal_hellparty_value —— ORIG T 0x085b58ac（报告 cal_hellparty_value.md）：
//   地狱值累计（上限钳制 + send_hellParty_value）。
//   +0xd0c..+0xd5b（m_padcee 未建模 hell 值表区）按报告偏移访问。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 NEAR（83=83，gap 1→0；p167 26/26 CLEAR）：
// 已修 P183 去 bf 引用缓存、P252 三数组具名 float 局部、P243 乘序、成员直存
// （fstps 0xb0c 无 value 槽）、比较反向。原「m_89c!=1 的 xor 形不可造」N 判词
// 已作废：CDungeon::m_89c uchar→bool（SweepCPartyDepsD2-2，值域 {0,1} 直证：
// STDungeonScript.cpp:1963 ScanInt((bool*)0)!=0 → CDungeon.cpp:457 拷贝）后，
// 源形 `!m_89c` 与 ORIG 0x085b5915 `movzbl; xor$1; test%al; je` 同形 ⇒
// DIFF→NEAR。残余：三处数组寻址常量拆分（ORIG `add $0x348; mov
// 0x14(%eax,%edx,4)` vs 4.4 `add $0x34c; mov 0x4(...)`），指令数相同，仅常量
// 折叠形差（N94/N4）。
#include "CPartyDepsD2/cal_hellparty_value.inc"
}

// ---------------------------------------------------------------------------
// CParty::get_rate_level_difference —— ORIG T 0x085b59ca（报告
//   get_rate_level_difference.md）：等级差档位（0/1/2/3）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/get_rate_level_difference.inc"
}

// ---------------------------------------------------------------------------
// TOD_UserState::ClearLayer() —— ORIG T 0x0864390e：层推进 + 脏标记 + 清层时间。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/ClearLayer.inc"
}

// ---------------------------------------------------------------------------
// TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex —— ORIG T 0x085ff020：
//   映射命中返回 SendingAPC_Info::get_user_charac_name()，未命中返回 0。
// [b28d-solo-CPartyDepsD2] 判词 N93（N86 族，NEAR 封顶）：P50 复活先声明后赋值
// （`iterator it;` 独立 C1Ev）+ 非 const iterator（P96）后 39=39、助记符序列全等，
// 唯 it/e 双槽方向相反——ORIG 4.1 先声明深（it=-0x10/e=-0xc），4.4 先声明浅
// （it=-0xc/e=-0x10）；交换声明序槽翻正但 C1Ev/end 发射序反（DIFF），两难不可兼得
// ⇒ N93 类对象双槽方向封顶。p167 调用集齐（C1Ev/end/find/ne/ptEv/get_user_charac_name）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/Get_APC_Name_byIndex.inc"
}

// ---------------------------------------------------------------------------
// TowerOfDespairMgr::Get_APCNameByLayer —— ORIG T 0x08643e4a：
//   m_apcMgr 非空时转 Get_APC_Name_byIndex。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/Get_APCNameByLayer.inc"
}

// ---------------------------------------------------------------------------
// TOD_Layer::increaseLayer() —— ORIG T 0x085fe824：m_layer + 1；超过 100 时
//   resetLayer()（m_layer 置 1）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/increaseLayer.inc"
}


// ---------------------------------------------------------------------------
// cUserHistoryLog::BloodRoundClear —— ORIG T 0x08684af0：
//   m_user 非空时 LogHistory("BloodRoundClear,%d,%u,%s", round, clearTime, names)。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/BloodRoundClear.inc"
}

// 注：用 #pragma pack（而非 __attribute__((packed))）——GCC 4.4 的 attribute
// packed 对含非 POD 成员（Key 有 user ctor）的外层 struct 不生效。
#pragma pack(push, 1)
struct Packet_TowerOfDespair_Statistic_GTS
{
    PacketHeader m_header;   // +0x00（packed 0xa 字节）
    TowerOfDespairStatistic_Key m_key;  // +0x0a
    unsigned char m_flag;    // +0x12
    unsigned int m_accId;    // +0x13

    Packet_TowerOfDespair_Statistic_GTS()
        : m_header(0x271c, 0x17)
    {
        m_flag = 0;
        m_accId = 0;
    }
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// CParty::ClearDungeon —— ORIG T 0x085a9330（报告 ClearDungeon.md 逐段转写）：
//   清图流程：普通/绝望塔/巡回分支 → 动作点/统计/奖励 → 时间与地狱值结算。
// ---------------------------------------------------------------------------
// [b28d-SweepCPartyDepsD2-2] 真 bug 修复（虚调用槽取错，Main 全库排查判据命中）：
//   ORIG 0x085a9b6e `add $0x8,%edx` = vptr+0x08 = _ZTVN8WongWork14CItemGeneratorE
//   word[4] @0x08c9a798 = 0x0853a400 = CItemGenerator::generateRandomItem
//   （Ghidra C `(**(code **)(*piVar14 + 8))(piVar14, ref, result)`；同 vtable
//   word[5]=vptr+0x0c=0x0853a406=generateSpecificItem）。旧码调
//   generateSpecificItem ⇒ 运行时选错虚函数。改后本 TU 唯一 `call *` 槽号
//   与 ORIG 逐条一致（本 TU 全量 grep 仅此 1 处间接调用）。
// [b28d-solo-CPartyDepsD2] 判词 N82（1058 vs 1060，§6.2 三件套闭合）：已修
// P167 缺口 ①getManager() 由具名缓存改 4 处逐点重调（P183）②m_partyIndex 改
// GetPartyIndex() 3 处（0x2c/0x22/0x15 timer）③itemList[i].second 改
// item.m_addInfo2 字段直读（消 get_add_info call）④push_back 改传 m_todPos
// 引用（消 pair<int&,int&> 临时 + push_back(&&) 重载）。p167 149/149 CLEAR
// （0 缺 0 多）；残余：帧 0x170 vs 0x1d0 全帧槽平移 + 多分支槽复用方向差
// （N82 巨件族）。语义/调用集齐，byte 层封顶。
// [b28d-round1-L24] 本轮真修（1058→1055→1037，ORIG 1054；gap +4→+1→-17）：
// ①**块序反转**（P222/P272）：`if (m_bloodState == 1) { A; return; } B` —— ORIG
//   0x085a9356 `cmp $1; jne <B(+0x1b0)>` 且 A 尾 `jmp <epilogue>`（0x85a94bd）
//   ⇒ then=A 直落、B 在后；旧码写 `if (!= 1) { B }` 使 4.4 反向。
// ②**64 位 union 直算**（P180/P246）：ORIG 0x085a94e0..0x085a9525 无具名局部，
//   `sub/sbb` 直对 +0x20/+0x24 与 +0x2c/+0x30、双字直存 +0x32c/+0x330 ⇒ 改用
//   BattleData.h 新增 `m_clearPlayedTime`/`m_clearEnterTime` 视角（旧手写
//   `<<32|` 拼接产 40 条 sar/or 展开，净 -39）。
// ③**P183 去 CUser* 缓存**：ORIG 0x085a9589/0x85a95e0/... 六个调用点各自
//   重算 `m_member[slot].m_pUser`（9 条 stride 0x18 寻址）⇒ 净 +38。块序交换
//   （blood 分支 `>= 1` 前置）实测零 codegen 影响（4.4 if/else 两臂同形）。
// 残余（-17）：tower 大段（ORIG[318:782]）与我方发射位置差 ⇒ difflib 大段错位；
//   p167 149/149 CLEAR（0 缺 0 多）；帧 0x170 vs 0x1bc（我方多 push %edi + 槽集大）
//   ⇒ N82/N95 巨件族。语义/调用集齐。
#include "CPartyDepsD2/ClearDungeon.inc"
}

// ---------------------------------------------------------------------------
// CParty::makeBestClearTimePacket —— ORIG T 0x085be1f8：成员击杀榜时长包
//   （isQuestMaze / 成员数 / 每成员 uid+bestTime+newRecord）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/makeBestClearTimePacket.inc"
}

// ---------------------------------------------------------------------------
// DisPatcher_SetPlayResult::SendResult —— ORIG T 0x081f5db6：
//   header 0x22 结算广播（排名/时间/奖励分/最佳时间+新记录）。
// ---------------------------------------------------------------------------
#include "CPartyDepsD2/SendResult.inc"
}
