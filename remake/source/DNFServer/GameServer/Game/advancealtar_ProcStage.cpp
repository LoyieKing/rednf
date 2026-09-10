// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 advancealtar_ProcStage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// advancealtar_ProcStage.cpp —— advancealtar::{ProcStage, CharacData,
// Timer_StageControl, HistoryLog} + getMemberNames 真实实现。
//
// 证据（ORIG 汇编+反编译 C 全文已读）：
//   docs/class_func_reports/advancealtar__ProcStage/{isRewardState,getStageType,
//     reset,resetAll,startStage,setNotSurvivalModeFirstRound,isSurvivalModeFirstRound,
//     getStageIndex}.md
//   docs/class_func_reports/advancealtar__CharacData/setSlotItem.md
//   docs/class_func_reports/advancealtar__Timer_StageControl/{RegistTimerEPLP,
//     RegistTimerClearReward}.md
//   docs/class_func_reports/advancealtar__HistoryLog/{dungeonClearInfo,enterDungeon}.md
//   docs/class_func_reports/advancealtar/getMemberNames.md
//   报告缺失、按门禁以 objdump 转写并标注的 W thunk：
//     CharacData::setUser 0x08134806 / getUser 0x08134814；
//     ProcStage::setState 0x08134844 / getStageDifficulty 0x0813486c /
//     getStartStageTime 0x08134886。
//
// 未建模子树（调用点保留 [未建模批次] 注释，不做伪造）：
//   CharacAdvanceAltarManager::getSlotItem(0x08132638)（setSlotItem 填表；
//   其余 getSlotItem 依赖 BuyUpgradeData::getFieldDataPoint/CDataManager::find_item
//   已在 CDataManager 域建模）、
//   CAdvanceAltarLog::incrementEntranceData / advanceAltarstatistics_（统计日志）。
// ============================================================================

