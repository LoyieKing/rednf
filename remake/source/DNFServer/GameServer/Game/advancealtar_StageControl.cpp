// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 advancealtar_StageControl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 声明唯一归口 advancealtar.h（具名布局：m_index@+0x00 / ProcStage 槽@+0x04 /
// CharacData 槽@+0x98，sizeof 0xc0）；本 TU 全部函数依据
// docs/class_func_reports/advancealtar__StageControl/ 报告翻译。
#include "CGameManager.h"
#include "CInventory.h"  // PacketGuard 完整定义 / CInventory::SendItemLockListInven
#include "CMap.h"  // CMap::get_index/get_monster_list、_mapMonster（sendMapInfo）
#include "GameWorld.h"
#include <cstring>        // std::memset / std::memcpy
#include <new>            // placement new（StageControl 子对象构造）
#include "advancealtar_ProcStage.h"  // ProcStage/CharacData 完整类型（getProcStage/selectStage）

namespace advancealtar
{
// ----------------------------------------------------------------------------
// ORIG T 0x0812f9a2（StageControl.md 全文）：m_index=-1 →
//   ProcStage::ProcStage(this+4, characData@this+0x98)（T 0x812dc18）+
//   CharacData::CharacData(this+0x98)（T 0x812d024）。
// 子对象以字节槽持有（advancealtar.h 布局注释），此处 placement 构造。
// [b28d-solo-StageControl] C1：ORIG 34 / ours 62。ORIG 为真成员直构（`lea 0x98(%eax),
//   %edx` + `add $0x4,%eax` + call ProcStage::C1(CharacData&) + call CharacData::C1，
//   仅 esi/ebx + sub 0x10 帧）。我方字节槽 + placement new ⇒ N83：4.4-O0 恒发
//   `movl $0x94,(%esp); call _ZnwjPv`（size 装载行）+ test/je + 每子对象一段 EH
//   cleanup（~28 条）。修法唯一路径 = StageControl 成员改真实类型，但
//   advancealtar.h → advancealtar_ProcStage.h → NOTIPacketStruct.h → advancealtar.h
//   为双向 include 环（NOTIPacketStruct.h 需 advancealtar::_Stage），成员化须拆环
//   （波及全仓 include CGameManager.h/advancealtar.h 的 TU）。语义等价（m_index=-1 +
//   ProcStage(*characData)@+0x04 + CharacData@+0x98，槽宽恰 0x94/0x28）。N 封顶（N83/P67）。
// [b28d-sweep 2026-09-09 复核] 解环路径已定位（供 Main 决策，本泳道未落）：①把
//   advancealtar::_Stage（advancealtar.h:71-73 + 断言 491）抽到叶子头
//   advancealtar_Stage.h；②NOTIPacketStruct.h 改 include 该叶子头（去 advancealtar.h
//   依赖）；③advancealtar.h 便可 include advancealtar_ProcStage.h，StageControl 成员改
//   真实类型 ProcStage m_procStage / CharacData m_characData。波及面：include
//   advancealtar.h 的 17 个 .cpp（含经 CGameManager.h/CUser.h 的传递消费）须复编；
//   4.4 生成的 EH landing 布局是否与 ORIG 4.1 逐条一致亦需实测（N91 族）。风险与
//   波及超出单 TU 泳道 ⇒ 维持 N83/P67 封顶，路径交 Main 定夺。
#include "advancealtar_StageControl/StageControl.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fa06（~StageControl.md 全文）：先 CharacData::~CharacData(this+0x98)
//   再 ProcStage::~ProcStage(this+0x04)（成员逆序析构）。两处子对象指针均直算
//   `mov this,%eax; add $0x98/0x4,%eax; mov %eax,(%esp)`，ORIG 无具名局部（P0：
//   指针全程寄存器/出参槽直传）→ 此处亦内联 reinterpret_cast，不落 -0xc 槽。
//
// [b28d-solo-StageControl] D1：ORIG 29 / ours 13。ORIG（真成员自动 D1）把
//   CharacData::~CharacData(+0x98) 建 try 区，抛时落点 `mov %edx,%ebx; mov %eax,%esi;
//   ProcStage~D1(+0x04); _Unwind_Resume` 后仍走正常路径 ProcStage~D1 ⇒ push esi/ebx +
//   sub 0x10 帧 + 整段 landing pad。我方字节槽 + 显式析构调用无 EH 清理（P67/N91：
//   4.4 对用户显式析构体不建该 pad）。与 C1 同根因（成员化须拆 include 环，见 C1 判词）。
//   可观察语义（逆序销毁 CharacData→ProcStage，指针/调用序/出参槽逐条同 ORIG）等价。
//   N 封顶（P67/N91）。
#include "advancealtar_StageControl/dtor_StageControl.inc"
}

