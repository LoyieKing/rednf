// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_StageTimeline/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar::StageTimeLineParameter 实现（EventStageDeps）。
// 覆盖 ORIG 符号（均经 nm 确认 T/W）：
//   - reset()                        0x08899ef6 T
//   - importStageTimeLineParameter() 0x08899f26 T
//   - debugLog()                     0x0889a02c T（ORIG 纯 ret）
//   - importSummonObject()           0x0889a21e T
//   - importActionDefine()           0x0889a56e T
// 以及仅被本域使用的元素类型生命周期：
//   - _SummonObject::_SummonObject   0x08134650 W（memset 0x24）
//   - _Action::_Action               0x0889b978 W（memset 0x14）
//   - ActionDefine C1/D1/reset       0x08899c62/0x08899c82/0x08899c98 T
// 逐函数对照 docs/class_func_reports/advancealtar__StageTimeLineParameter/
// [G5 2026-09-10 · L12 round3 复核] 2 件全核（p167 + widthscan + 判据⑪）：
//   · importStageTimeLineParameter：判据⑪ = (b) 整帧平移（79/79）⇒ N126 封顶，
//     p167 MISSING=EXTRA=2（别名）。
//   · importActionDefine（gap +11）：widthscan 报 `clr_l orig=0 our=10`
//     （我方 9×`movl $0x0,-0x10`=state、1×`-0x14`=err）⇒ ORIG 的 err/state 由
//     esi/ebx 寄存器承载（无槽）；本波 P231 纯 register（非 asm 绑定，P0 合规）
//     实测 286→288 insns 反劣 ⇒ 回滚，判 N（槽/寄存器分配差）；p167 MISSING=EXTRA=8
//     （别名）⇒ 无语义缺口。
//   · [C05③ 2026-09-10 F3c 复核] ORIG 同目标 3 条（0889a5dd/0889a5f4/0889a60b
//     `jne 0x0889a64c` = body 入口）在我方是 `||` 短路 bool 物化形
//     （`mov $1/$0,%eax; test %al,%al; je`，非 `jmp+nop` 岛）；两侧 `jmp+nop` 岛
//     计数 2:2（函数头 + 内层回边）⇒ 闸门「我方岛 − ORIG 岛 ≥ 2」不成立，
//     守卫亦非源码相邻独立 `if(cond) goto L;`（是 `||` 条件表达式）⇒ 维持 N。
# include "CDataManager.h"    // advancealtar::StageTimeLineParameter/_SummonObject/
                              // _Action/ActionDefine 权威声明
#include "DNFLexWrapper.h"    // loadRDARScriptFile/ScanType/ScanInt(bool*)/ScanStr

#include <cstdio>
#include <cstring>

namespace advancealtar
{

// ---- 元素类型生命周期 ------------------------------------------------------

#include "advancealtar_StageTimeline/_SummonObject.inc"


#include "advancealtar_StageTimeline/_Action.inc"
{
    std::memset(this, 0, sizeof(*this));
}

#include "advancealtar_StageTimeline/ActionDefine.inc"
    : m_id(0), m_actions()
{
}

#include "advancealtar_StageTimeline/dtor_ActionDefine.inc"
{
}

#include "advancealtar_StageTimeline/reset.inc"
{
    m_id = 0;
    m_actions.clear();
}

// 布局断言（报告/nm 实证尺寸）。
typedef char assert_summon_0x24[(sizeof(_SummonObject) == 0x24) ? 1 : -1];
typedef char assert_action_0x14[(sizeof(_Action) == 0x14) ? 1 : -1];
typedef char assert_actdef_0x10[(sizeof(ActionDefine) == 0x10) ? 1 : -1];

// ---- StageTimeLineParameter ------------------------------------------------

#include "advancealtar_StageTimeline/reset_1.inc"


#include "advancealtar_StageTimeline/debugLog.inc"


#include "advancealtar_StageTimeline/importStageTimeLineParameter.inc"


#include "advancealtar_StageTimeline/importSummonObject.inc"


#include "advancealtar_StageTimeline/importActionDefine.inc"


}  // namespace advancealtar