// ============================================================================
// b28d-52 判词表（本 TU 独占；ORIG 地址来自 game_func_report 二进制切片）
// 已修（转 AE/IDENTICAL）：
//   ProcStage::dieMob 0x0812e1de（P214① 汇聚块 + P248② m_padC bool 化）
//   _SummonObjectMs::C1 0x08134674（_SummonObject 迁叶子头 ⇒ 真实子对象 C1，消 N83）
//   HistoryLog::leaveDungeon 0x08133abc（P214③ char[0x100]={0} + P0 实参内联）
//   ProcStage::endProcAllKillMonster 0x0812ea00（P222 正向包裹 + P0 表达式内联）
//   ProcSummonObjectMs::reset 0x0812d43c（P96/P270 const_iterator + P28 后置 ppEi）
// 剩余 DIFF（N 归因）：
//   CharacData::upgreadGage 0x0812d176 —— N85（x87 fildl/flds/fmulp 两向 lowering）
//     + N82（帧 0x28 vs 0x38 槽组平移）
//   CharacData::upGage 0x0812d0b6 —— N85（x87 累加 flds 序；ORIG fildl 直读）
//   HistoryLog::enterDungeon 0x081339b0 —— N78（4.1 memset 内联为对齐剥离+rep stos
//     慢路，4.4 恒 call）
//   HistoryLog::dungeonClearInfo 0x08133bd4 —— N82（帧 0x15c vs 0x168 + 槽组平移）
//   ProcStage::startStage 0x0812de70 —— N82（帧 0x24 vs 0x34，4.4 多 0x10 槽）
//   ProcStage::rewardClearStage 0x0812f0fa —— P214① 汇聚块极性（4.4 反产 sete;test;je）
//   ProcSummonObjectMs::completePushSummonObject 0x0812d5d0 —— 跨版本 && 汇聚物化
//     （4.1 mov$1/mov$0+test；4.4 折直跳）+ signed 形参 mem 直比
//   ProcSummonObjectMs::summonObjectAndSendPacket 0x0812d658 —— P225 X3 变体
//     （ORIG test/sete/test/jne；4.4 cmpl mem 直跳）
//   ProcSummonObjectMs::dieMob 0x0812db00 —— P48（ORIG 两次独立 operator->）
//     + bool 物化（xor $1）不可同时复现
//   ProcStage::sendMailRewardItem 0x0812f47a —— P264 重定位（全局单例立即数 .o 显示 0）
//     + 局部缓冲槽差 3
//   ProcStage::clearStageAndReward 0x0812e3f6 —— N82/N86（迭代器 copy-elision 槽差）
//   ProcStage::addAchievementReward 0x0812f3ae —— P0（manager 指针被缓存成具名局部，
//     ORIG 为 lea 直用）+ 帧差
//   ProcStage::makeCurrentPhaseSummonObject 0x0812eba8 —— N82/N86（大件槽组 + 迭代器）
//   ProcStage::isEndStage 0x0812e2c6 —— P248② 字段 bool 化后 ORIG sete 物化不可复现
//   ProcStage::sendClearStage 0x0812e6de —— N86（迭代器/容器 copy-elision 槽差）
//   ProcSummonObjectMs::debugLog 0x0812d804 —— N89（vector/map 迭代器外链 + trace 变参）
// ----------------------------------------------------------------------------
// [b28d-D03 本波 判词]（fresh `check_tu_routed.sh`：identical=226 ae=436（合计 662）
//   near=0 diff=5 = _Z41__static_initialization…（N123 豁免）+ 下列 4 件；
//   T1b② 槽复用命中 1 件：summonObjectAndSendPacket 见下）
//   · makeCurrentPhaseSummonObject 0x0812eba8 —— 本波三改：①`int spawnMs = m_field10;
//     spawnMs += it->m_firstDelayMs;` 拆两句（ORIG 0x812f054 段 `mov→槽; add %eax,槽`
//     两步，合并写被 4.4 折 lea）②`int create = 1/0`（widthscan：ORIG 全函数 clr_b=0/
//     cmp_b=0 ⇒ 原码 int 非 bool；旧 bool 形产 `movb $1` + `cmpb $0`）③补
//     `so->m_deadFlag = 0;`（ORIG 0x812f020 `movb $0x0,0xc(%eax)` 照抄，P171）。
//     残差 = N82/N86（帧 0xac vs ORIG 0xb0 + 槽组平移 + 我方多 push %edi：总栈两侧同为
//     0xb8）+ N7；`tl/assigns` 具名局部两形历轮实测劣（156→201/474）勿再试。
//   · sendMailRewardItem 0x0812f47a —— shift 探针：**MIXED**，唯一残差 = title/itemBase
//     槽基 1 字节（OURS -0x15b/-0x3bd vs ORIG -0x15c/-0x3be）。本波补试 `char[1]` 与
//     `char = 0` 两种 1 字节前置死局部均无效/反劣（后者整槽组漂移）⇒ 维持 P312 framePad[1]
//     现状，判 N95（槽基 gap 1B，4.4 分配器不可控）。
//   · summonObjectAndSendPacket 0x0812d658 —— **[d03-T1b② 槽复用命中] MIXED→PURE SHIFT
//     c=0**：原 OURS-only[-0x48,-0x20,-0x14] vs ORIG-only[-0x44,-0x1c]（整帧平移 4）⇒ 多开
//     的 4 字节具名局部 = `CUser* user = m_characData->getUser();`（我方-only -0x14）。
//     改内联 `m_characData->getUser()->Send(guard);` ⇒ 槽直方图与 ORIG **完全一致**、
//     指令数 **132→130 = ORIG（130/130）**、TU 四数 ae 435→436、diff 6→5。
//     残差仅 2 条跨版本项：ORIG 的 `call memcpy@plt`（我方 4.4 内联 rep movsl，N84/N78）
//     + ORIG 的 `_Unwind_Resume`（4.4 判 memcpy nothrow 不布 landing，P216/N104 族）
//     ⇒ 封 N84/N78+P216，gap=0。
//   · dungeonClearInfo 0x08133bd4 —— shift：**PURE SHIFT c=0**（帧 0x15c 两侧一致、槽直方
//     图全等）；唯一残差 = 1 条 N18a（ORIG `mov (%eax),%eax; mov %eax,%ebx` = CharacData::
//     m_field0 读两段 vs 4.4 单段 `mov (%eax),%ebx`）。P254 路径（字段 enum 化）需新造
//     enum 类型且写入端在 advancealtar_StageControl.cpp（非本泳道）⇒ 不动，封 N18a。
//     [C05②/C07 试形负例，本波实测] 曾试「字段读改具名 int 局部」（`const int characField0 =
//     control.getCharacData()->m_field0;`）⇒ **反劣**：帧 0x15c→0x150、丢 `push %esi`、
//     槽组整体漂移（-0x12c→-0x11c 等），verdict 仍 DIFF ⇒ 已回滚（该字段读在 ORIG 是
//     寄存器级升格、无槽，具名局部强制落槽）。每件一次旋钮已用，维持 N18a。
//   · clearStageAndReward 0x0812e3f6 —— shift：PURE SHIFT c=0（帧 0x6c 一致）；残差 =
//     N7（esi/ebx/edi 角色互换）+ 4.4 对 `(A||B) && !C` 的 setcc 汇合块序与 4.1 相反
//     （P214①/P225 边界）；P226 条件向语句表达式已把 NDIFF 19→14。
// ============================================================================
// b28d-solo-ProcStage 收口判词（16 件清单；verdict 只认 check_tu_routed.sh + p167）
//   本泳道收口读数：identical=226 ae=435（合计 661）near=0 diff=6
//   （diff 余 = _GLOBAL__ 噪音 + 下列 5 件 N 封顶件）；missing_in_game 169→155。
//   AE/IDENTICAL（11 件，逐条对齐，p167 CLEAR）：
//     addAchievementReward 0x0812f3ae（P0 内联 cam/rewardParam + enum 槽 diff/value）
//     completePushSummonObject 0x0812d5d0（P226 语句表达式 + P171 `size()-1` 照抄 + P285）
//     ProcSummonObjectMs::dieMob 0x0812db00（P48 四次 operator-> + P248② bool 字段 + P171 死局部）
//     CharacData::upGage 0x0812d0b6（P319 索引 cast + 去多余 +0.5f + 两臂各 sendTestCheckGage）
//     CharacData::upgreadGage 0x0812d176（switch 决策树 P251/P286 + P319 + 去 +0.5f）
//     isEndStage 0x0812e2c6（P28 it++ + P222 正向包裹 + P225 嵌套 if sete 物化）
//     HistoryLog::enterDungeon 0x081339b0（P312 慢路 + P0 全实参内联）
//     ProcStage::rewardClearStage 0x0812f0fa（P167 符号族：items const 指针 + const_iterator
//       + `it->m_type`（ptEv 非 deEv）；P28 it++；P197④ size()!=0；P286 单 case switch；
//       NDIFF 189→0、p167 34/34 CLEAR）
//     ProcStage::sendClearStage 0x0812e6de（P28 三循环 it++；put_binary(char*,int) 非 const
//       重载（C cast 去 const，P167 3 处）；rewardCount 具名 size 缓存 + `if (n>0)` 守卫；
//       body.m_stage 逐字段直写（消 memcpy EXTRA）；NDIFF 191→0）
//     ProcStage::startStage 0x0812de70（P211 档③：CDataManager::find_map 返回类型 void*→CMap*
//       （CDataManager.h，已 R2 广播）+ 删本处 cast ⇒ LHS-first 直存；NDIFF 129→0）
//     ProcSummonObjectMs::debugLog 0x0812d804（P167 `it->second`（ptEv×7 非 deEv）；P28 两循环
//       it++；P171 死计数器移入 for 增量段（`it++, ++n`，清零在 begin 之后）；NDIFF 197→0）
//     isSurvivalModeFirstRound 0x0812f70c（P0/N96 清绑 + P226 条件向语句表达式）
//   N 封顶（5 件，语义等价；各件函数头注记残差与实测数据）：
//     dungeonClearInfo 0x08133bd4 —— N18a：余 1 条 `mov (%eax),%eax; mov %eax,%ebx`
//       （ORIG 两段中转 vs 4.4 单段直落）。P254 enum 化路径受阻：CharacData::m_field0
//       无权威 enum 类型且写入端在 advancealtar_StageControl.cpp（非本泳道，已 N76 判词），
//       本波不动。NDIFF 101→2、p167 CLEAR。
//     ProcSummonObjectMs::summonObjectAndSendPacket 0x0812d658 —— N95：全帧槽号整体深 4
//       （4.4 帧顶 gap；prologue sub $0x50 与 ORIG 全等、指令逐条对齐）。已修：P223 空 then
//       岛（`if (p==0) goto L_next;` 复现 ORIG `sete;test;jne <nop岛>`，setne/je 旧形消除）、
//       P48/P157 so 缓存复用（消 1 次 operator[] EXTRA）。NDIFF 119→23。
//     sendMailRewardItem 0x0812f47a —— N82/N95：数组组 1 字节差（title -0x15b vs -0x15c、
//       itemBase -0x3bd vs -0x3be）+ count 装载序（P75）。已修：P170 _RewardItem::m_value1
//       int→unsigned long（AdvanceAltarField.h，R2；make_pair IRKm mangled 实证）、P0/P44
//       group/titleLen/characNo 实参内联、P171 0xf 死局部、P312 framePad[1] 顶数组组、
//       P28 `it++, ++i` 增量段。NDIFF 151→11、p167 CLEAR。
//     clearStageAndReward 0x0812e3f6 —— N7（esi/ebx/edi 角色互换）+ 物化块序（P214①/P225
//       边界：4.4 对 `(A||B) && !C` 的 setcc 汇合块序与 4.1 相反；P226 条件向语句表达式已把
//       NDIFF 19→14）。已修：P48 两次独立 getStageType、P197④ size()!=0、P168a 档① 裸 !decTicket、
//       P0/P183 去 cam 引用局部（ORIG 逐次 lea 内联）。p167 CLEAR。
//     makeCurrentPhaseSummonObject 0x0812eba8 —— N82/N86：帧 0xac vs ORIG 0xb0（少 4）+
//       槽组平移（phase -0x3c vs -0x38）。已修：P167/P96 全套 const 迭代器族（TimeLine const
//       化，头 R2；消 12 处 MISSING/EXTRA）、P220 迭代器哨兵形（两 end() 预求值 + `rangedIt==end()`
//       eq 正极性）、P28 Action 循环 it++、const ActionDefine*（消转换构造）。tl/assigns 具名
//       局部两形实测均劣（NDIFF 156→201/474）已回退。NDIFF 371→156。
//   P0 清绑：isSurvivalModeFirstRound 的 `register bool r __asm__("al")` 已删（改 P226 语句表达式，
//     22=22 AE）。全文件无 register asm 绑定（N96 清零，扫描零命中）。
//   真语义 bug（本波修正）：CharacData::summonSlotUnit 0x0812d278 —— 第二条件误用 union 视角
//     m_needGage（+0x08）；ORIG 0x0812d330 `mov 0x8(%eax)`（eax=pair* ⇒ pair+0x8=second+0x04=
//     m_itemId）+ Ghidra C `if (*(int*)(iVar4+8)==param_2)` 联证 ⇒ 改 `it->second.m_itemId == need`，
//     该件 DIFF→AE（NEAR→AE，p167 8/8 CLEAR）。
//   .h 改动（R2 已广播，均零 ABI：返回/字段类型不入 mangled）：
//     CDataManager.h find_map const 版返回 void*→CMap*；
//     advancealtar_ProcStage.h getTimeLine/ m_timeLine → const TimeLine*；
//     AdvanceAltarField.h _RewardItem::m_value1 int→unsigned long（32 位同宽）。
// ============================================================================