// ORIG W 0x81348ba：(const) return *(int*)this;
#include "advancealtar_StageControl/getIndex.inc"
}

// ORIG W 0x82a669c：*(int*)this = idx;
#include "advancealtar_StageControl/setIndex.inc"
}

// ORIG T 0x0812fc4e（leaveUser.md 全文）：getUser/getCurCharacR 判空 → 0；
//   [blocker] HistoryLog::leaveDungeon(user, procStage)（报告 0x8133abc 在，
//   源码声明/定义缺，随 ProcStage 域批次）→ out_from_dungeon → 广播
//   basic_info(0,2) → send_itemspace(0) → getCurCharacInvenR().
//   SendItemLockListInven() → setAdvanceAltarIndex(-1)（CUser+0x8d012）→
//   destroy(this) → 1。
#include "advancealtar_StageControl/leaveUser.inc"
}

// ORIG T 0x0812fb98（onTimerStageTick.md 全文）：ProcStage::getState 为 3/4 时
//   CharacData::upGage() + ProcStage::procTimeLine()，恒返回 1。
#include "advancealtar_StageControl/onTimerStageTick.inc"
}

// ORIG T 0x0812fa54（reset.md 全文）：ProcStage::resetAll() + CharacData::reset()。
#include "advancealtar_StageControl/reset.inc"
}

// ORIG T 0x081300ce（docs/class_func_reports/advancealtar__StageControl/destroy.md
// 全文，Prod_GlobalAdv 2026-08-24）：reset(this) → G_CGameManager()->returnAdvanceAltar(this)。
#include "advancealtar_StageControl/destroy.inc"
}

// ORIG W 0x081348ae：return (ProcStage*)(this + 0x04)。
#include "advancealtar_StageControl/getProcStage.inc"
}

// ORIG W 0x081348a0：return (CharacData*)(this + 0x98)。
#include "advancealtar_StageControl/getCharacData.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081300f6（docs/class_func_reports/advancealtar__StageControl/
// sendStageInfo.md 全文）：getUser 判空直接返回；put_header(0,0x209) +
//   put_binary({stageIndex, stageDifficulty}, 8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
// [B24D判词 DONE_AE] 语义正确：getUser 判空返回 + _NotiAdvanceAltarStageInfo(8B)+
//   put_header(0,0x209)+put_binary(&body,8)+finalize(true)+Send。已改用与 ORIG 同源
//   struct 写法（去 body[]+memcpy 二次拷贝，68/68 指令对齐）；仅剩 call/符号地址
//   AE 占位差异。
#include "advancealtar_StageControl/sendStageInfo.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fa7a（docs/class_func_reports/advancealtar__StageControl/
// selectStage.md 全文，Prod_GlobalAdv 批次 2026-08-24）：
//   user 判空 → 3；!isAvailableTciket → 0xf5；!isEnterStage(stageId) → 8；
//   CharacData(+0x98)：m_field0 = getCurrentRidableId()、setUser(user)、
//     setSlotItem()；ProcStage::resetAll → startStage(stageId, diff)；
//   失败透传错误码；成功 → sendStageInfo() → sendMapInfo()【未建模批次跳过】
//     → setNotSurvivalModeFirstRound() → set_state(0xd) → 0。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] selectStage：ORIG 78 / ours 83。已按 P183/P0 去掉
//   getCharacData()/getProcStage() out-of-line call 与具名指针槽（ORIG 每访问点重算
//   this+0x98 / this+0x4），并补 P198① `int err = 0; err = startStage(...)` 预清两句式。
//   残差 = N76：`m_field0 = getCurrentRidableId()` 4.4 恒 LHS-first（地址升格 %ebx 跨
//   call ⇒ push ebx/sub $0x24、+5 条），ORIG 4.1 为 RHS-first 直存 `mov %eax,0x98(%edx)`；
//   P211 档②/③（字段 unsigned 化、RHS (unsigned) cast）实测均不翻（4.4.6 同 LHS-first）。
//   另 setUser/startStage 实参寄存器角色 eax↔edx 互换（P75 跨版本）。语义逐条等价。
// [round1-L29 2026-09-10 复核] 判据① CLEAR。残差 = `mov this,%edx; mov %eax,0x98(%edx)`（ORIG
//   RHS-first）vs 我方 `push %ebx; mov this,%ebx; mov %eax,(%ebx)`（LHS-first+指针缓存，
//   N76/P183 反向不可补）；另 `lea 0x98(%eax),%edx` vs `add $0x98,%eax`（N107 六形探针全败）。
//   ORIG 0x0812fdd4 / 0x0812fe1c。
#include "advancealtar_StageControl/selectStage.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081306be（sendEplp.md 全文）：getUser 判空直接返回；
//   available = user->m_advanceAltar.isAvailableTciket()（CUser+0x8df60）；
//   char isAvailableTicket; pad[3]}（8B，权威定义 NOTIPacketStruct.h）；
//   char isAvailableTicket; pad[3]}（8B，权威定义 advancealtar.h）；
//   put_header(0,0x20e) + put_binary(8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] P183/P0：ORIG 直接 `this+0x98` 调 CharacData::getUser
//   （无 getCharacData thunk call）；isAvailableTciket 结果先落具名 bool 槽 -0x9
//   （`mov %al,-0x9` 直存、body 字段处 `movzbl -0x9; mov %al` 读回），且声明在 body
//   之前 ⇒ 调用序在 body ctor 之前；旧 `? 1 : 0` 三目被物化成 test/je/mov$1/jmp/mov$0。
#include "advancealtar_StageControl/sendEplp.inc"
}

