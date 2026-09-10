// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_AssaultPlaceDepsA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ pvp_assault::CAssaultPlace 前半 30 个 ORIG-only 方法（Wave6）。
// 组：W6_AssaultPlaceA（符号清单 /tmp/only_orig/pvp_assault.txt 第 24-53 行）。
// 全部语义取自 docs/class_func_reports/pvp_assault__CAssaultPlace/ 对应报告
// （汇编 + Ghidra C）；每函数前注释 ORIG 地址/尺寸。
// [b28d-solo-W6_AssaultPlaceDepsA 收口] 清单 9 件：_AvailableAssault/StartAssault/
//   TimeoutConnectP2P 三件修至 AE（负极性包裹 P59 / P227 name 内联升格 /
//   P0 删 log 具名槽 + P223 双标签守卫岛 + 语句表达式 setne）；其余 6 件封 N
//   （EndAssault N82附、RevivalUser N104、_ProcessChaos N7/P75/N121、
//   _SendHoldVictory N98/N82、EvadeAssaultPlace N18b/N7/N108、
//   TimeoutEndAssault N18a），逐条实测数据见各函数头判词。
// [G5 2026-09-10 · L12 round2 组5 复核] 6 件全核：p167 逐件核对，EvadeAssaultPlace CLEAN，
//   其余 MISSING=EXTRA 同数（C1/C2、D1/D2 同址别名）⇒ **无语义缺口**。
//   · _SendHoldVictory（NEAR）：ORIG s=-0x3c / member=-0x38 / name=-0x34 ↔ 我方
//     s=-0x38 / member=-0x34 / name=-0x30 ⇒ **整体平移 4**（N126/N95 族，非相对序差，
//     判据⑪ 判定后不再做声明重排）。
//   · _ProcessChaos / EvadeAssaultPlace：ORIG `call; mov 0x8(%ebp),%edx;
//     mov 0x124(%edx),%edx; cmp %edx,%eax` ↔ 我方 eax↔edx 角色互换（N7/N22；交换操作数
//     在 CHCT/TimeoutEndAssault 已实测反劣）。
//   · TimeoutEndAssault：残余 1 条 regmove（`mov 0x10c(%eax),%eax; mov %eax,%ebx`）；
//     `(ENUM_ASSAULT_STATE)(unsigned int)` 替代形仍单段（P265 在本后端不成立）⇒ N18a。
//   · EndAssault（NEAR）：槽集合两侧全等、相对序差（P309 头部声明试验已证无效）。
//   · RevivalUser：仅 _ZTI/全局 reloc 显示差 + 1 条，AE 后等价。
#include "CAssaultMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_event_manager / s_power_manager / s_systemTime_
#include "TimerDispatcher.h"      // TimerCloseAssault/TimerEndAssault/TimerConnectP2PAssaultTimeout
#include "CPowerManager.h"        // IsPowerWarEventOn / IsAbleToRevival / GetPowerWarLog(CPowerWarLog)
#include "CGuildServerProxy.h"    // CEventBase::IsEventing（vtable+0x34 槽）
#include "GameWorld.h"            // G_GameWorld / send_all / send_to_vill / get_disposition
#include "CDisconnectDetecter.h"  // disconnect_detecter::RegisterUser
#include "CDataManager.h"         // PowerParameterScript::GetPenalty/GetBreakBonus (G_CDataManager()+0xa628)
#include "Village.h"

