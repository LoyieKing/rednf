#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_ProcStage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "advancealtar_ProcStage/CharacData.inc"


#include "advancealtar_ProcStage/dtor_CharacData.inc"


#include "advancealtar_ProcStage/setSlotItem.inc"


#include "advancealtar_ProcStage/reset.inc"


#include "advancealtar_ProcStage/upGage.inc"


#include "advancealtar_ProcStage/sendTestCheckGage.inc"


#include "advancealtar_ProcStage/setUser.inc"


#include "advancealtar_ProcStage/getUser.inc"


#include "advancealtar_ProcStage/_SummonObjectMs.inc"


#include "advancealtar_ProcStage/ProcSummonObjectMs.inc"


#include "advancealtar_ProcStage/dtor_ProcSummonObjectMs.inc"


#include "advancealtar_ProcStage/reset_1.inc"


#include "advancealtar_ProcStage/isEndSummon.inc"


#include "advancealtar_ProcStage/getAliveMonsterCount.inc"


#include "advancealtar_ProcStage/insertSummonObject.inc"


#include "advancealtar_ProcStage/getLastInsertDataIndex.inc"


#include "advancealtar_ProcStage/completePushSummonObject.inc"


#include "advancealtar_ProcStage/compare.inc"


#include "advancealtar_ProcStage/summonObjectAndSendPacket.inc"


#include "advancealtar_ProcStage/debugLog.inc"


#include "advancealtar_ProcStage/dieMob.inc"


// ==== ProcStage =============================================================

#include "advancealtar_ProcStage/ProcStage.inc"


#include "advancealtar_ProcStage/dtor_ProcStage.inc"


#include "advancealtar_ProcStage/incCurrentMs.inc"


#include "advancealtar_ProcStage/isEndAllPhase.inc"


#include "advancealtar_ProcStage/makeCurrentPhaseSummonObject.inc"


#include "advancealtar_ProcStage/procTimeLine.inc"


#include "advancealtar_ProcStage/setState.inc"
{
    // P95 先 this 后值：4.4 对普通成员赋值恒「先备值(mov 0xc,%edx)后装 this」，
    // ORIG 为先 this(0x8→eax)后值(0xc→edx)——经 pack(1) 具名视图访问翻转取数序
    //（/tmp/probeF.cpp sd 形实证）。偏移/宽度不变。
    struct V_P95
    {
        char m_pad4[4];   // +0x00（m_characData 槽）
        int m_state;      // +0x04
    };
    ((V_P95*)this)->m_state = static_cast<int>(state);
}

#include "advancealtar_ProcStage/isRewardState.inc"


#include "advancealtar_ProcStage/getStageType.inc"


#include "advancealtar_ProcStage/getStageDifficulty.inc"


#include "advancealtar_ProcStage/getStartStageTime.inc"


#include "advancealtar_ProcStage/getMap.inc"


#include "advancealtar_ProcStage/getTimeLine.inc"


#include "advancealtar_ProcStage/getCurrentSurvivalRoundNo.inc"


#include "advancealtar_ProcStage/getMapBasisLevel.inc"


#include "advancealtar_ProcStage/reset_2.inc"


#include "advancealtar_ProcStage/resetAll.inc"


#include "advancealtar_ProcStage/isSurvivalModeFirstRound.inc"


#include "advancealtar_ProcStage/setNotSurvivalModeFirstRound.inc"


#include "advancealtar_ProcStage/getStageIndex.inc"


#include "advancealtar_ProcStage/startStage.inc"


#include "advancealtar_ProcStage/addCreatedMapMonster.inc"


// ==== HistoryLog ============================================================

#include "advancealtar_ProcStage/killMonster.inc"


#include "advancealtar_ProcStage/dungeonClearInfo.inc"


#include "advancealtar_ProcStage/enterDungeon.inc"



// ==== Wave5 AltarProc 追加（2026-08-27）=====================================

#include "advancealtar_ProcStage/isOnEvent.inc"

// ---- CharacData ------------------------------------------------------------

#include "advancealtar_ProcStage/getGage.inc"


#include "advancealtar_ProcStage/getGageLevel.inc"


#include "advancealtar_ProcStage/upgreadGage.inc"


#include "advancealtar_ProcStage/summonSlotUnit.inc"


// ---- ProcStage -------------------------------------------------------------

#include "advancealtar_ProcStage/getState.inc"


#include "advancealtar_ProcStage/dieMob_1.inc"


#include "advancealtar_ProcStage/isEndStage.inc"


#include "advancealtar_ProcStage/clearStageAndReward.inc"


#include "advancealtar_ProcStage/endProcAllKillMonster.inc"


#include "advancealtar_ProcStage/pause.inc"


#include "advancealtar_ProcStage/getClearSurvivalRoundNo.inc"


#include "advancealtar_ProcStage/rewardClearStage.inc"


#include "advancealtar_ProcStage/addAchievementReward.inc"


#include "advancealtar_ProcStage/sendMailRewardItem.inc"


#include "advancealtar_ProcStage/sendClearStage.inc"


// ---- HistoryLog ------------------------------------------------------------

#include "advancealtar_ProcStage/leaveDungeon.inc"


#include "advancealtar_ProcStage/starAdd.inc"


#include "advancealtar_ProcStage/starSub.inc"


#include "advancealtar_ProcStage/updateStatistics.inc"


#include "advancealtar_ProcStage/buyItem.inc"

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

}  // namespace advancealtar