// ORIG T 0x08130644（onEplp.md 全文）：sendEplp(endType) +
//   Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex())
//   （ORIG T 0x0812f956，定义 advancealtar_ProcStage.cpp）。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] onEplp：ORIG 27 / ours 29。
// [round5-A29 2026-09-10 破 N] 真因不是「P85 跨语句 %eax 保活不可造」——ORIG 0x08130644 的
//   `mov 0xc(%ebp),%eax; test %eax,%eax; js END; cmp $0x2,%eax; jle ZERO; cmp $0x3,%eax;
//   je ONE; jmp END` 是 **4.1 对 switch 的决策树**（P163/P324/P286），不是 if 链产物：
//   if 链在 4.4 恒按语句重载形参并折 `cmpl $3,0xc(%ebp)` 内存直比（P85），而 switch 形
//   4.4 同样产「单次载入 + 寄存器比较链」（P286 单 case switch 族）。探针
//   /tmp/dtprobe/flagprobe2.cpp `f5` 与 ORIG 逐条同形（movb 预清 → movl 形参 → testl →
//   js → cmpl $2 → jle → cmpl $3 → je → jmp → movb $0 → jmp → movb $1）。
//   叶块物理序 = case 书写序（P324）：case 0/1/2 的 ZERO 块在 case 3 的 ONE 块之前。
//   StageEndType::T 值域 END_N0..END_N3 含 0，无 P305 越界删 case 风险；
//   `(int)` 显式转换即 f5 的 int 形参口径（同宽零开销）。
#include "advancealtar_StageControl/onEplp.inc"
}

// ORIG T 0x08130692（onEplpReturnVillage.md 全文）：ProcStage::getState()==6 → leaveUser()。
#include "advancealtar_StageControl/onEplpReturnVillage.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081305f6（onClearReward.md 全文）：(-1 < e && 2 < e) && e == 3 时
//   ProcStage::setState(6) + Timer_StageControl::RegistTimerEPLP(getIndex(), 3)
//   （第二参为字面 3，非透传）。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] onClearReward：ORIG 30 / ours 30。P223 多岛形（三条独立否定
//   守卫 goto + 尾标签串 island1/2/3）逐条复现 ORIG 三岛（js/jle/jne 分落 nop+jmp 岛，
//   第三岛直落；body 尾 jmp join 同形）。唯一残差 = 4.4 语句级重载形参多一条
//   `mov 0xc(%ebp),%eax` + 第三比较 `cmpl $3,mem` vs ORIG `cmp $3,%eax`（P85 跨语句
//   %eax 保活；P0 禁具名局部/绑定形）。语义逐值等价。N 封顶（P85 族）。
// [round5-A29 · D08/C05③ 闸门复核，2026-09-10] ORIG 三守卫落 +0x44/+0x47/+0x4a（**三址
//   互异**：+0x44/+0x47 为 `nop;jmp leave` 岛、+0x4a 为直落 `nop`）⇒ 不属「多守卫**同
//   目标**」族；且我方岛数（3）− ORIG 岛数（3）= 0 < 2，未达 Main 的「差≥2 才试」闸门
//   ⇒ **不套用 D08 合并 `||`**（同 TU 内 `_IsInArea` 0x82f0298 才是该钥匙的正向实例）。
// [round1-L29 2026-09-10 复核] 判据① CLEAR；P286 单 case switch 已实测（27→28 条反劣，已回退）。
//   残差同 onEplp（P85 族）⇒ N（ORIG 0x081305f6）。
#include "advancealtar_StageControl/onClearReward.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fbfa（onTimer.md 全文）：msg 0xa9 → onEplp(endType)；
//   0xaa → onEplpReturnVillage()；0xa8 → onClearReward(endType)；其余无操作。
//   TIMER_MESSAGE 0xa8-0xaa 枚举器名待 TimerDispatcher 批次定名，暂用字面值。
// ----------------------------------------------------------------------------
// [b28d-D9] P85/P209：ORIG 线性比较链值全程 %eax；4.4 signed 形参 mem 直比（unsigned 强转实测反劣 29>27）（ORIG 0x0812fbfa sz 0x00000053）
#include "advancealtar_StageControl/onTimer.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fdd4（gotoStage.md 全文；静态语义，asm 不使用 this）：
//   getUserVillage 判空 → cMyTrace(0x585 行, vill) 返回；getArea(get_area(false))
//   判空 → cMyTrace(0x58c 行, area) 返回；否则 erase_user →
//   包1 0x17{short uid, byte vill, byte 0xff, short posX, short posY,
//   byte direction, byte visible_values} Send → clear →
//   包2 3{byte 1, short uid, byte 1} Send + send_all(guard, user) → set_state(0xd)。
// ----------------------------------------------------------------------------
// 头声明 static（ORIG 无 this）；类外定义不带 static。
// [b28d-D9] 槽差/P227：trace 临时链位置 + 槽序差（ORIG 0x0812fdd4 sz 0x000002fa）
#include "advancealtar_StageControl/gotoStage.inc"
}


