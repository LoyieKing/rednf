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

// ----------------------------------------------------------------------------
// ORIG T 0x0812d024（CharacData.md 全文）：4 标量清零 +
//   map<unsigned char,_SlotItem>@+0x10 默认构造（m_slotItems 成员初始化即一致）。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/CharacData.inc"
    : m_field0(0), m_user(0), m_gageLevel(0), m_gage(0), m_slotItems()
{
}

// ORIG T 0x0812d062（~CharacData.md 全文）：map@+0x10 析构（成员自动析构一致）。
#include "advancealtar_ProcStage/dtor_CharacData.inc"
{
}

// ORIG T 0x0812d246：m_user 非空时经其 CharacAdvanceAltarManager（CUser +0x8df60）
//   getSlotItem 同步 m_slotItems（map<uchar,_SlotItem>）。
#include "advancealtar_ProcStage/setSlotItem.inc"
{
    if (m_user == 0)
        goto L_done;
    m_user->m_advanceAltar.getSlotItem(m_slotItems);
L_done:;
}

// ORIG T 0x0812d078（reset.md 全文）：m_gageLevel/m_user/m_gage/m_field0 清零 +
//   m_slotItems clear。
#include "advancealtar_ProcStage/reset.inc"
{
    m_gageLevel = 0;
    m_user = 0;
    m_gage = 0;
    m_field0 = 0;
    m_slotItems.clear();
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
{
    ConfigParameter* cfg = &G_CDataManager()->m_altarConfig;
    // ORIG 求值序：先 aux[level]（→edx）后 *m_strPtr（→eax）再 imul ⇒ 交换书写序；
    //   索引位 `static_cast<unsigned int>(m_gageLevel)` 复现 ORIG 两段装载
    //   `mov 0x8(%eax),%eax; mov %eax,%edx`（P319；裸 int 索引 4.4 恒折单段）。
    int maxGage = cfg->m_gageUpAux[static_cast<unsigned int>(m_gageLevel)] *
                  *reinterpret_cast<int*>(cfg->m_strPtr);
    if (m_gage >= maxGage)
    {
        m_gage = maxGage;
        sendTestCheckGage();
    }
    else
    {
        float ratio = (float)cfg->m_gageUpRate[static_cast<unsigned int>(m_gageLevel)] /
                      (float)*reinterpret_cast<int*>(cfg->m_strPtr);
        int inc = (int)((float)cfg->m_gageUpRate[0] * ratio);
        m_gage = m_gage + inc;
        sendTestCheckGage();
    }
}

// ORIG T 0x0812d372（sendTestCheckGage.md 全文）：空函数。
#include "advancealtar_ProcStage/sendTestCheckGage.inc"
{
}

// ORIG W 0x08134806：*(this+4) = user。
#include "advancealtar_ProcStage/setUser.inc"
{
    m_user = user;
}

// ORIG W 0x08134814：return *(this+4)。
#include "advancealtar_ProcStage/getUser.inc"
{
    return m_user;
}

// ----------------------------------------------------------------------------
// ORIG W 0x08134674：_SummonObject ctor(this+0x14) 后 memset(this,0,0x38)。
// m_summon 为字节槽（CDataManager.h 反向不可达），ctor 显式调用复现调用序列。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/_SummonObjectMs.inc"
{
    // b28d-52：m_summon 改真实成员后由编译器自动发 _SummonObject::C1 call
    // （ORIG 0x08134677 `add $0x14,%eax; call _SummonObject::C1Ev`），
    // 无需 placement new（消 N83 的 extern _ZnwjPv + EH cleanup 段）。
    std::memset(this, 0, sizeof(*this));
}

#include "advancealtar_ProcStage/ProcSummonObjectMs.inc"
                                                  int const& currentMs)
    : m_currentMs(0), m_summons(), m_summonMap(),
      m_deadCount(0), m_aliveDelta(0),
      m_characData(&characData), m_pCurrentMs(const_cast<int*>(&currentMs))
{
}