#include "advancealtar_ProcStage.h"

#include <algorithm>  // std::random_shuffle
#include <cstdio>     // sprintf

#include "QuickParty.h"       // 先于 CDataManager/CDungeon 链：QUICKPARTY_H 守卫去重
#include "CDataManager.h"     // G_CDataManager/find_dungeon/find_map
#include "CDungeon.h"         // CDungeon::get_index
// CMap 完整类型（startStage/isEndStage 直取 m_map->m_timeLine@+0xf4 / m_110@+0x110）
#include "CMap.h"
#include "CSystemTime.h"      // CSystemTime::getCurSec
#include "GlobalData.h"       // s_systemTime_
#include "CGameManager.h"     // StageControl 完整声明（dungeonClearInfo 引用）
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整布局（HistoryLog::killMonster）
#include "TimerQueue.h"       // G_TimerQueue / TimerEntry::OBJ_TYPE
#include "RDARScriptStringManager.h"  // g_scriptStringManager_（sendMailRewardItem 标题解析）
#include "CMailBoxHelper.h"           // MakeSystemMultiMailPostal/ReqDBSendNewSystemMultiMail
#include "CAdvanceAltarLog.h"         // advanceAltarstatistics_（HistoryLog 统计）
#include "Inven_Item.h"               // Inven_Item（邮件附件数组）