// ----------------------------------------------------------------------------
// ORIG W 0x081307a6（getCenterCount.md 全文）：map/user 判空 → 0；
 // 遍历 CMap::get_monster_list()（按值返回 list<_mapMonster>），对每只怪
 // getTowerInfo(monster.m_orderIdx, out, out)（+0x2c），命中即计数。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] getCenterCount：ORIG 103 / ours 99。已对齐：P183 去
//   procStage/characData 具名指针槽（直接 this+4 / this+0x98）；P28 增量段 it++（ppEi
//   后置带 sret 临时槽）；P96 非 const iterator（去转换构造临时）；ORIG -0x10 list 指针
//   具名槽（monsterList = &monsters，begin/end 从槽取 this）逐条复现。
//   唯一残差 = hit 取反物化：ORIG `movzbl -0x9,%eax; xor $1,%eax; test %al,%al; jne
//   <skip>; ++count; jmp; nop`（8 条）vs 4.4 `cmpb $1,-0x9; jne; ++count`（P168a④：
//   寄存器 xor 物化无绑定不可达，X2 两语句落栈槽；`hit^1` 被折 mem 直比 N18b）。
//   语义等价（hit!=0 ⇒ ++count）。N 封顶（P168a④/N18b 族）。
// [b28d-sweep 2026-09-09] 复验：归一化后唯一差异仍是上述 4↔1 条；另试 const 容器 +
//   const_iterator（ORIG 0x08135d88/0x08135dae 段确用 `_ZNKSt4list...` const 版）⇒
//   verdict 无改善（仍 DIFF）且本 TU 少发射 iterator→const_iterator 转换构造
//   ⇒ check_tu identical 80→79 回退，已回退维持非 const iterator。
// [round1-L29 2026-09-10 修复] 真因：callee `getTowerInfo` 权威声明**已是 bool**
//   （advancealtar.h:421 / advancealtar_DB.cpp:314），我方却用 `unsigned char hit` 接收 ⇒
//   4.4 折 `cmpb $1,-0x9`。改为 `const bool hit` + P223 goto 尾空标签形
//   （`if (!hit) goto hit_done; ++count; hit_done:;`）⇒ **AE 102/102 ndiff=0**。
//   ORIG 0x08130869 证据形：`mov %al,-0x9; movzbl -0x9,%eax; xor $1,%eax; test %al,%al;
//   jne <skip>; addl $1,-0x14; jmp <join>; <skip>: nop`。
#include "advancealtar_StageControl/getCenterCount.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081301e0（sendMapInfo.md 全文，0416B）：user/map/timeLine 三重判空；
 // _NotiAdvanceAltarMapInfo(0x14 packed){stageType, clearSurvivalRoundNo,
 // mapIndex=CMap::get_index, currentPhaseIndex=timeline+0xc, mapBasisLevel}；
 // put_header(0,0x20a)+put_binary(0x14)；put_int(phaseSummarys.size()) 后逐
 // _PhaseSummary put_binary(0xc)；生存模式且非首轮：put_int(getCenterCount())
 // 并遍历怪物表逐只构造 _MapMonsterSummary（仅 getTowerInfo 命中者
 // addCreatedMapMonster + put_binary(0xd)）；否则 put_int(0)；finalize→Send。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] sendMapInfo：ORIG 293 / ours 290。已修：P183 去
