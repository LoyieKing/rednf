// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 advancealtar_StageTimeline/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG 0x08134650 W：memset(this, 0, 0x24)。
#include "advancealtar_StageTimeline/_SummonObject.inc"
}

// ORIG 0x0889b978 W：memset(this, 0, 0x14)。
#include "advancealtar_StageTimeline/_Action.inc"
}

// ORIG 0x08899c62 T：+0x00 清零 + vector<_Action>@+0x04 默认构造。
#include "advancealtar_StageTimeline/ActionDefine.inc"
}

// ORIG 0x08899c82 T：析构 vector<_Action>（成员自动析构即一致）。
#include "advancealtar_StageTimeline/dtor_ActionDefine.inc"
}

// ORIG 0x08899c98 T：id=0 + m_actions.clear()。
#include "advancealtar_StageTimeline/reset.inc"
}

// 布局断言（报告/nm 实证尺寸）。
typedef char assert_summon_0x24[(sizeof(_SummonObject) == 0x24) ? 1 : -1];
typedef char assert_action_0x14[(sizeof(_Action) == 0x14) ? 1 : -1];
typedef char assert_actdef_0x10[(sizeof(ActionDefine) == 0x10) ? 1 : -1];

// ---- StageTimeLineParameter ------------------------------------------------

// ============================================================================
// reset @ 0x08899ef6：清两个 map + TimeLine::reset（0x08899dfa，位于
// advancealtar_StageControl.cpp）。无其它副作用。
// ============================================================================
#include "advancealtar_StageTimeline/reset_1.inc"
}

// ============================================================================
// debugLog @ 0x889a02c：push %ebp / mov %esp,%ebp / pop %ebp / ret —— 纯空函数。
// ============================================================================
#include "advancealtar_StageTimeline/debugLog.inc"
}

// ============================================================================
// importStageTimeLineParameter @ 0x8899f26：
//   reset() → loadRDARScriptFile("", path) 失败返 0；
//   循环 ScanType(token,true)：
//     - 失败（EOF）→ 返 0；
//     - "[summon object]" → importSummonObject()，非 0 即作为错误码返回；
//     - "[action define]" → importActionDefine()，非 0 即作为错误码返回；
//     - 其它 token 跳过。
// 返回 0 = 成功（init.md 口径非零=Fail），非 0 = 子段错误码。
// ============================================================================
#include "advancealtar_StageTimeline/importStageTimeLineParameter.inc"
}

// ============================================================================
// importSummonObject @ 0x889a21e：
//   段内逐行：key isEnemy("enemy"/"friend") type("normal"/"named"/"boss")
//   i1 i2(存低16位) i3 i4 i5 i6 → 插入 m_summonMap（insert，key=m_id）。
//   行首 ScanInt 失败 = 段结束（[/summon object]/EOF）→ 返 0；
//   任一子项失败 → 返对应错误码。
// ============================================================================
#include "advancealtar_StageTimeline/importSummonObject.inc"
}

// ============================================================================
// importActionDefine @ 0x889a56e：
//   外层循环每轮重建 ActionDefine；ScanType(token,true)：
//     - 失败（段内 EOF）→ 错误 0x253；
//     - "[/action define]" → 返 0（正常收段，调用方按 0 继续）；
//     - "[normal]"/"[named]"/"[boss]" → ad.id=ScanInt(ok)（失败 0x26b），随后
//       内层循环逐行 _Action：summonId=ScanInt(ok)（失败=行耗尽→插入并续外层），
//       find(summonId) 未命中 → printf + 0x27a；m_pSummon=&命中值；
//       firstDelayMs<0 → 0x285；nextDelayMs<500 → 0x290；
//       第 4 列 !=0 → flag=1 且 flagValue=该列低 16 位。push_back 后循环。
//       插入 make_pair(ad.id, ad) 后外层继续。
//     - 其它 token → printf "Type undefined" + 0x267。
// ============================================================================
#include "advancealtar_StageTimeline/importActionDefine.inc"
}

}  // namespace advancealtar