#include <utility>                    // std::make_pair

namespace advancealtar
{

// ==== CharacData ============================================================

// ----------------------------------------------------------------------------
// ORIG T 0x0812d024（CharacData.md 全文）：4 标量清零 +
//   map<unsigned char,_SlotItem>@+0x10 默认构造（m_slotItems 成员初始化即一致）。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/CharacData.inc"
}

// ORIG T 0x0812d062（~CharacData.md 全文）：map@+0x10 析构（成员自动析构一致）。
#include "advancealtar_ProcStage/dtor_CharacData.inc"
}

// ORIG T 0x0812d246：m_user 非空时经其 CharacAdvanceAltarManager（CUser +0x8df60）
//   getSlotItem 同步 m_slotItems（map<uchar,_SlotItem>）。
#include "advancealtar_ProcStage/setSlotItem.inc"
}

// ORIG T 0x0812d078（reset.md 全文）：m_gageLevel/m_user/m_gage/m_field0 清零 +
//   m_slotItems clear。
#include "advancealtar_ProcStage/reset.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d0b6（upGage.md 全文）：cfg = G_CDataManager()+0x6364；
//   maxGage = **(int**)(cfg+0x18) * cfg->[0xc+(level+4)*4]（= *m_strPtr * m_gageUpAux[level]）；
//   m_gage >= maxGage → m_gage = maxGage；否则 ratio = m_gageUpRate[level] / base（float 局部）、
//   inc = (int)(m_gageUpRate[0] * ratio)（x87 截断，无 +0.5f）、m_gage += inc；
//   两臂各自调用 sendTestCheckGage()（ORIG 两处 call 0x812d103/0x812d16e）。
// ----------------------------------------------------------------------------
// b28d-solo-ProcStage：原实现有两处语义偏差——①`+ 0.5f` 多余（ORIG 0x812d141 段
//   `fnstcw/fldcw $0xc/fistpl` 为纯 (int) 截断，无 fadd 0.5）；②sendTestCheckGage()
//   写在 if 外（4.4 共享尾一次 call）而 ORIG 两臂各一次。形态另按 ORIG 对齐：
//   then 块在后（`jl <then>` 由 m_gage>=maxGage 反向条件复现）、ratio/inc 具名局部
//   （ORIG -0x10/-0xc 槽）、maxGage 内联表达式（ORIG 无 base 具名槽）。
#include "advancealtar_ProcStage/upGage.inc"
}