// ORIG T 0x0812d3f0（~.md 全文）：map/vector 析构（成员自动析构一致；
// 堆元素由 reset() delete，dtor 不释放与 ORIG 一致）。
#include "advancealtar_ProcStage/dtor_ProcSummonObjectMs.inc"
{
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d43c（reset.md 全文）：m_currentMs/m_deadCount/m_aliveDelta 清零；
//   遍历 vector operator delete 各元素；map clear；vector clear。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/reset_1.inc"
{
    m_currentMs = 0;
    m_deadCount = 0;
    m_aliveDelta = 0;
    // b28d-52：ORIG 0x0812d478 begin 结果经 const_iterator 转换构造（P96/P270：
    // `__normal_iteratorIPKP...` 转换 ctor），增量段为后置 ppEi（P28）。
    for (std::vector<_SummonObjectMs*>::const_iterator it = m_summons.begin();
         it != m_summons.end(); it++)
    {
        delete *it;
    }
    m_summonMap.clear();
    m_summons.clear();
}

// ORIG T 0x0812d50a（isEndSummon.md 全文）：return vector.size() <= m_currentMs。
#include "advancealtar_ProcStage/isEndSummon.inc"
{
    // b28c-L5：ORIG 0x0812d50a 两处约束——
    //   ① `mov (%eax),%ebx`（m_currentMs 一步 32 位直落 callee-saved、跨 size()
    //      调用保活）⇒ 源侧**不得**写 static_cast（显式 cast 使 4.4 退化成
    //      `mov → mov %eax,%ebx` 两段并丢 push %ebx；int 与 size_t 的隐式 usual
    //      arithmetic conversion 即 ORIG 的 `cmp %eax,%ebx / setae` 无符号形）；
    //   ② 尾形 `test %al,%al; je; mov $1; jmp; mov $0` 是 if-return 分支物化，
    //      非 `return expr;` 的 setae 直返（P56）。
    if (m_currentMs >= m_summons.size())
    {
        return true;
    }
    return false;
}

// ORIG W 0x08134820（objdump）：return m_field2c - m_field28。
#include "advancealtar_ProcStage/getAliveMonsterCount.inc"
{
    return m_aliveDelta - m_deadCount;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d540（insertSummonObject.md 全文）：push_back(&so) +
//   map.insert(make_pair(so.m_seq, &so))。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/insertSummonObject.inc"
{
    m_summons.push_back(&so);
    m_summonMap.insert(std::make_pair(so.m_seq, &so));
}

// ORIG T 0x0812d5ba（getLastInsertDataIndex.md 全文）：return vector.size()。
#include "advancealtar_ProcStage/getLastInsertDataIndex.inc"
{
    return static_cast<int>(m_summons.size());
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d5d0（completePushSummonObject.md 全文）：
//   firstIndex>0 且 size()>firstIndex 时对 [firstIndex, end) 按 compare 升序 sort。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/completePushSummonObject.inc"
{
    // b28d-solo-ProcStage：ORIG 0x0812d5e0 用 size()+operator[] 取地址形
    //   （`call size; lea -0x1(%eax),%edx; …call operator[]`，无 begin()/end()
    //   迭代器构造）；且第二实参是 `&m_summons[size()-1]`（P171 照抄 ORIG 的
    //   末元素不参与排序行为，勿按语义"修正"成 end()）。
    // ORIG 0x0812d5d7 物化形 = `(fi<=0) || (size()<=fi)` 短路（`cmpl $0,mem; jle <mov$1>`
    //   + `cmp fi,%eax; jg <mov$0>` + fall `mov $1`）⇒ 条件写德摩根取反形
    //   （4.4 对 `A&&B` 恒双失败跳，与 ORIG 极性不符）。
    if (({ firstIndex <= 0 ||
          static_cast<int>(m_summons.size()) <= firstIndex; }))
    {
        return;
    }
    std::sort(&m_summons[firstIndex], &m_summons[m_summons.size() - 1],
              ProcStage::ProcSummonObjectMs::compare);
}

// ORIG T 0x0812daec（compare.md 全文）：return lhs->m_ms < rhs->m_ms。
#include "advancealtar_ProcStage/compare.inc"
                                            const _SummonObjectMs* rhs)
{
    return lhs->m_ms < rhs->m_ms;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d658（summonObjectAndSendPacket.md 全文）：自 m_currentMs 起遍历
//   m_summons：元素空跳过；*m_pCurrentMs < so->m_ms → 结束返回；
//   到时且未发送（m_sentFlag==0）→ m_aliveDelta++、置位、组
//   _NotiAdvanceAltarSpawnUnit{spawnIndex=seq, memcpy _SummonObject} 包
//   （header 0x20b, payload 0x28）发当前用户。每轮 m_currentMs++。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/summonObjectAndSendPacket.inc"
{
    // b28d-solo-ProcStage：ORIG 0x0812d660 先把 size() 缓存到 -0x10（P13），
    //   `so` 具名局部在 -0xc；判空用一次独立 operator[]（P48 无 CSE），
    //   m_sentFlag 条件再发第三次 operator[]。
    unsigned int count = static_cast<unsigned int>(m_summons.size());
    _SummonObjectMs* so;
    while (static_cast<unsigned int>(m_currentMs) < count)
    {
        // b28d-solo-ProcStage：P223——ORIG 0x0812d691 `sete %al; test %al,%al;
        //   jne 0812d7d9`（nop 占位岛）+ body 尾 `jmp 0812d7da` ⇒ 原码是
        //   `if (p == 0) goto L_next; body; L_next: ++m_currentMs;`（空 then 岛）。
        //   旧正向 `if (p != 0) {body}` 4.4 恒产 setne/je（差集观察）。
        if (m_summons[m_currentMs] == 0)
        {
            goto L_next;
        }
        so = m_summons[m_currentMs];
        if (so->m_ms > *m_pCurrentMs)
        {
            return;
        }
        // b28d-solo-ProcStage：P48/P157——ORIG 0x812d6cd/0x812d6e2 两处均读
        //   `-0xc(%ebp)`（so 缓存）而非重发 operator[]（差集旧形多 1 次
        //   `vector::operator[](unsigned)`）。
        if (so->m_ms <= *m_pCurrentMs && so->m_sentFlag == 0)
        {
            ++m_aliveDelta;
            so->m_sentFlag = 1;

            NOTIPacketStruct::_NotiAdvanceAltarSpawnUnit body;
            body.m_spawnIndex = static_cast<unsigned short>(so->m_seq);
            std::memcpy(&body.m_summon, &so->m_summon, 0x24);

            PacketGuard guard;
            guard.put_header(0, 0x20b);
            guard.put_binary((char*)&body, 0x28);
            guard.finalize(true);
            // [d03-T1b② 槽复用试形] ORIG 无 user 具名槽（我方-only -0x14 疑为该 4 字节
            //   局部致全帧平移 4）⇒ 内联表达式直调；一次旋钮，无改善即回滚。
            m_characData->getUser()->Send(guard);
        }
    L_next:
        ++m_currentMs;
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d804（debugLog.md 全文）：vector/map 两轮 cMyTrace 遍历输出
//   （格式串含 ORIG 原始 "Vecotr" 拼写，忠实保留；行号 0x140/0x14d）。
//   ObjectId/ObjectType/ObjectIndex 取 m_summon 槽内 _SummonObject
//   +0x00 m_id / +0x08 m_type / +0x0c m_field0c。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/debugLog.inc"
{
    // b28d-solo-ProcStage：ORIG 0x0812d834 起每字段重发 operator*（P48/P157 无 CSE，
    //   共 9 次 `call …deEv`）+ 判空 `sete/jne` + cMyTrace 每轮构造即调用
    //   （P221/P37 全表达式临时链，ctor 在循环体内 -0x4c）。
    {
        // P171 死计数器照抄：ORIG 0x0812d828 的 `movl $0,-0x20` 在 begin() 之后、
        //   `addl $1,-0x20` 在增量段 ppEi 之后（0x0812d94d）⇒ 源码是
        //   `it=begin(); n=0; for(; it!=end(); it++, ++n)`（continue 也走增量段）。
        std::vector<_SummonObjectMs*>::const_iterator it = m_summons.begin();
        int n = 0;
        for (; it != m_summons.end(); it++, ++n)   // P28：ORIG 增量段 ppEi（后置）
        {
            if ((*it) == 0)
            {
                continue;
            }
            cMyTrace(__PRETTY_FUNCTION__, 0x140, 0)(
                "\tVecotr Ms = %06d ActionDefineId=%d ObjectId=%d spawndId=%d isDead=%d ObjectType=%d ObjectIndex=%d\n",
                (*it)->m_ms, (*it)->m_actionDefineId,
                (*it)->m_summon.m_id,
                static_cast<int>((*it)->m_seq),
                static_cast<int>((*it)->m_deadFlag),
                (*it)->m_summon.m_type,
                (*it)->m_summon.m_field0c);
        }
    }
    {
        // P171 死计数器照抄：ORIG 0x0812d99b `movl $0,-0x1c` 在 begin() 之后。
        std::map<short, _SummonObjectMs*>::const_iterator it = m_summonMap.begin();
        int n = 0;
        for (; it != m_summonMap.end(); it++, ++n)   // P28：ORIG 增量段 ppEi（后置）
        {
            // b28d-solo-ProcStage：ORIG 0x0812d9a7 起为 `call operator->; mov 0x4(%eax),%eax`
            //   （P167 新判据：ptEv 而非 deEv ⇒ 源写 `it->second`，每字段重发；map 侧无判空）。
            cMyTrace(__PRETTY_FUNCTION__, 0x14d, 0)(
                "\tMap Ms = %06d ActionDefineId=%d ObjectId=%d spawndId=%d isDead=%d ObjectType=%d ObjectIndex=%d\n",
                it->second->m_ms, it->second->m_actionDefineId,
                it->second->m_summon.m_id,
                static_cast<int>(it->second->m_seq),
                static_cast<int>(it->second->m_deadFlag),
                it->second->m_summon.m_type,
                it->second->m_summon.m_field0c);
        }
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812db00（dieMob.md 全文）：m_summonMap.find((short)mobSeq)；
//   未命中 → 0；命中且 so->m_sentFlag==1 且 m_deadFlag!=1 →
//   m_deadFlag=1、m_deadCount++、getUser 非空调 HistoryLog::killMonster(
//   user, _SummonObject::m_id（m_summon 槽 +0x00）, msg) 并返回 1；否则 0。
//   （第二 ushort 参数 ORIG 未引用，忠实保留。）
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/dieMob.inc"
                                          const MSG_MONSTER_DIE& msg)
{
    (void)param;  // ORIG 未使用
    std::map<short, _SummonObjectMs*>::iterator it = m_summonMap.find((short)mobSeq);
    // b28d-solo-ProcStage：ORIG 0x0812db53 起正向包裹（`test %al; je <尾 mov$0>` 单出口），
    //   且 `it->second` 每次重发 operator->（P48/P167：0x812db73/0x812db84/0x812dbb1/
    //   0x812dbe9 四处 `call operator->`），`so` 仅作首次结果缓存；
    //   m_deadFlag 判读为 `movzbl; xor $1; test; je`（P248 档② bool 字段 + 裸 `!`）。
    if (it != m_summonMap.end())
    {
        _SummonObjectMs* so = it->second;
        // P171 死写入照抄：ORIG 0x812db7e~0x812db8f 第二次 operator-> 取
        //   it->second->m_sentFlag 存 -0x10 后从不读取。
        const int sentFlag = it->second->m_sentFlag;
        (void)sentFlag;
        if (so->m_sentFlag == 1 && !so->m_deadFlag)
        {
            it->second->m_deadFlag = true;
            ++m_deadCount;
            CUser* user = m_characData->getUser();
            if (user != 0)
            {
                HistoryLog::killMonster(*user, it->second->m_summon.m_id, msg);
            }
            return 1;
        }
    }
    return 0;
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
    : m_characData(&characData),
      m_state(State::S_N7),
      m_field8(0),
      m_fieldc(0),
      m_field10(0),
      m_field14(0),
      m_summonObjectMs(*m_characData, m_field8),
      m_createdMapMonsters(),
      m_difficulty(static_cast<StageDifficulty::T>(-1)),
      m_mapBasisLevel(0),
      m_dungeon(0),
      m_map(0),
      m_survivalFirst(1),
      m_survivalRound(0),
      m_roundMaps(),
      m_startStageTime(0),
      m_timeLine(0)
{
}

// ORIG T 0x0812dd26（~ProcStage.md 全文）：vector/map/子对象析构——成员自动
// 析构与 ORIG 一致（堆元素由 reset() delete）。
#include "advancealtar_ProcStage/dtor_ProcStage.inc"
{
}

// ORIG T 0x0812e146（incCurrentMs.md 全文）：m_field8 += gmTimeSpeed_ * 1000。
#include "advancealtar_ProcStage/incCurrentMs.inc"
{
    int speed = Manager::gmTimeSpeed_ * 1000;   // ORIG -0x4 槽往返（P0 具名局部）
    m_field8 = m_field8 + speed;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812eb5a（isEndAllPhase.md 全文）：timeLine 非空且
//   (unsigned)m_fieldc >= phases.size()。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/isEndAllPhase.inc"
{
    // b28c-L5：同 isEndSummon —— ORIG 0x0812eb5a `mov 0xc(%eax),%ebx` 一步直落
    //   （无 static_cast，隐式 usual conversion 保 `cmp %eax,%ebx / setae`）+
    //   尾 if-return 物化岛（P56）；现形 cast 致两段中转、直返省整段岛（少 4 条）。
    if (m_timeLine == 0)
    {
        return false;
    }
    if (m_fieldc >= m_timeLine->m_phases.size())
    {
        return true;
    }
    return false;
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
{
    if (m_timeLine == 0)
    {
        return false;
    }
    if (isEndAllPhase())
    {
        return false;
    }

    // b28d-solo-ProcStage：P167/P96——ORIG 0x812ebd9 `_ZNKSt6vectorI…Phase…EixEj`
    //   （const 版 operator[]）+ 全套 `__normal_iteratorIPKN…` ⇒ m_timeLine 为
    //   const TimeLine*（头已 const 化），phase 取 const 引用。
    const Phase& phase = m_timeLine->m_phases[m_fieldc];
    ++m_fieldc;
    const int alive = m_summonObjectMs.getAliveMonsterCount();
    // ORIG 0x0812ec27/0x0812ec2f 两局部：-0x30=phaseEnd（先取 m_field0 再 add
    //   m_field10）、-0x2c=phaseLength（phaseEnd 的副本，供尾部 m_field10+=）。
    int phaseEnd = phase.m_field0;
    const int phaseLength = phaseEnd;
    phaseEnd += m_field10;

    // b28d-solo-ProcStage：P167/P220——ORIG 0x812ec5c/0x812ec71 循环前两次 end()
    //   初始化两个 const_iterator 哨兵（-0x7c=rangedIt、-0x80=unlimitedIt），
    //   命中分支直接 `mov -0x78,%eax; mov %eax,-0x80/-0x7c`（迭代器整体赋值）；
    //   两处 `rangedIt == end()` 用 eq 正极性（差集 eq ×2）。
    std::vector<_ActionAssign>::const_iterator rangedIt = phase.m_assigns.end();
    std::vector<_ActionAssign>::const_iterator unlimitedIt = phase.m_assigns.end();
    for (std::vector<_ActionAssign>::const_iterator it = phase.m_assigns.begin();
         it != phase.m_assigns.end(); it++)
    {
        if (it->m_minAliveCount == -1 && it->m_maxAliveCount == -1)
        {
            unlimitedIt = it;            // 不限档
        }
        else if (alive >= it->m_minAliveCount && it->m_maxAliveCount >= alive)
        {
            rangedIt = it;               // 存活数区间档（循环不覆盖已命中者）
        }
    }
    if (rangedIt == phase.m_assigns.end())
    {
        rangedIt = unlimitedIt;
    }
    if (rangedIt == phase.m_assigns.end())
    {
        return false;
    }

    std::map<int, int> spawnCounts;
    int firstIndex = -1;
    // b28d-solo-ProcStage：P167——ORIG 的 m_actions 迭代器为 const 族
    //   （ZNK begin/end + ne(const,const)），无转换构造 ⇒ define 须为 const 指针。
    const ActionDefine* define = rangedIt->m_define;
    if (define != 0)
    {
        for (std::vector<_Action>::const_iterator it = define->m_actions.begin();
             it != define->m_actions.end(); it++)   // P28：ORIG 增量段 ppEi
        {
            // ORIG 0x812f054 段：`mov m_field10→-0x18; …; add %eax,-0x18`（两步具名局部）
            // ⇒ 拆两句（P0/P186：合并成一句被 4.4 折 lea）。
            int spawnMs = m_field10;
            spawnMs += it->m_firstDelayMs;
            while (spawnMs < phaseEnd)
            {
                // widthscan：ORIG 全函数无 movb/cmpb（clr_b/cmp_b orig=0）⇒ create 是 int
                //（4.4 写 bool 会产 movb $1 + cmpb；ORIG 为 cmpl）。
                int create = 1;
                if (it->m_flag != 0)
                {
                    const int key = it->m_pSummon->m_field0c;
                    std::map<int, int>::iterator found = spawnCounts.find(key);
                    if (found == spawnCounts.end())
                    {
                        spawnCounts.insert(std::make_pair(key, 1));
                    }
                    else if (found->second < it->m_flagValue)
                    {
                        ++found->second;
                    }
                    else
                    {
                        create = 0;       // 该对象本轮配额用尽
                    }
                }
                if (create)
                {
                    _SummonObjectMs* so = new _SummonObjectMs;
                    so->m_ms = spawnMs;
                    so->m_seq = m_field14;
                    m_field14 = static_cast<short>(m_field14 + 1);
                    so->m_actionDefineId = define->m_id;
                    // ORIG 0x812f020 段 `movb $0x0,0xc(%eax)`：new 后显式清零 m_deadFlag
                    //（P171 照抄原码显式写；ctor memset 已清但 ORIG 逐条发射）。
                    so->m_deadFlag = 0;
                    std::memcpy(&so->m_summon, it->m_pSummon, 0x24);
                    m_summonObjectMs.insertSummonObject(*so);
                    spawnMs += it->m_nextDelayMs;
                    if (firstIndex == -1)
                    {
                        firstIndex = m_summonObjectMs.getLastInsertDataIndex();
                    }
                }
                else
                {
                    spawnMs += it->m_nextDelayMs;
                }
            }
        }
    }
    m_summonObjectMs.completePushSummonObject(firstIndex);
    m_field10 += phaseLength;
    m_summonObjectMs.debugLog();
    return true;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e0d6（procTimeLine.md 全文）：state==3 时 isEndSummon →
//   makeCurrentPhaseSummonObject 失败置 state=4；incCurrentMs 后仍为 3 则
//   summonObjectAndSendPacket()。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/procTimeLine.inc"
{
    if (m_state != State::S_N3)
        goto L_out;
    if (m_summonObjectMs.isEndSummon())
    {
        bool made = makeCurrentPhaseSummonObject();
        if (!made)
        {
            m_state = State::S_N4;
        }
    }
    incCurrentMs();
    if (m_state == State::S_N3)
        m_summonObjectMs.summonObjectAndSendPacket();
L_out:;
}

// ORIG W 0x08134844：*(this+4) = state。
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

// ORIG T 0x0812f770：state==3 || state==4。
#include "advancealtar_ProcStage/isRewardState.inc"
{
    return m_state == 3 || m_state == 4;
}

// ORIG T 0x0812eb04：dungeon 非空 ? *(int*)(dungeon+0x804) : 0。
#include "advancealtar_ProcStage/getStageType.inc"
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    // CDungeon+0x804 = m_advanceAltarType（int；getStageType.md 汇编 `mov 0x804(%eax),%eax`）。
    return m_dungeon->m_advanceAltarType;
}

// ORIG W 0x0813486c：return *(int*)(this+0x68)。
#include "advancealtar_ProcStage/getStageDifficulty.inc"
{
    return m_difficulty;
}

// ORIG W 0x08134886：return *(int*)(this+0x8c)。
#include "advancealtar_ProcStage/getStartStageTime.inc"
{
    return m_startStageTime;
}

// ORIG W 0x08134860：return *(int*)(this+0x74)。
#include "advancealtar_ProcStage/getMap.inc"
{
    return m_map;
}

// ORIG W 0x08134852：return *(int*)(this+0x90)。
#include "advancealtar_ProcStage/getTimeLine.inc"
{
    return m_timeLine;
}

// ORIG W 0x08134878：return *(int*)(this+0x7c) + 1（objdump 实证 add $0x1）。
#include "advancealtar_ProcStage/getCurrentSurvivalRoundNo.inc"
{
    return m_survivalRound + 1;
}

// ORIG W 0x08134894：return *(int*)(this+0x6c)。
#include "advancealtar_ProcStage/getMapBasisLevel.inc"
{
    return m_mapBasisLevel;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812dde0（reset.md 全文）：+0x08/+0x0c/+0x10 清零、difficulty=-1、
// survivalRound=0、ProcSummonObjectMs::reset(+0x18)；type!=1 时另做：
// state=7、+0x14=0、map(+0x50) clear、survivalFirst=1、field7c=0、vector clear。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/reset_2.inc"
{
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_difficulty = static_cast<StageDifficulty::T>(-1);
    m_map = 0;                    // ORIG +0x74 总是清零
    m_summonObjectMs.reset();

    if ((unsigned int)type != 1u)   // ORIG reg 形比较（P209：unsigned 载寄存器）
    {
        m_state = 7;
        m_field14 = 0;
        m_createdMapMonsters.clear();
        m_survivalFirst = 1;
        m_survivalRound = 0;      // ORIG +0x7c 仅非生存分支清零
        m_roundMaps.clear();
    }
}

// ORIG T 0x0812dd9a：dungeon=0、startStageTime=0、reset(0)、reset(1)。
#include "advancealtar_ProcStage/resetAll.inc"
{
    m_dungeon = 0;
    m_startStageTime = 0;
    reset(StageType::STAGE_NORMAL);
    reset(StageType::STAGE_SURVIVAL);
}

// ORIG T 0x0812f70c：stageType==1 && survivalFirst!=0。
#include "advancealtar_ProcStage/isSurvivalModeFirstRound.inc"
{
    // b28d-solo-ProcStage：P0/N96 清绑（原 `register bool r __asm__("al")` 违禁）。
    // ORIG 0x0812f70d 汇聚值仅存 %eax/%al（无栈槽往返）⇒ 用 P226 条件向语句表达式
    // （`if (({ A && B; }))`，内不含具名变量）复现短路物化岛 + 零槽。
    if (({ getStageType() == 1 && m_survivalFirst != 0; }))
        return true;
    return false;
}

// ORIG T 0x0812f74c：stageType==1 时 survivalFirst=0。
#include "advancealtar_ProcStage/setNotSurvivalModeFirstRound.inc"
{
    if (getStageType() == 1)
    {
        m_survivalFirst = 0;
    }
}

// ORIG T 0x0812eb26：dungeon 非空 ? dungeon->get_index() : 0。
#include "advancealtar_ProcStage/getStageIndex.inc"
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    return m_dungeon->get_index();
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
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return 3;
    }

    m_dungeon = G_CDataManager()->find_dungeon(stageId);
    if (m_dungeon == 0)
    {
        return 0x15;
    }

    int mapIdx = 0;
    if (getStageType() == 1)
    {
        reset(StageType::STAGE_SURVIVAL);
        if (isSurvivalModeFirstRound())
        {
            // ORIG: roundMaps = dungeon->m_vec814（CDungeon+0x814，startStage.md `lea 0x814(%eax)`）；
            //   P0：无 src 具名引用槽 ⇒ 直接内联赋值（旧 src 形多 sret 临时）。
            m_roundMaps = m_dungeon->m_vec814;
            std::random_shuffle(m_roundMaps.begin(), m_roundMaps.end());
            // ORIG: state=1、startStageTime=getCurSec、HistoryLog::enterDungeon。
            m_state = 1;
            m_startStageTime = GlobalData::s_systemTime_.getCurSec();
            HistoryLog::enterDungeon(*user, *this);
        }
        else
        {
            ++m_survivalRound;
        }

        // P211/P254：ORIG `mov 0x7c(%eax),%ebx` 单段直落 + `setae` 无符号 ⇒ 隐式转换。
        if (m_survivalRound >= m_roundMaps.size())
        {
            m_state = 7;
            return 0x15;
        }
        mapIdx = m_roundMaps[static_cast<unsigned int>(m_survivalRound)];
        m_mapBasisLevel = m_survivalRound;
    }
    else
    {
        reset(StageType::STAGE_NORMAL);
        m_difficulty = diff;
        m_state = 1;
        // ORIG: mapIdx/mapBasisLevel 按 difficulty 索引 CDungeon 表——
        //   mapIdx: `mov 0x8(%eax,%edx,4)` edx=diff+0x200 → dungeon+0x808+diff*4 = m_808[diff]
        //     （原实现漏 +0x8 基址，此为偏移修正；CDungeon.h +0x808 int[3]）；
        //   mapBasisLevel: `mov (%eax,%edx,4)` edx=diff+0x208 → dungeon+0x820+diff*4 = m_820[diff]。
        mapIdx = m_dungeon->m_808[static_cast<unsigned int>(diff)];
        m_mapBasisLevel = m_dungeon->m_820[static_cast<unsigned int>(diff)];
        m_startStageTime = GlobalData::s_systemTime_.getCurSec();
        HistoryLog::enterDungeon(*user, *this);
    }

    // P211 档③（b28d-solo-ProcStage）：find_map 返回类型修正为 CMap*（CDataManager.h，
    //   已 R2 广播）⇒ 本处无 cast、字段与 RHS 同型，复现 ORIG 0x0812e0a8
    //   `mov 0x8(%ebp),%edx; mov %eax,0x74(%edx)` LHS-first 直存。
    m_map = G_CDataManager()->find_map(mapIdx);
    if (m_map == 0)
    {
        return 0x15;
    }
    m_timeLine = &m_map->m_timeLine;   // CMap+0xf4 TimeLine（startStage.md `lea 0xf4(%eax)`）
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e16e（addCreatedMapMonster.md 全文）：seq = m_field14；
//   summary.m_seq(+0x04) = seq；m_field14 = seq+1；
//   m_createdMapMonsters.insert(make_pair(seq, summary))。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/addCreatedMapMonster.inc"
{
    // P0：ORIG 无 seq 槽（0x812e177 movzwl 后寄存器直存/lea 自增），++ 后缀单读双用。
    summary.m_seq = m_field14++;
    m_createdMapMonsters.insert(std::make_pair(summary.m_seq, summary));
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
{
    user.m_historyLog.KillMonster(monsterId,
                                  msg.m_killValue1,
                                  msg.m_killValue2,
                                  msg.m_killPairA24,
                                  msg.m_killPairA26,
                                  msg, (char)0, false, 0, 0,
                                  (eExpAddReason)1);
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
{
    // b28d-solo-ProcStage：ORIG 0x08133bf0 起每次重发 control.getProcStage()
    //   （P48/P0 表达式内联，无 procStage 具名槽）；-0x1c=startStageTime、-0x20=playSec
    //   为 ORIG 具名局部（`movl $0,-0x20` 预清 + `sub -0x1c,%eax` 回写）；
    //   数组 -0x11f 用 P312 `={0}` 慢路（旧显式循环为逐字节形）；
    //   stageIndex/ridableData/hist 全内联（ORIG 无槽，ridableData 经 %ebx 保活）。
    // ORIG 槽序：-0x20=playSec（先声明，函数头预清 `movl $0,-0x20`）、
    //   -0x1c=startStageTime；分支为 then 后置（`cmpl $0,-0x1c; jg <then>`）⇒ 反向条件。
    long playSec = 0;
    const int startStageTime = control.getProcStage()->getStartStageTime();
    if (startStageTime <= 0)
    {
        playSec = 0;
    }
    else
    {
        playSec = static_cast<long>(
            GlobalData::s_systemTime_.getCurSec() - startStageTime);
    }

    char memberNames[255] = {0};              // ebp-0x11f 起 0xff 字节清零
    getMemberNames(&user, memberNames);

    user.m_historyLog.DungeonClearInfo(
        static_cast<int>(isLast), playSec,
        control.getProcStage()->getStageIndex(),
        0, 1, ENUM_DUNGEON_MODE_0, memberNames,
        control.getCharacData()->m_field0);

    if (isLast)
    {
        HistoryLog::advanceAltarstatistics_.incrementEntranceData(
            static_cast<unsigned int>(control.getProcStage()->getStageIndex()),
            0, 1, 0);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x081339b0（enterDungeon.md 全文）：
//   names[0xff] 清零后 getMemberNames；EnterDungeon(stageIndex, difficulty, 1,
//   names, 2, 0)；incrementEntranceData(..., idx, 1,0,0) [未建模跳过]。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/enterDungeon.inc"
{
    // b28d-solo-ProcStage：ORIG 0x081339bb 是 4.1 的「对齐剥离 + rep stos + 尾字节」
    //   慢路（数组槽 -0x107 非 4 对齐）⇒ P312 形 `char buf[N]={0}` + 前置死局部
    //   （1 字节 pad 把数组顶到非对齐槽）；旧显式循环为逐字节形。
    char memberNames[255] = {0};             // ebp-0x107 起 0xff 字节清零
    getMemberNames(&user, memberNames);

    // P0：ORIG 无 difficulty/stageIndex/hist 具名槽——difficulty 经 %ebx 跨
    //   getStageIndex 调用保活（`mov %eax,%ebx`）、stageIndex 留 %eax、
    //   hist 由 `lea 0x79700(%edx),%ecx` 内联 ⇒ 全部表达式内联。
    user.m_historyLog.EnterDungeon(procStage.getStageIndex(),
                                   procStage.getStageDifficulty(), 1,
                                   memberNames, 2, 0);

    HistoryLog::advanceAltarstatistics_.incrementEntranceData(
        static_cast<unsigned int>(procStage.getStageIndex()), 1, 0, 0);
}


// ==== Wave5 AltarProc 追加（2026-08-27）=====================================

// ORIG T 0x0812cfa6（_ZN12advancealtar9isOnEventEv，worklist missing）：
// 事件开关全局函数，定义归 manager/批次；此处局部声明供 rewardClearStage 调用。
#include "advancealtar_ProcStage/isOnEvent.inc"

// ---- CharacData ------------------------------------------------------------

// ORIG W 0x08142088（objdump）：return *(int*)(this+0xc)。
#include "advancealtar_ProcStage/getGage.inc"
{
    return m_gage;
}

// ORIG W 0x08142094（objdump）：return *(int*)(this+8)。
#include "advancealtar_ProcStage/getGageLevel.inc"
{
    return m_gageLevel;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d176（upgreadGage.md 全文）：cfg = G_CDataManager()+0x6364；
//   level==3 → ret2；cost = (int)((float)m_gageUpAux[level] * m_4)（x87 截断，
//   无 +0.5f）再 `cost = *m_strPtr * cost`；m_gage<cost → ret3；否则扣费并按档位
//   0→1 / 1→2 / 2→3 推进 m_gageLevel，ret0。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/upgreadGage.inc"
                                         // 消费点 store 直存形定案 unsigned（见 .h 判词）
{
    ConfigParameter* cfg = &G_CDataManager()->m_altarConfig;  // CDataManager+0x6364（upgreadGage.md `add $0x6364`）
    if (m_gageLevel == 3)
    {
        return 2;   // ORIG 0x0812d194 `mov $0x2,%eax; jmp 尾`：满级错误码 2
    }
    // b28d-solo-ProcStage：ORIG 0x812d1b3 `fildl aux[level]` → `flds 0x4` → fmulp →
    //   `fistpl`（纯截断，无 +0.5f）→ `imul *m_strPtr` 覆盖同槽；base/ratio 均无具名槽。
    int cost = (int)((float)cfg->m_gageUpAux[static_cast<unsigned int>(m_gageLevel)] *
                     cfg->m_4);
    cost = *reinterpret_cast<int*>(cfg->m_strPtr) * cost;
    if (m_gage < cost)
    {
        return 3;   // ORIG 0x0812d1f0 `mov $0x3,%eax; jmp 尾`：gage 不足
    }
    m_gage -= cost;
    // ORIG 0x812d20b 决策树 `cmp $1; je L1; cmp $2; je L2; test; jne default` +
    //   叶块物理序 0,1,2 ⇒ 原码是 `switch (m_gageLevel)`（P251/P286；if-else 链
    //   在 4.4 恒反产 jne 内联块序）。
    switch (m_gageLevel)
    {
    case 0:
        m_gageLevel = 1;
        break;
    case 1:
        m_gageLevel = 2;
        break;
    case 2:
        m_gageLevel = 3;
        break;
    }
    return 0;   // ORIG 0x0812d23e `mov $0x0,%eax`（成功统一出口，非各臂就地写）
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d278（summonSlotUnit.md 全文）：cfg 同上；it=m_slotItems.find(slot)；
// 未命中 → 4；cost=**(int**)(cfg+0x18)*it->second.m_field8（iterator-> +0xc=
// pair second@+4 内 +0x08 字段）；条件 it->second.m_needGage==need（+0x08）
// 且 cost<=m_gage → 扣费 ret0；否则 ret2。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/summonSlotUnit.inc"
{
    ConfigParameter* cfg = &G_CDataManager()->m_altarConfig;  // CDataManager+0x6364（summonSlotUnit.md `add $0x6364`）
    std::map<unsigned char, _SlotItem>::const_iterator it = m_slotItems.find(slot);
    if (it == m_slotItems.end())
    {
        return 4;
    }
    // +0x18 = m_strPtr 双解引（mov 0x18(%eax);mov(%eax)）。
    // ORIG 0x812d…：先读 it->second.m_field8（0xc(%eax)→%edx）再解 m_strPtr，
    // imul %edx —— 乘法左操作数为 m_field8（求值顺序复现）。
    const int cost = it->second.m_field8 * *reinterpret_cast<int*>(cfg->m_strPtr);
    // b28d-solo-ProcStage 真语义 bug 修正：ORIG 0x0812d330 `mov 0x8(%eax),%eax`
    //   （eax=pair* ⇒ pair+0x8 = second+0x04 = m_itemId）与 need 比较；
    //   Ghidra C `if (*(int*)(iVar4 + 8) == param_2)` 联证。旧形误用 union 视角
    //   m_needGage（+0x08），比错字段 4 字节。
    if (it->second.m_itemId == need && cost <= m_gage)
    {
        m_gage -= cost;
        return 0;
    }
    return 2;
}

// ---- ProcStage -------------------------------------------------------------

// ORIG W 0x08134838（objdump）：return *(int*)(this+4)。
#include "advancealtar_ProcStage/getState.inc"
{
    return m_state;
}

// ORIG T 0x0812e1de（dieMob.md 全文）：先转发 ProcSummonObjectMs(+0x18)；
 // 命中返回 true；否则 m_createdMapMonsters 查 (short)mobSeq：end 或死亡标记
 // (m_posX 兼用) 为 1 → false；否则置 1 返回 true。
#include "advancealtar_ProcStage/dieMob_1.inc"
                       const MSG_MONSTER_DIE& msg)
{
    // P168a①：bool callee 裸负形 → ORIG `call; xor$1,%eax; test%al; je`（r1 探针）。
    if (!m_summonObjectMs.dieMob(mobSeq, param, msg))
    {
        std::map<short, _MapMonsterSummary>::iterator it =
            m_createdMapMonsters.find(static_cast<short>(mobSeq));
        // b28d-52：ORIG 0x0812e26f 起为「两负跳汇聚 + mov$1/mov$0 物化 + test」
        // （P214① 汇聚块）：正极性 && 短路直写条件（P0：ORIG 汇聚值无槽往返 ⇒
        // 无具名 bool 局部），字段按 P248 档② bool 化后用裸 !field。
        if (it != m_createdMapMonsters.end() && !it->second.m_padC)
        {
            it->second.m_padC = true;
            return true;
        }
        return false;
    }
    return true;
}

// ORIG T 0x0812e2c6（isEndStage.md 全文）：
#include "advancealtar_ProcStage/isEndStage.inc"
{
    const bool allPhaseEnd = isEndAllPhase();
    const bool summonEnd = m_summonObjectMs.isEndSummon();
    const int aliveSummons = m_summonObjectMs.getAliveMonsterCount();
    int aliveOnMap = 0;
    for (std::map<short, _MapMonsterSummary>::const_iterator it =
             m_createdMapMonsters.begin();
         it != m_createdMapMonsters.end(); it++)
    {
        // ORIG `movzwl 0x8(...)` 读 _MapMonsterSummary+0x06 m_towerRefA（short）零扩 u16，
        //   与 CMap+0x110 m_110（int，`mov 0x110(%eax)` 全 dword）比较；P225 推广：
        //   && 链拆嵌套 if 才能让 4.4 对中间比较发 `sete; test; je` 物化。
        if (m_map != 0)
        {
            if (static_cast<unsigned>(static_cast<unsigned short>(it->second.m_towerRefA)) ==
                static_cast<unsigned>(m_map->m_110))
            {
                if (!it->second.m_padC)
                {
                    ++aliveOnMap;
                }
            }
        }
    }
    if (aliveOnMap == 0)
    {
        endType = StageEndType::END_N0;
        return true;
    }
    // b28d-solo-ProcStage：ORIG 0x812e3cd 为正向包裹——`cmpb $0,slot; je <尾 mov$0>`
    //   ×2 + `cmpl $0; jne`，即 4.1 对 bool 槽直接 cmpb（P222 正极性；4.4 对
    //   `!bool` 恒 movzbl+xor，N122）。it++ 为 P28 后置形（ORIG 增量段有独立临时槽
    //   -0x18，使 begin/end 槽深移 4 位对齐 -0x20/-0x1c）。
    if (allPhaseEnd && summonEnd && aliveSummons == 0)
    {
        endType = StageEndType::END_N3;
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e3f6（clearStageAndReward.md 全文）：dungeon/user 判空直接返回；
 // clearStage(idx,diff,type,round,&star,&unlock,&bFlag)；成功 → 奖励表：
 // addAchievementReward(map) → rewardClearStage(vec) → setNextStage；
 // type==0 或 (type==1 && vec 非空) 且 decTicket()==0（扣票成功）时直接结束，
 // 否则走解锁商店/清关包分支。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/clearStageAndReward.inc"
{
    if (m_dungeon == 0)
    {
        return;
    }
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return;
    }
    int starCount = 0;
    bool unlockBuy = false;
    bool bFlag = false;
    // b28d-solo-ProcStage：P0/P183——ORIG 无 cam 引用槽（0x812e464 每次
    //   `mov -0x1c(%ebp),%edx; lea 0x8df60(%edx),%ecx` 内联）⇒ 全部实参内联。
    if (!user->m_advanceAltar.clearStage(
            getStageIndex(),
            static_cast<StageDifficulty::T>(m_difficulty),
            static_cast<StageType::T>(getStageType()),
            getClearSurvivalRoundNo(),
            starCount, unlockBuy, bFlag))
    {
        return;
    }

    std::map<int, _Reward> rewardMap;
    addAchievementReward(rewardMap);
    std::vector<_RewardItem> rewardItems;
    rewardClearStage(rewardItems);
    user->m_advanceAltar.setNextStage(getStageIndex());

    // b28d-solo-ProcStage：P48——ORIG 0x812e4ff/0x812e50e 两次独立 getStageType()
    //   （差集 MISSING ×1；旧具名 stageType 缓存少一条 call）；
    //   P197④——ORIG 0x812e51e `call size; test; je` ⇒ 原码 `size() != 0` 非 empty()；
    //   P168a 档①——ORIG 0x812e547 `call decTicket; xor $1; test; je` ⇒ bool 返回 + 裸 `!`。
    // ORIG 0x812e533 `test %al; je 主流程` + 0x812e54c `xor $1; test; je 主流程`
    //   ⇒ 单条 `if ((A||B) && !C) return;`（A/B 物化为 mov$1/mov$0 汇合）。
    //   P226 条件向语句表达式（内不含具名变量）复现 A||B 的零槽短路物化岛。
    if (({ getStageType() == 0 ||
          (getStageType() == 1 && rewardItems.size() != 0); }) &&
        !user->m_advanceAltar.decTicket())
    {
        return;
    }
    std::vector<_BuyItem> buyItems;
    if (unlockBuy)
    {
        user->m_advanceAltar.setUnlockBuyItem(
            static_cast<AdvanceAltarShopType::T>(0), getStageIndex(), &buyItems);
        user->m_advanceAltar.setUnlockBuyItem(
            static_cast<AdvanceAltarShopType::T>(1), getStageIndex(), &buyItems);
        user->m_advanceAltar.setUnlockBuyItem(
            static_cast<AdvanceAltarShopType::T>(2), getStageIndex(), &buyItems);
    }
    sendClearStage(rewardItems, buyItems, rewardMap, starCount, bFlag);
    if (0 < starCount)
    {
        HistoryLog::starAdd(*user, AddStarType::ADD_BY_CLEAR,
                            getStageIndex(), starCount);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812ea00（endProcAllKillMonster.md 全文）：user 空 → 1；
 // stageType==1：startStage(getStageIndex(), difficulty)==0 → state=3、ret0，
 // 非 0 → clearStageAndReward、ret1；其它 type：clearStageAndReward、ret1。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/endProcAllKillMonster.inc"
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return true;                   // ORIG ret1
    }
    if (getStageType() == 1)
    {
        // b28d-52：ORIG 0x0812ea64 为正向包裹（P222）——`test; setne; je <ret0 块>`
        // 成功路径 fall-through 调 clearStageAndReward；且 difficulty 经 %ebx
        // 跨 getStageIndex 调用保活（P0：无槽 ⇒ 表达式内联实参）。
        if (startStage(getStageIndex(),
                       static_cast<StageDifficulty::T>(getStageDifficulty())) != 0)
        {
            clearStageAndReward();
            return true;
        }
        m_state = State::S_N3;
        return false;                  // ORIG ret0：进入下一轮奖励态
    }
    clearStageAndReward();
    return true;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812eaa6（pause.md 全文）：pause=true 且 state==3 → state=2、ret1；
 // pause=false 且 state∈{2,1} → state=3、ret1；其余 ret0。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/pause.inc"
{
    if (pauseOn)
    {
        if (m_state != State::S_N3)
        {
            return 0;
        }
        m_state = State::S_N2;
    }
    else
    {
        if (m_state != State::S_N2 && m_state != State::S_N1)
        {
            return 0;
        }
        m_state = State::S_N3;
    }
    return 1;
}

// ORIG W 0x0812eb4e（getClearSurvivalRoundNo.md objdump）：return *(this+0x7c)。
#include "advancealtar_ProcStage/getClearSurvivalRoundNo.inc"
{
    return m_survivalRound;
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
{
    if (m_dungeon == 0)
    {
        return;
    }
    out.clear();

    // b28d-solo-ProcStage：P167 符号族——ORIG 迭代器为 const 族
    //   （`__normal_iteratorIPKN…` deEv×5 + ptEv×2、ZNK begin/end、ne(const,const)、
    //   ppEi），且**无** const_iterator 转换构造 ⇒ items 本身是 const 指针。
    const std::vector<_RewardItem>* items = 0;
    // b28d-solo-ProcStage：ORIG 0x812f12d `cmp $1; je <生存块>`（生存块后置）——
    //   4.4 对 if 形 `==1`/`!=1` 恒产 setcc 物化（各试一形），P286 单 case switch
    //   才出 ORIG 的寄存器比较直跳；default 块在前 = ORIG 普通块 fall-through。
    switch (getStageType())
    {
    default:
        // CDungeon+0x82c = m_clearReward（ClearRewardData），元素 stride 0xc = sizeof(vector<_RewardItem>)，
        //   按 difficulty 索引 m_vec[diff]（rewardClearStage.md `add $0x820`/`add $0xc` 折叠 base）。
        items = &m_dungeon->m_clearReward.m_vec[static_cast<unsigned int>(m_difficulty)];
        break;
    case 1:
        // CDungeon+0x850 = m_survivalReward（SurvivalRewardData，rewardClearStage.md `add $0x850`）。
        items = m_dungeon->m_survivalReward.getRoundRewardData(m_survivalRound);
        break;
    }

    std::vector<_RewardItem> mailNormal;   // local_28
    std::vector<_RewardItem> mailEvent;    // local_34
    if (items != 0)
    {
        int n = 0;   // P171 死计数器照抄（ORIG -0xc）
        // P28——ORIG 增量段 0x812f276 为 ppEi（后置）⇒ 写 it++。
        for (std::vector<_RewardItem>::const_iterator it = items->begin();
             it != items->end(); it++)
        {
            if (isOnEvent())
            {
                out.push_back(*it);
                if (it->m_type == 0)   // P167：ORIG ptEv（非 deEv）
                {
                    mailNormal.push_back(*it);
                }
                else
                {
                    mailEvent.push_back(*it);
                }
            }
            else if (it->m_type == 0)   // P167：ORIG ptEv
            {
                out.push_back(*it);
                mailNormal.push_back(*it);
            }
        }
        (void)n;   // P171：ORIG 仅 `movl $0,-0xc` 初始化、无自增（死写入照抄）
    }
    // b28d-solo-ProcStage：P197④——ORIG 0x812f2c2 `call size; test; setne; test; je`
    //   ⇒ 原码 `size() != 0`（empty() 在 4.4 恒 out-of-line call 且无 setne 段）。
    if (mailNormal.size() != 0)
    {
        sendMailRewardItem(m_characData->getUser(), mailNormal,
                           "chn_game_server_msg_88", "chn_game_server_msg_89");
    }
    if (isOnEvent() && mailEvent.size() != 0)
    {
        sendMailRewardItem(m_characData->getUser(), mailEvent,
                           "chn_game_server_msg_86", "chn_game_server_msg_87");
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812f3ae（addAchievementReward.md 全文）：user 判空；manager
 // getClearStageSummary(int[3]) 后逐槽(0..2)：data=RewardParameter@
 // G_CDataManager()+0x634c .getAchievementRewardData((StageDifficulty)i, sum[i])；
 // 命中则 addRewardData(data->m_type, out)（data+0 即 _Reward 首键/m_type）。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/addAchievementReward.inc"
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return;
    }

    int summary[3];
    std::memset(summary, 0, sizeof(summary));      // memset(local,0,0xc)
    user->m_advanceAltar.getClearStageSummary(summary);
    // b28d-solo-ProcStage：P0 槽-局部律——ORIG 0x0812f3e9 `lea 0x8df60(%eax),%edx` /
    //   0x0812f41f `lea 0x634c(%eax),%edx` 均寄存器直传（无槽）⇒ 原码是表达式内联，
    //   不得写具名引用/指针局部（旧形 cam/rewardParam 各占一槽、多 3 条并挤掉
    //   -0x14/-0x10 两个 ORIG 具名局部）；-0x14=diff（`mov -0x18,%eax; mov %eax,-0x14`）、
    //   -0x10=value（`mov -0x14,%eax; mov -0x28(%ebp,%eax,4),%eax; mov %eax,-0x10`）照抄。
    for (int i = 0; i < 3; ++i)
    {
        StageDifficulty::T diff = static_cast<StageDifficulty::T>(i);
        int value = summary[diff];
        AchievementRewardData* data =
            G_CDataManager()->m_altarReward.getAchievementRewardData(
                diff, value);
        if (data != 0)
        {
            user->m_advanceAltar.addRewardData(data->m_type, out);  // *(int*)data [+0x00]
        }
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812f47a（sendMailRewardItem.md 全文）：三参判空；标题经
 // RDARScriptStringManager 表 4 解析（sender 截 0x1e、title 截 0xff）；最多
 // 10 个 _RewardItem 转 pair(itemId,count) + Inven_Item[10] 数组；调用
 // MakeSystemMultiMailPostal(pairs,&items,count) 打包后
 // ReqDBSendNewSystemMultiMail(sender,...,title,strlen(title),0xf,group,false)。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/sendMailRewardItem.inc"
                                          const std::vector<_RewardItem>& items,
                                          const char* senderKey,
                                          const char* titleKey)
{
    if (user == 0 || senderKey == 0 || titleKey == 0)
    {
        return;
    }

    // b28d-solo-ProcStage：P0——ORIG 0x812f4c6/0x812f500 的 strncpy 第 2 参直接是
    //   findString 的 %eax（无 senderText/titleText 具名指针槽；旧形多 2 槽 ⇒
    //   全帧深 3）。循环计数器槽序：-0x14=count（在 itemBase 前声明、体首 ++）、
    //   -0x10=i（在 pairs 后声明、体尾 ++、条件 `cmpl $0x9` 用它）。
    // b28d-solo-ProcStage：P312——ORIG 数组槽基比 4.4 深 5 字节（sender -0x4b
    //   vs -0x46）⇒ 前置 5 字节死局部把数组组整体顶到 ORIG 同址。
    char framePad[1];
    (void)framePad;
    char sender[30];                      // ebp-0x4b 起 0x1e
    std::strncpy(sender, g_scriptStringManager_.findString(4, senderKey, 0),
                 sizeof(sender));
    char title[255];                      // ebp-0x15c 起 0xff
    std::strncpy(title, g_scriptStringManager_.findString(4, titleKey, 0),
                 sizeof(title));

    unsigned int count = 0;               // -0x14
    Inven_Item itemBase[10];              // ebp-0x3be，stride 0x3d 十次构造
    std::vector<std::pair<int, int>> pairs;
    int i = 0;                            // -0x10（上限 10）
    for (std::vector<_RewardItem>::const_iterator it = items.begin();
         it != items.end() && i < 10; it++, ++i)
    {
        ++count;
        // P178 嵌套实参形：ORIG 0x812f5a4 make_pair → pair<int,int> 转换 ctor → push_back。
        pairs.push_back(std::pair<int, int>(
            std::make_pair(it->m_value1, it->m_value2)));
    }

    // b28d-solo-ProcStage：P171——ORIG 0x812f631 `movl $0xf,-0xc(%ebp)` 死写入
    //   （调用点仍压立即数 0xf）⇒ 具名 int 局部照抄，同时把 count/i 槽顶深 4。
    int fieldType = 0xf;
    (void)fieldType;
    WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(pairs, itemBase, count);
    // b28d-solo-ProcStage：P0/P44——ORIG 0x812f65f/0x812f66f/0x812f684 把
    //   group/titleLen/characNo 留在 %ebx/%esi/%edx（无槽）⇒ 全部实参内联；
    //   求值序 = 右→左（group → titleLen → characNo），与 ORIG 装载序一致。
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
        sender, itemBase, count, 0,
        static_cast<unsigned int>(user->get_charac_no(-1)), title,
        static_cast<unsigned int>(std::strlen(title)), 0xf,
        user->GetServerGroup(), false);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e6de（sendClearStage.md 全文，const 方法）：ridableId=current
 // 数据对象 +0x18 u16 符号扩展；包体 _NotiAdvanceAltarClearStage(0x1c packed)
 // {stageType, clearRound, ridable, clearFlag, stage{idx,diff}, star}；
 // put_header(0,0x20f)+put_binary(0x1c)；rewards：put_int(size)+逐元素 0xc；
 // buyItems 同 0xc；rewardMap：put_int(size)+逐 value(_Reward 5B)；finalize→Send。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/sendClearStage.inc"
                               const std::vector<_BuyItem>& buyItems,
                               const std::map<int, _Reward>& rewardMap,
                               int starCount, bool clearFlag) const
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return;
    }

    short ridableId = 0;
    AdvanceAltarData* current = user->m_advanceAltar.getCurrentAdvanceAltarData();
    if (current != 0)
    {
        // AdvanceAltarData(=_RidableBase)+0x18 = m_maxClearRound（short；objdump 0x0812e728 `movzwl 0x18(%eax)`）。
        ridableId = current->m_maxClearRound;
    }

    NOTIPacketStruct::_NotiAdvanceAltarClearStage body;
    body.m_stageType = getStageType();
    body.m_clearSurvivalRoundNo = getClearSurvivalRoundNo();
    body.m_ridableId = ridableId;
    body.m_clearFlag = clearFlag;
    // b28d-solo-ProcStage：ORIG 0x812e76b/0x812e776 逐字段直写 packed 子对象
    //   （`call getStageIndex; mov %eax,-0x3f` + `mov 0x68(%eax),%eax; mov %eax,-0x3b`）
    //   ⇒ 不得用 int[2]+memcpy（多一条 memcpy call，P167 EXTRA）。
    body.m_stage.m_field0 = getStageIndex();
    body.m_stage.m_field4 = static_cast<StageField4_t>(m_difficulty);
    body.m_starCount = starCount;

    PacketGuard guard;
    guard.put_header(0, 0x20f);
    guard.put_binary(reinterpret_cast<char*>(&body), 0x1c);
    // b28d-solo-ProcStage：ORIG 0x812e7c8 先 size() 缓存到 -0xc，put_int 复用缓存，
    //   随后 `cmpl $0,-0xc; jle <跳过循环>` —— 原码是 `int n=rewards.size();
    //   put_int(n); if (n > 0) { for(...) }`（buyItems/rewardMap 两段无此守卫）。
    const int rewardCount = static_cast<int>(rewards.size());
    guard.put_int(rewardCount);
    if (rewardCount > 0)
    {
        for (std::vector<_RewardItem>::const_iterator it = rewards.begin();
             it != rewards.end(); it++)   // P28：ORIG 增量段 ppEi（后置）
        {
            // ORIG 0x812e820 调用 put_binary(char*,int)（Pc 非 const 重载；P167 差集：
            //   ORIG `_ZN18InterfacePacketBuf10put_binaryEPci` vs 旧 `EPKci`）⇒ C cast 去 const。
            guard.put_binary((char*)&*it, 0xc);
        }
    }
    guard.put_int(static_cast<int>(buyItems.size()));
    for (std::vector<_BuyItem>::const_iterator it = buyItems.begin();
         it != buyItems.end(); it++)   // P28：ORIG 增量段 ppEi（后置）
    {
        guard.put_binary((char*)&*it, 0xc);
    }
    guard.put_int(static_cast<int>(rewardMap.size()));
    for (std::map<int, _Reward>::const_iterator it = rewardMap.begin();
         it != rewardMap.end(); it++)   // P28：ORIG 增量段 ppEi（后置）
    {
        // ORIG 0x812e944 `call operator->; add $0x4,%eax` ⇒ &it->second（Pc 版）。
        guard.put_binary((char*)&it->second, 5);
    }
    guard.finalize(true);
    user->Send(guard);
}

// ---- HistoryLog ------------------------------------------------------------

// ----------------------------------------------------------------------------
// ORIG T 0x08133abc（leaveDungeon.md 全文）：names[0xff] 清零 + getMemberNames；
 // LeaveDungeon(hist@user+0x79700, stageIndex, difficulty, names, 0)。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/leaveDungeon.inc"
{
    // b28d-52：ORIG 0x08133abc `mov $0x40,%edx; rep stos` = 0x100 字节零初始化
    // （P214③：char buf[N]={0} 复现 4.1 rep stos；旧 255 + 显式循环为逐字节形）。
    char memberNames[0x100] = {0};
    getMemberNames(&user, memberNames);

    // P0：ORIG 无 difficulty/stageIndex 槽（difficulty 入 %ebx 跨调用保活）⇒ 实参内联。
    user.m_historyLog.LeaveDungeon(procStage.getStageIndex(),
                                   procStage.getStageDifficulty(), memberNames, 0);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08133d56（starAdd.md 全文）：LogHistory("star+,%d,%d,%d")+
// incrementEntranceData(advanceAltarstatistics_, idx,0,0,cnt)。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/starAdd.inc"
                         int count)
{
    user.LogHistory("star+,%d,%d,%d", static_cast<int>(type), stageIndex,
                    count);
    HistoryLog::advanceAltarstatistics_.incrementEntranceData(
        static_cast<unsigned int>(stageIndex), 0, 0,
        static_cast<unsigned int>(count));
}

// ORIG T 0x08133db0（starSub.md 全文）：仅 LogHistory("star-,%d,%d")。
#include "advancealtar_ProcStage/starSub.inc"
{
    user.LogHistory("star-,%d,%d", static_cast<int>(type), value);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08133e30（updateStatistics.md 全文）：CAdvanceAltarLog::updateDatabase
 // (advanceAltarstatistics_)。依赖随 CAdvanceAltarLog 批次解锁。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/updateStatistics.inc"
{
    HistoryLog::advanceAltarstatistics_.updateDatabase();
}

// ----------------------------------------------------------------------------
// ORIG T 0x08133dda（buyItem.md 全文）：
 // incrementLayerStatData(advanceAltarstatistics_, shopType, param2, param4, 1,
 // param5)——param3(第 1 个 short) 未参与调用，ORIG 原样。
// ----------------------------------------------------------------------------
#include "advancealtar_ProcStage/buyItem.inc"
                         short c, int d)
{
    HistoryLog::advanceAltarstatistics_.incrementLayerStatData(
        static_cast<unsigned short>(shopType), static_cast<unsigned int>(a),
        static_cast<unsigned short>(c),
        static_cast<unsigned short>(1) /* useCountDelta 固定 1 */,
        static_cast<unsigned short>(d));
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
{
    if (user == 0)
    {
        return;
    }
    if (buf == 0)
    {
        return;
    }
    sprintf(buf, "\"%s\",\"%s\",\"N/A\",\"N/A\",\"N/A\"",
            user->getCurCharacName(), user->getCurCharacName());
}
}  // namespace advancealtar