//   getCharacData/getProcStage out-of-line call 与具名 procStage 槽（每点重算
//   this+0x98/this+0x4）；body ctor 移到 map 判空后/getTimeLine 前（ORIG 0x0813026e）；
//   P214① sendMonsters 改直接 `if (a||b)`（无具名 bool 槽）；else 分支具名 zero 局部；
//   Send 前重新 `this+0x98` 调 getUser。
// [b28d-sweep 2026-09-09 重判] 旧判词「-0x20 死计数器/-0x18 list 指针槽/it++ 引入即
//   反劣」**全部作废**——旧读数是在 3 处真 bug + put_binary 重载 + hit 槽未补的前提下
//   测的。本轮修后（clean 口径 ORIG 289 / ours 292）：
//   ①真 bug：`!isSurvivalModeFirstRound()` 极性反（ORIG je/jne 同落 L_true，P224）；
//   ②真 bug：phase 循环 put_binary 误选 PKc 重载（ORIG 三处均 Pc @0x0811df08，P167）；
//   ③真 bug：getTowerInfo 首实参误传 m_orderIdx，ORIG 取 it->+0x00=m_mobId（0x08130454）；
//   ④补 hit 具名 uchar 槽（ORIG 0x0813047c `mov %al,-0xd`，P0）；
//   ⑤phase/monsters 两循环改后置 `it++`（ORIG 调 ppEi，P28 增量侧）；
//   ⑥补 phase 死计数器（ORIG 0x08130308 `movl $0,-0x20` + 每轮 `addl $1,-0x20`，P171）；
//   ⑦补 monsters 具名 list 指针槽（ORIG 0x081303d9 `mov %eax,-0x18`）；
//   ⑧删 cam 具名引用槽（ORIG 每轮重算 lea 0x8df60，P183）；
//   ⑨删原两个撑帧 double（补 ④⑥⑦ 后 4.4 帧自涨到 0xa0，删后回 0x90 与 ORIG 逐字对齐）。
//   残余 = hit 取反物化（ORIG movzbl/xor/test/jne 4 条 vs 4.4 `cmpb $0` 2 条，P168a④
//   /N18b 封顶）+ 槽号分配差（N95/N126 版本常数）。p167：put_binary 差集清零，仅剩
//   D1/D2、C1/C2 同址别名与 list const/非 const 迭代器符号选择差（非语义缺口）。
// [round1-L29 2026-09-10 复核] 我方 292 / ORIG 289；判据① 分支数 66 vs 67，无目标错位。
//   残差含 getTowerInfo 的 X 族（同 getCenterCount，待 callee bool 化）+ 槽号 N95/N126。
//   ORIG 0x081301e0。
#include "advancealtar_StageControl/sendMapInfo.inc"
}

// ---- TimeLine / Phase / TimeLineSummary（ORIG advancealtar__TimeLine 报告）----
#include "advancealtar_StageControl/Phase.inc"
}

#include "advancealtar_StageControl/reset_1.inc"
}

#include "advancealtar_StageControl/TimeLineSummary.inc"
}

#include "advancealtar_StageControl/dtor_TimeLineSummary.inc"
}

#include "advancealtar_StageControl/TimeLineSummary_1.inc"
}

TimeLineSummary& TimeLineSummary::operator=(const TimeLineSummary& other)
{
    m_field0 = other.m_field0;
    m_phaseSummarys = other.m_phaseSummarys;
    return *this;
}

#include "advancealtar_StageControl/reset_2.inc"
}

// TimeLine ctor（ORIG T 0x08899d6e）：vector<Phase> 默认构造 + summary@+0xc 构造。
#include "advancealtar_StageControl/TimeLine.inc"
}

// TimeLine dtor（ORIG T 0x08899db4）：summary 先、vector 后自动析构。
#include "advancealtar_StageControl/dtor_TimeLine.inc"
}

// 拷贝构造（ORIG W 0x0837e642）：vector/summary 成员拷贝。
#include "advancealtar_StageControl/TimeLine_1.inc"
}

TimeLine& TimeLine::operator=(const TimeLine& other)
{
    m_phases = other.m_phases;
    m_summary = other.m_summary;
    return *this;
}

#include "advancealtar_StageControl/reset_3.inc"
}

}  // namespace advancealtar