// ORIG T 0x0812d372（sendTestCheckGage.md 全文）：空函数。
#include "advancealtar_ProcStage/sendTestCheckGage.inc"
}

// ORIG W 0x08134806：*(this+4) = user。
#include "advancealtar_ProcStage/setUser.inc"
}

// ORIG W 0x08134814：return *(this+4)。
#include "advancealtar_ProcStage/getUser.inc"
}

// ----------------------------------------------------------------------------
// ORIG W 0x08134674：_SummonObject ctor(this+0x14) 后 memset(this,0,0x38)。
// m_summon 为字节槽（CDataManager.h 反向不可达），ctor 显式调用复现调用序列。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/_SummonObjectMs.inc"
}

#include "advancealtar_ProcStage/ProcSummonObjectMs.inc"
}

// ORIG T 0x0812d3f0（~.md 全文）：map/vector 析构（成员自动析构一致；
// 堆元素由 reset() delete，dtor 不释放与 ORIG 一致）。
#include "advancealtar_ProcStage/dtor_ProcSummonObjectMs.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d43c（reset.md 全文）：m_currentMs/m_deadCount/m_aliveDelta 清零；
//   遍历 vector operator delete 各元素；map clear；vector clear。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/reset_1.inc"
}

// ORIG T 0x0812d50a（isEndSummon.md 全文）：return vector.size() <= m_currentMs。
#include "advancealtar_ProcStage/isEndSummon.inc"
}

// ORIG W 0x08134820（objdump）：return m_field2c - m_field28。
#include "advancealtar_ProcStage/getAliveMonsterCount.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d540（insertSummonObject.md 全文）：push_back(&so) +
//   map.insert(make_pair(so.m_seq, &so))。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/insertSummonObject.inc"
}

// ORIG T 0x0812d5ba（getLastInsertDataIndex.md 全文）：return vector.size()。
#include "advancealtar_ProcStage/getLastInsertDataIndex.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d5d0（completePushSummonObject.md 全文）：
//   firstIndex>0 且 size()>firstIndex 时对 [firstIndex, end) 按 compare 升序 sort。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/completePushSummonObject.inc"
}

// ORIG T 0x0812daec（compare.md 全文）：return lhs->m_ms < rhs->m_ms。
#include "advancealtar_ProcStage/compare.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d658（summonObjectAndSendPacket.md 全文）：自 m_currentMs 起遍历
//   m_summons：元素空跳过；*m_pCurrentMs < so->m_ms → 结束返回；
//   到时且未发送（m_sentFlag==0）→ m_aliveDelta++、置位、组
//   _NotiAdvanceAltarSpawnUnit{spawnIndex=seq, memcpy _SummonObject} 包
//   （header 0x20b, payload 0x28）发当前用户。每轮 m_currentMs++。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/summonObjectAndSendPacket.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d804（debugLog.md 全文）：vector/map 两轮 cMyTrace 遍历输出
//   （格式串含 ORIG 原始 "Vecotr" 拼写，忠实保留；行号 0x140/0x14d）。
//   ObjectId/ObjectType/ObjectIndex 取 m_summon 槽内 _SummonObject
//   +0x00 m_id / +0x08 m_type / +0x0c m_field0c。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/debugLog.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812db00（dieMob.md 全文）：m_summonMap.find((short)mobSeq)；
//   未命中 → 0；命中且 so->m_sentFlag==1 且 m_deadFlag!=1 →
//   m_deadFlag=1、m_deadCount++、getUser 非空调 HistoryLog::killMonster(
//   user, _SummonObject::m_id（m_summon 槽 +0x00）, msg) 并返回 1；否则 0。
//   （第二 ushort 参数 ORIG 未引用，忠实保留。）
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/dieMob.inc"
}

// ==== ProcStage =============================================================

// ----------------------------------------------------------------------------
// ORIG T 0x0812dc18（ProcStage.md 全文）：存参 characData；state=7；
//   field8/fieldc/field10=0；field14=0（short）；ProcSummonObjectMs ctor
//   (this+0x18, *characData, &field8)；map(+0x50) 构造；difficulty=-1；
//   mapBasisLevel/dungeon/map=0；survivalFirst=1；survivalRound=0；
//   vector(+0x80) 构造；startStageTime=0；timeLine=0。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/ProcStage.inc"
}

// ORIG T 0x0812dd26（~ProcStage.md 全文）：vector/map/子对象析构——成员自动
// 析构与 ORIG 一致（堆元素由 reset() delete）。
#include "advancealtar_ProcStage/dtor_ProcStage.inc"
}