namespace pvp_assault
{

// ---------------------------------------------------------------------------
// ORIG T 0x82e7162 (size 0x61)：_GetTeamCount(ENUM_ASSAULT_TEAM)
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_GetTeamCount.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7288 (size 0x71)：_GetTeamCount(int*, ENUM_ASSAULT_STATE)
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_GetTeamCount_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e90a0 (size 0x6b)：_MakeChangeHp(PacketGuard*, int, int)
// 包 0x88：uid(short)+hp(int)
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_MakeChangeHp.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e910c (size 0x35)：_MakeGiveAlarm(PacketGuard*) —— 裸包 0x8b。
// 无判空防御（P140：ORIG 切片 0x082e910c 无 `cmpl $0,0xc(%ebp); je`，重建
// over-guard 已删；两调用点 152/364 均传局部 &packet，语义无损）。
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_MakeGiveAlarm.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e88a0 (size 0x35)：_MakeEndAssault(PacketGuard*) —— 裸包 0x84
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_MakeEndAssault.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebd64 (size 0x41)：_IsProcsssChaos() —— 事件未进行时 true
// （GetRepeatEvent(0x20)→vtable+0x34 槽 IsEventing；!=1 时返回 true）
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_IsProcsssChaos.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebaee (size 0x09)：_IsChangeDieState() —— ORIG 恒返 1
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_IsChangeDieState.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e82a2 (size 0x5d)：_SetReadyState() —— 场状态置 2 + 全员 state 1
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_SetReadyState.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e91fa (size 0x83)：DoConnectP2P() —— 场状态置 3 + timer 15s
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/DoConnectP2P.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e804a (size 0x4d)：_BackupPartyInfo(CParty*, ENUM_ASSAULT_TEAM)
// this+(team*4)+0x12c —— 即 m_parties[team]（PartyInfo::Set 写指针）
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_BackupPartyInfo.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7ac4 (size 0x65)：_SendGiveAlarm() —— MakeGiveAlarm → SendPacket
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_SendGiveAlarm.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7918 (size 0x6d)：_SendUserInfo() —— UserInfo(USERINFO==1) 广播
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_SendUserInfo.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e919a (size 0x5f)：_CheckAssaultable() —— state5 双队各>0
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_CheckAssaultable.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e949c (size 0x65)：ClearBadP2PUser()
// 反复求 min/max 并按 min 清坏连接槽，直至区间收敛
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/ClearBadP2PUser.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebaf8 (size 0xa3)：_ChangeDieState(int)
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_ChangeDieState.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebb9c (size 0xc7)：_ProcessChaos(int killer, int victim)
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF 封 N（P321-T5）：已修 P226 语句表达式
//   sete 守卫 + P248 档② bool 字段（m_chaosFlags）+ `>0` 极性（ORIG 0x082ebd32
//   `setg` 复现）；残差 5 组全为装载序/寄存器角色，T5 禁改类型：
//   ① 0x082ebbd2 `cmp %edx,%eax` GetTeam 角色互换（删 (int) cast 无效；交换
//      操作数引 push %ebx 劣化已回退）——N18b/N7；
//   ② 0x082ebc38 `mov -0xc,%eax` 先于 `movl $0xa,0x4(%esp)`（4.4 常量先）——P75；
//   ③ 0x082ebc63 make_basic_info `lea→eax; movl $0; mov %eax,0x4(%esp); mov this,%eax`
//      求值序（4.4 先载 this）——P75；
//   ④ 0x082ebd51 send_all 的 `mov -0xc,%edx` 在 G_GameWorld call 之后（4.4 提前
//      升格 %ebx 跨 call）——P52/N7；
//   ⑤ 4.4 尾部多 2 条 jmp（if/else+外层块尾，ORIG 直落 epilogue）——N121 族。
//   p167 CLEAR。ORIG 0x082ebb9c
#include "W6_AssaultPlaceDepsA/_ProcessChaos.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e9502 (size 0x49f)：StartAssault()
// 返回 1=成功开战（ebx 结果），0=转结束流程。
// PowerWar 计数段依赖 CPowerWarLog（WAMgr 组权威头）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P223 循环岛 + P221 trace 临时链 +
//   P227（BLUE/RED 的 name 内联进 operator() 实参 ⇒ 4.4 复现 ORIG 0x082e9835
//   `mov %eax,%ebx` 升格保活，消 OURS-only mov×2）⇒ 297=297 ndiff=0；
//   p167 CLEAR 53=53。ORIG 0x082e9502
#include "W6_AssaultPlaceDepsA/StartAssault.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e99a2 (size 0x133)：EndAssault() —— 单播结算包 + 清场计时
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] NEAR 封 N（P321-T5）：P223 双标签 goto 消循环
//   nop 岛 + 头部声明 i/allDead 使 i=-0x10/allDead=-0x9 归位后 88=88；残差
//   N82附——4.4 聚合对象优先占低槽：PacketGuard -0x24 vs ORIG -0x1c、
//   aliveCounts -0x18/-0x14 vs ORIG -0x24/-0x20。P309 头部声明 aliveCounts
//   （无初始化+后置赋值）实测无效（4.4 仍给块内 PacketGuard 低槽），已回退。
//   p167 CLEAR。ORIG 0x082e99a2
#include "W6_AssaultPlaceDepsA/EndAssault.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ea004 (size 0x183)：TimeoutEndAssault()
// state==4 才推进到 5 并 EndAssault。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF 封 N（P321-T5）：P227 全表达式临时链复现
//   ctor 前右起 varargs 求值 + P285 正向包裹消 epilogue 尾岛后，仅剩 N18a——ORIG
//   0x082ea099 `mov 0x10c(%eax),%eax; mov %eax,%ebx` 两段 regmove 被 4.4 折单段
//   （`mov 0x10c(%eax),%ebx`）。S1 一次替代形 `(ENUM_ASSAULT_STATE)(unsigned int)`
//   实测仍单段（已恢复直证形）；m_state int→enum 属 .h 跨 TU 改动（S2 归 Main）。
//   p167 CLEAR。ORIG 0x082ea004
#include "W6_AssaultPlaceDepsA/TimeoutEndAssault.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e9502 系列使用的 enemy 局部逻辑已并入上面 StartAssault；
// 本段以下为 ConnectP2P/CompleteLoad/Revival/Evade/Timeout 等。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ORIG T 0x82eb956 (size 0x197)：ConnectP2P(CUser*, char*, int)
// ---------------------------------------------------------------------------
// [b28d-62] AE：正向 if 包裹（P257/P285）消除体尾 `jmp+nop` 中继；p167 CLEAR；
//   108=108。ORIG 0x082eb956
#include "W6_AssaultPlaceDepsA/ConnectP2P.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82eb774 (size 0x1e1)：CompleteLoad(CUser*)
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P223 双标签 goto 消 nop 岛 + P0 删 slotUser
//   槽 + P59 正向包裹（allLoaded）⇒ 131=131 逐条对齐；p167 CLEAR。ORIG 0x082eb774
#include "W6_AssaultPlaceDepsA/CompleteLoad.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ea716 (size 0x183)：TimeoutConnectP2P()
// 超时尚未 P2P 连接的参战者逐一登记/通报，随后重试开战；若期间某空场转失败
// （state 变 6）则提前返回 0。
// ---------------------------------------------------------------------------
// [b28c-L7][SIGFIX:pvp_assault::CAssaultPlace::TimeoutConnectP2P] char→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...TimeoutConnectP2P()"，tsv 第 16 行；
// mangled 不变；`started` 局部改 bool——ORIG 尾 `movzbl -0x19(%ebp),%eax` 无
// setne 归一化 + 消费点 `cmpb $0x0,-0x19(%ebp); je`，P292/P321-T3 判 bool）。
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P0 删 log 具名槽（改链式直传，槽号一次
//   全对齐）+ P227 name 内联 + P223 双标签 goto 复现三条守卫岛（Empty 的
//   jne→nop 岛 + 语句表达式 setne 守卫 + body 尾 jmp）⇒ 262=262 ndiff=0；
//   p167 CLEAR。ORIG 0x082ea716
#include "W6_AssaultPlaceDepsA/TimeoutConnectP2P.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ed43e (size 0x1b5)：RevivalUser(CUser*, int&)
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF 封 N（N104，P321-T5）：`||` 直接物化 +
//   uid 内联（P0）后逐条对齐，仅剩 N104 EH 尾块相对序——ORIG 正常路径 PacketGuard
//   D1+epilogue 在函数最末、unwind 块居中；4.4 把 unwind 块压最末、正常路径尾多
//   `jmp` 跳过（块内容逐条全等，仅两尾块互换，ndiff 17 系 difflib rotate 放大）。
//   p167 CLEAR。ORIG 0x082ed43e
#include "W6_AssaultPlaceDepsA/RevivalUser.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82eb49c (size 0x145)：EvadeAssaultPlace(CUser*)
// ---------------------------------------------------------------------------
// [b28d-62][b28d-solo-W6_AssaultPlaceDepsA] DIFF 封 N（P321-T5）：N18b/N7——
//   ORIG 0x082eb564 `call GetTeam; mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx;
//   cmp %edx,%eax`（enum 留 %eax、int 字段装 %edx）；4.4 恒中转 `mov %eax,%edx`
//   并把字段装 %eax。删 (int) cast、交换操作数序（引 push %ebx + 帧 0x28→0x24
//   劣化）两形实测均不可翻；当前 clean diff 仅剩该 1 组（ndiff 7），历史 N108
//   守卫岛已由前轮 P223 形消解、不再残差。p167 CLEAR。ORIG 0x082eb49c
#include "W6_AssaultPlaceDepsA/EvadeAssaultPlace.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ecaee (size 0x2cd)：_SendHoldVictory(CUser*, CUser*, int)
// 包 0x109：双方连破名单广播（无队用单名，有队 4 成员名）。
// ---------------------------------------------------------------------------
// [b28d-62][b28d-solo-W6_AssaultPlaceDepsA] NEAR 封 N（P321-T5）：N98——ORIG 4 处
//   strlen 为 4.1 内联 word-at-a-time vs 4.4 恒 call（p167 EXTRA 4×strlen）；
//   且 N82 槽序——ORIG s1=-0x3c/member=-0x38/name=-0x34/len=-0x30、s2=-0x20，
//   4.4 按首次使用序使 s1=-0x38 等整体浅 4（-0x3c 被后用的 member2 占用）；
//   声明序/头部声明两形实测均不闭合（N98「4.4 槽按首次使用分配」）。
//   p167 CLEAR。ORIG 0x082ecaee
#include "W6_AssaultPlaceDepsA/_SendHoldVictory.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ec89c (size 0x251)：_VictoryBonus(CUser*, char)
// 返回 penalty + breakBonus。阵营判定同 getPowerSide(char side)。
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_VictoryBonus.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e8098 (size 0x209)：_SeparateTeam(CUser*, CUser*)
// 双队长各自队伍列出并布阵。依赖 _AddUser/_MoveUser/_MoveAssaultPlace/
// _SendUserUdpIpPort（B 组交付）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：users[2] 头部零初始化（ORIG 0x082e80a1 双
//   movl $0 证据）+ party 槽复用 + at 内联实参 + 去三目 ⇒ 167=167 对齐；
//   p167 CLEAR。ORIG 0x082e8098
#include "W6_AssaultPlaceDepsA/_SeparateTeam.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7bbc (size 0x14d)：_DispositionUser(CUser**)
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/_DispositionUser.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e8b66 (size 0xf3)：_MakeUserInfo(PacketGuard*, ENUM_USERINFO)
// 包 0x02：byte(type)+short(count)+逐人 basic_info。
// ---------------------------------------------------------------------------
// [b28d-62] AE：删 P171 多余空指针守卫 + Empty() 正极性 continue + P134 user
//   免槽内联；71=71，p167 CLEAR。ORIG 0x082e8b66
#include "W6_AssaultPlaceDepsA/_MakeUserInfo.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e75ca (size 0xd1)：_AvailableAssault(CUser*, int&)
// 年龄门槛 0xf（PowerWar 中 vill==7 时 0xc）；<0x1e 级未入场检查 → 0xe/0xd；
// 组队分支：party is_limit_minimum_age→0x5f、is_limit_minimum_level→0xff/0xfe。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P0 删 bool/uid 具名槽 + `<= 0x1d`（setle）+
//   P59 正极性 throw 顺序；补负极性包裹 `if (!is_limit_minimum_level(...)) {…}`
//   （ORIG 0x082e77b5 `xor $0x1,%eax; test %al,%al; je <函数尾>`；正极性
//   `if (f()) return;` 4.4 产 test;jne 少 xor）⇒ 149=149 对齐；p167 CLEAR。
//   ORIG 0x082e75ca
#include "W6_AssaultPlaceDepsA/_AvailableAssault.inc"
}

// ---------------------------------------------------------------------------
// ORIG W 0x82f0614 (size 0x6c)：CAssaultPlace::CAssaultPlace()（默认 ctor）
// [SweepCAssaultMgr 迁移自 CAssaultMgr.cpp，Main 2026-09-09 批准；仅搬位置，
//  函数体逐字节不变]
// 依据：ORIG StaticPool<pvp_assault::CAssaultPlace,128>::C1 @0x82f1462（128 insn）
// 含完整 new[] EH cleanup（0x82f14f9..0x82f153d：CNodeD1 循环 + _ZdaPv +
// _Unwind_Resume）；4.4.7/4.1.2 探针 /tmp/p_sp1..p_sp8 实证：T 的默认 ctor 体在
// 池实例化 TU 内可见 ⇒ 判 nothrow ⇒ 省该 cleanup（我方 103 insn）；仅声明（定义
// 在他 TU）⇒ 布 cleanup（128 insn）。CAssaultPlace 池实例化于 CAssaultMgr.cpp
// （CAssaultPlaceFactory::m_pool）⇒ 本定义必须不在该 TU（P284）。
// ---------------------------------------------------------------------------
#include "W6_AssaultPlaceDepsA/CAssaultPlace.inc"
}
}  // namespace pvp_assault