// ORIG T 0x0812e146（incCurrentMs.md 全文）：m_field8 += gmTimeSpeed_ * 1000。
#include "advancealtar_ProcStage/incCurrentMs.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812eb5a（isEndAllPhase.md 全文）：timeLine 非空且
//   (unsigned)m_fieldc >= phases.size()。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/isEndAllPhase.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812eba8（makeCurrentPhaseSummonObject.md 全文）：
//   timeLine 空 → 0；全部 phase 完成 → 0；取 phases[m_fieldc] 并后移下标；
//   alive=getAliveMonsterCount()；phaseEnd=m_field10+phase.m_field0；
//   按 assigns 的 [minAlive,maxAlive]（-1=不限）选 ActionDefine：
//   无命中 → 用不限档；仍无 → 0。define 空则跳过生成但保留尾部流程。
//   对 define->m_actions：spawnMs=m_field10+firstDelayMs，未达 phaseEnd 且
//   （flag==0 或 flag 计数 < flagValue）时 new _SummonObjectMs 填充并插入，
//   首个插入记 lastInsertIndex；否则 spawnMs+=nextDelayMs。
//   尾部 completePushSummonObject(lastInsertIndex)、m_field10+=时长、debugLog、返回 1。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/makeCurrentPhaseSummonObject.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e0d6（procTimeLine.md 全文）：state==3 时 isEndSummon →
//   makeCurrentPhaseSummonObject 失败置 state=4；incCurrentMs 后仍为 3 则
//   summonObjectAndSendPacket()。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/procTimeLine.inc"
}

// ORIG W 0x08134844：*(this+4) = state。
#include "advancealtar_ProcStage/setState.inc"
}

// ORIG T 0x0812f770：state==3 || state==4。
#include "advancealtar_ProcStage/isRewardState.inc"
}

// ORIG T 0x0812eb04：dungeon 非空 ? *(int*)(dungeon+0x804) : 0。
#include "advancealtar_ProcStage/getStageType.inc"
}

// ORIG W 0x0813486c：return *(int*)(this+0x68)。
#include "advancealtar_ProcStage/getStageDifficulty.inc"
}

// ORIG W 0x08134886：return *(int*)(this+0x8c)。
#include "advancealtar_ProcStage/getStartStageTime.inc"
}

// ORIG W 0x08134860：return *(int*)(this+0x74)。
#include "advancealtar_ProcStage/getMap.inc"
}

// ORIG W 0x08134852：return *(int*)(this+0x90)。
#include "advancealtar_ProcStage/getTimeLine.inc"
}

// ORIG W 0x08134878：return *(int*)(this+0x7c) + 1（objdump 实证 add $0x1）。
#include "advancealtar_ProcStage/getCurrentSurvivalRoundNo.inc"
}

// ORIG W 0x08134894：return *(int*)(this+0x6c)。
#include "advancealtar_ProcStage/getMapBasisLevel.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812dde0（reset.md 全文）：+0x08/+0x0c/+0x10 清零、difficulty=-1、
// survivalRound=0、ProcSummonObjectMs::reset(+0x18)；type!=1 时另做：
// state=7、+0x14=0、map(+0x50) clear、survivalFirst=1、field7c=0、vector clear。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/reset_2.inc"
}

// ORIG T 0x0812dd9a：dungeon=0、startStageTime=0、reset(0)、reset(1)。
#include "advancealtar_ProcStage/resetAll.inc"
}

// ORIG T 0x0812f70c：stageType==1 && survivalFirst!=0。
#include "advancealtar_ProcStage/isSurvivalModeFirstRound.inc"
}

// ORIG T 0x0812f74c：stageType==1 时 survivalFirst=0。
#include "advancealtar_ProcStage/setNotSurvivalModeFirstRound.inc"
}

// ORIG T 0x0812eb26：dungeon 非空 ? dungeon->get_index() : 0。
#include "advancealtar_ProcStage/getStageIndex.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812de70（startStage.md 全文）：
//   getUser 判空 → 3；find_dungeon(stageId) 判空 → 0x15；
//   stageType==1（生存轮次推进）:
//     reset(STAGE_SURVIVAL)；非首轮 → ++survivalRound；首轮 →
//       roundMaps = dungeon+0x814 vector 拷贝 + random_shuffle、state=1、
//       startStageTime=getCurSec、HistoryLog::enterDungeon；
//     survivalRound >= roundMaps.size() → state=7、返回 0x15；
//     否则 mapBasisLevel = survivalRound、mapIdx = roundMaps[survivalRound]；
//   其它（普通关卡选择）:
//     reset(STAGE_NORMAL)、difficulty=diff、state=1、
//     mapIdx = *(int*)(dungeon + (stageId+0x200)*4)、
//     mapBasisLevel = *(int*)(dungeon + (stageId+0x208)*4)、
//     startStageTime=getCurSec、HistoryLog::enterDungeon；
//   公共尾部: find_map(mapIdx) 判空 → 0x15；timeLine = map+0xf4；返回 0。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/startStage.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e16e（addCreatedMapMonster.md 全文）：seq = m_field14；
//   summary.m_seq(+0x04) = seq；m_field14 = seq+1；
//   m_createdMapMonsters.insert(make_pair(seq, summary))。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/addCreatedMapMonster.inc"
}

// ==== HistoryLog ============================================================

// ----------------------------------------------------------------------------
// ORIG T 0x08133b40（killMonster.md 全文）：
//   cUserHistoryLog(user+0x79700).KillMonster(monsterId,
//     msg.m_killValue1(+0xa1e), msg.m_killValue2(+0xa1a),
//     msg.m_killPairA24(+0xa24), msg.m_killPairA26(+0xa26),
//     msg, roleType=0, isBoss=false, curExp=0, exp=0, eExpAddReason=1)。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/killMonster.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08133bd4（dungeonClearInfo.md 全文）：
//   playSec = startStageTime>=1 ? getCurSec()-startStageTime : 0；
//   names[0xff] 清零后 getMemberNames(user, names)；
//   cUserHistoryLog(user+0x79700).DungeonClearInfo(isLast, playSec, stageIndex,
//     0, 1, ENUM_DUNGEON_MODE 0, names, characData->m_field0)；
//   isLast 时 incrementEntranceData(advanceAltarstatistics_, idx, 0,1,0)
//     [CAdvanceAltarLog 未建模，跳过]。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/dungeonClearInfo.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081339b0（enterDungeon.md 全文）：
//   names[0xff] 清零后 getMemberNames；EnterDungeon(stageIndex, difficulty, 1,
//   names, 2, 0)；incrementEntranceData(..., idx, 1,0,0) [未建模跳过]。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/enterDungeon.inc"
}


// ==== Wave5 AltarProc 追加（2026-08-27）=====================================

// ORIG T 0x0812cfa6（_ZN12advancealtar9isOnEventEv，worklist missing）：
// 事件开关全局函数，定义归 manager/批次；此处局部声明供 rewardClearStage 调用。
#include "advancealtar_ProcStage/isOnEvent.inc"
#include "advancealtar_ProcStage/getGage.inc"
}

// ORIG W 0x08142094（objdump）：return *(int*)(this+8)。
#include "advancealtar_ProcStage/getGageLevel.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d176（upgreadGage.md 全文）：cfg = G_CDataManager()+0x6364；
//   level==3 → ret2；cost = (int)((float)m_gageUpAux[level] * m_4)（x87 截断，
//   无 +0.5f）再 `cost = *m_strPtr * cost`；m_gage<cost → ret3；否则扣费并按档位
//   0→1 / 1→2 / 2→3 推进 m_gageLevel，ret0。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/upgreadGage.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d278（summonSlotUnit.md 全文）：cfg 同上；it=m_slotItems.find(slot)；
// 未命中 → 4；cost=**(int**)(cfg+0x18)*it->second.m_field8（iterator-> +0xc=
// pair second@+4 内 +0x08 字段）；条件 it->second.m_needGage==need（+0x08）
// 且 cost<=m_gage → 扣费 ret0；否则 ret2。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/summonSlotUnit.inc"
}

// ---- ProcStage -------------------------------------------------------------

// ORIG W 0x08134838（objdump）：return *(int*)(this+4)。
#include "advancealtar_ProcStage/getState.inc"
}

// ORIG T 0x0812e1de（dieMob.md 全文）：先转发 ProcSummonObjectMs(+0x18)；
 // 命中返回 true；否则 m_createdMapMonsters 查 (short)mobSeq：end 或死亡标记
 // (m_posX 兼用) 为 1 → false；否则置 1 返回 true。
#include "advancealtar_ProcStage/dieMob_1.inc"
}

// ORIG T 0x0812e2c6（isEndStage.md 全文）：
#include "advancealtar_ProcStage/isEndStage.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e3f6（clearStageAndReward.md 全文）：dungeon/user 判空直接返回；
 // clearStage(idx,diff,type,round,&star,&unlock,&bFlag)；成功 → 奖励表：
 // addAchievementReward(map) → rewardClearStage(vec) → setNextStage；
 // type==0 或 (type==1 && vec 非空) 且 decTicket()==0（扣票成功）时直接结束，
 // 否则走解锁商店/清关包分支。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/clearStageAndReward.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812ea00（endProcAllKillMonster.md 全文）：user 空 → 1；
 // stageType==1：startStage(getStageIndex(), difficulty)==0 → state=3、ret0，
 // 非 0 → clearStageAndReward、ret1；其它 type：clearStageAndReward、ret1。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/endProcAllKillMonster.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812eaa6（pause.md 全文）：pause=true 且 state==3 → state=2、ret1；
 // pause=false 且 state∈{2,1} → state=3、ret1；其余 ret0。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/pause.inc"
}

// ORIG W 0x0812eb4e（getClearSurvivalRoundNo.md objdump）：return *(this+0x7c)。
#include "advancealtar_ProcStage/getClearSurvivalRoundNo.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812f0fa（rewardClearStage.md 全文）：奖励源——生存模式取
 // dungeon+0x850 SurvivalRewardData.getRoundRewardData(survivalRound)，普通
 // 取内嵌 std::vector<_RewardItem>[difficulty] @ dungeon+0x82c（stride 0xc=12B
 // 即 vector 本体尺寸）。遍历（空表跳过）：事件开启时全部计入 out 并按
 // m_type 分流邮件集；关闭时仅 m_type==0 计入 out+普通邮件集。尾部分别以
 // "chn_game_server_msg_88/89"（普通）、"…_86/87"（活动）寄系统邮件。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/rewardClearStage.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812f3ae（addAchievementReward.md 全文）：user 判空；manager
 // getClearStageSummary(int[3]) 后逐槽(0..2)：data=RewardParameter@
 // G_CDataManager()+0x634c .getAchievementRewardData((StageDifficulty)i, sum[i])；
 // 命中则 addRewardData(data->m_type, out)（data+0 即 _Reward 首键/m_type）。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/addAchievementReward.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812f47a（sendMailRewardItem.md 全文）：三参判空；标题经
 // RDARScriptStringManager 表 4 解析（sender 截 0x1e、title 截 0xff）；最多
 // 10 个 _RewardItem 转 pair(itemId,count) + Inven_Item[10] 数组；调用
 // MakeSystemMultiMailPostal(pairs,&items,count) 打包后
 // ReqDBSendNewSystemMultiMail(sender,...,title,strlen(title),0xf,group,false)。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/sendMailRewardItem.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e6de（sendClearStage.md 全文，const 方法）：ridableId=current
 // 数据对象 +0x18 u16 符号扩展；包体 _NotiAdvanceAltarClearStage(0x1c packed)
 // {stageType, clearRound, ridable, clearFlag, stage{idx,diff}, star}；
 // put_header(0,0x20f)+put_binary(0x1c)；rewards：put_int(size)+逐元素 0xc；
 // buyItems 同 0xc；rewardMap：put_int(size)+逐 value(_Reward 5B)；finalize→Send。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/sendClearStage.inc"
}

// ---- HistoryLog ------------------------------------------------------------

// ----------------------------------------------------------------------------
// ORIG T 0x08133abc（leaveDungeon.md 全文）：names[0xff] 清零 + getMemberNames；
 // LeaveDungeon(hist@user+0x79700, stageIndex, difficulty, names, 0)。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/leaveDungeon.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08133d56（starAdd.md 全文）：LogHistory("star+,%d,%d,%d")+
// incrementEntranceData(advanceAltarstatistics_, idx,0,0,cnt)。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/starAdd.inc"
}

// ORIG T 0x08133db0（starSub.md 全文）：仅 LogHistory("star-,%d,%d")。
#include "advancealtar_ProcStage/starSub.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08133e30（updateStatistics.md 全文）：CAdvanceAltarLog::updateDatabase
 // (advanceAltarstatistics_)。依赖随 CAdvanceAltarLog 批次解锁。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/updateStatistics.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08133dda（buyItem.md 全文）：
 // incrementLayerStatData(advanceAltarstatistics_, shopType, param2, param4, 1,
 // param5)——param3(第 1 个 short) 未参与调用，ORIG 原样。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/buyItem.inc"
}
}  // namespace advancealtar

// ----------------------------------------------------------------------------
// ORIG T 0x0812cf54（docs/class_func_reports/advancealtar/getMemberNames.md 全文）：
//   +0x07 `cmpl $0,0x8; je end` + +0x0d `cmpl $0,0xc; je end` ⇒ 两独立判空守卫
//   （P133：非合并 || ——合并生 setne/test 汇聚形，与此 test-mem/jcc 形不符）；
//   +0x19 `call getCurCharacName; mov %eax,%ebx`（第 1 次结果缓存 ebx）
//   +0x26 `call getCurCharacName`（第 2 次，eax）⇒ 两次独立 getCurCharacName
//   （P48 O0 无 CSE：Ghidra 双臂同 getter = 源码两次内联调用，禁命名局部缓存，
//   否则少一条 call + ebx 缓存）。sprintf 实参右→左：末参(第 2 个 %s)=先算的
//   ebx(call1)，首参(第 1 个 %s)=后算的 eax(call2)；故两实参均内联 getCurCharacName()。
// ----------------------------------------------------------------------------
namespace advancealtar
{
#include "advancealtar_ProcStage/getMemberNames.inc"
}
}  // namespace advancealtar
