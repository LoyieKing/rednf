// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EmblemCompoundServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EmblemCompoundServer + ARAD::SCRIPT::EmblemCompound 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__EmblemCompound/*.md 与
// docs/class_func_reports/EmblemCompoundServer/{EmblemCompoundServer,~EmblemCompoundServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstdio>
#include <string>

#include "EmblemCompoundServer.h"
#include "GameTypes.h"      // cMyTrace
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证）----
typedef char emblem_compound_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound) == 0x84 ? 1 : -1];
typedef char emblem_compound_result_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound::Result) == 0xa ? 1 : -1];
typedef char emblem_compound_server_size_assert[sizeof(EmblemCompoundServer) == 0x88 ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

// ============================================================================
// EmblemCompound —— ORIG ctor 0x088b1408 / dtor 0x088b14c2。
// 成员按声明序自构造/自析构即与 ORIG 一致：
//   ctor：map(+0x00) → map(+0x18) → 7×vector(+0x30 起，步长 0xc)
//   dtor：逆序析构 7 个 vector → map(+0x18) → map(+0x00)
// ============================================================================
#include "EmblemCompoundServer/EmblemCompound.inc"
}

#include "EmblemCompoundServer/dtor_EmblemCompound.inc"
}

// ============================================================================
// loadScript —— ORIG T 0x088b154a。
// "Etc/EmblemRenewal/" + "emblem_compound_jpn.etc"；三节：
//   [emblem compound cost]   grade value         → m_cost（重复 grade → printf+-2）
//   [emblem compound result] grade 4×short flag  → m_result（new Result(0xa)；
//                              重复 grade → delete+printf+-2）
//   [emblem list]            listIdx itemIndex   → m_list[listIdx].push_back
// 循环形态与 ORIG 相同：节内由 ScanTypeOrInt 取数值项（isType==false&&isInt==true
// 才处理）；遇 TYPE/EOF 退出后回到外层 ScanType(token,true)。查重均为
// end()→find()→operator==、命中则插入否则报错的分支形态（与 ORIG 一致）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
// [b28d-solo-EmblemCompoundServer] loadScript 判词（ORIG 0x088b154a，273 insn）。
// 已修：外层 while→for(;;)+头部 break 复现 4.1 条件块前置布局（P283/P164，消 ~30 行
// 布局差）；new Result()（P90 value-init）；list 节 int 槽 + prvalue 转换临时
// （p167 实证 ORIG 调 push_back(T&&) `EOm`，具名左值会错选 `RKm`；修正后 p167 CLEAR
// 35/35）。
// 残余 N：ORIG +0x1c2 的 value-init 前置清零为 4.1 循环展开
// （`mov;mov;mov;movw;mov;movw;…;je;add;jmp` 清 8B + movb 尾，9 条），4.4 恒
// 展开 4×movw+movb（探针 /tmp/nr_probe.cpp `new R()` 实证，5 条）⇒ N78/跨版本
// 展开形态不可复现；gap 6 全在此段。其余助记符序列逐条同构（mnem-diff 0 块）。
#include "EmblemCompoundServer/loadScript.inc"
}

// ============================================================================
// clearScript —— ORIG T 0x088b1926：清 cost map → begin/end 各取一次后遍历
// delete Result* → 清 result map。不清理 m_list（ORIG 如此）。
// ============================================================================
#include "EmblemCompoundServer/clearScript.inc"
}

// getCost —— ORIG T 0x088b19b0：find→end→operator!=，命中返回 value，未命中 -1。
#include "EmblemCompoundServer/getCost.inc"
}

// getResult —— ORIG T 0x88b1a14：同上形态，命中返回 Result*，未命中 0。
#include "EmblemCompoundServer/getResult.inc"
}

#include "EmblemCompoundServer/getList.inc"
}

}  // namespace SCRIPT
}  // namespace ARAD

// ============================================================================
// 注：ORIG C1 中间的 ARAD::Singleton<EmblemCompoundServer>::Singleton()（W
// 0x08193636，空基类构造）未建模——与 CerashopAddRestrict::Manager 同口径，
// Singleton<T> 采用 Arad_DataManager.h 的 Meyers 模板。
// ============================================================================
#include "EmblemCompoundServer/EmblemCompoundServer.inc"
}

#include "EmblemCompoundServer/dtor_EmblemCompoundServer.inc"
}

// Load —— ORIG T 0x081915ba：cMyTrace(pretty,0x1c7,9,true,false) +
// operator()("\t- Loading Emblem Compound script - ") + loadScript()==0。
#include "EmblemCompoundServer/Load.inc"
}

// Destroy —— ORIG T 0x0819162c：return 1。基类 ScriptInterface 槽1 为 void
// （仓库权威约定），可观察行为一致。
#include "EmblemCompoundServer/Destroy.inc"
}

// Test —— ORIG T 0x081915b4：空（非虚成员）。
#include "EmblemCompoundServer/Test.inc"
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   GetEmblemInfo        ORIG T 0x08191818（GetEmblemInfo.md 全文转写）
//   GetEmblemRandomIndex ORIG T 0x081919e8（GetEmblemRandomIndex.md 全文转写）
//   IsResultSelectable   ORIG T 0x08191ad6（IsResultSelectable.md 全文转写）
//   CheckCondition       ORIG T 0x08191636（CheckCondition.md 全文转写）
//   Process              ORIG T 0x08191afa（Process.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "MoreTypes.h"          // arad::SigAradEmblemCompound 完整定义
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item / ENUM_CMDPACKET
#include "LogManager.h"
#include "PacketGuard.h"
#include "CMailBoxHelper.h"     // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"
#include "CompoundEmblemDeps.h" // getCompoundEmblemStatistic
#include "AvatarConvertServer.h" // StatisticsEmblemCreate

// [b28d-solo-EmblemCompoundServer] GetEmblemInfo 判词（ORIG 0x08191818，126 insn）：
// IDENTICAL/AE。修法：SigAradEmblemCompound 加 materialInfo[2] 匿名 union 数组视角
// （P235/P246，scaled 索引 `(%eax,%edx,8)`）；slot 内联实参无槽（P0/P157）；
// materialInfo::itemIdx 同型化为 unsigned int 复现 ORIG `mov 0x2(%eax),%ecx` 单条
// 直装（P211 档③/P248；int 字段恒 `mov→%eax; mov %eax,%ecx` 两段）。
#include "EmblemCompoundServer/GetEmblemInfo.inc"
}

// [b28d-solo-EmblemCompoundServer] GetEmblemRandomIndex 判词（ORIG 0x081919e8，77 insn）：
// IDENTICAL/AE。修法：listIdx 改 `int listIdx = 0`（ORIG movl 清零 ⇒ P10）；
// get_rand_int 结果具名 int 局部（槽 -0x10）；empty 后重发 m_result[grade]
// （P157 逐使用点重读）+ `if(!m_selectable)` 取反形（P168a/P292，xor $1）。
#include "EmblemCompoundServer/GetEmblemRandomIndex.inc"
}

#include "EmblemCompoundServer/IsResultSelectable.inc"
}

// [b28d-solo-EmblemCompoundServer] CheckCondition 判词（ORIG 0x08191636，124 insn）：
// IDENTICAL/AE。修法：grade0 具名 int 局部（ORIG 槽 -0xc，movzwl+cwtl）；money/need
// 去具名局部改内联（`get_money() < m_cost[grade0]`，P52 左值先物化 %ebx）；
// logFormat 的 money/need 逐实参重算（P157/P92）。
#include "EmblemCompoundServer/CheckCondition.inc"
}

// [b28d-solo-EmblemCompoundServer] Process 判词（ORIG 0x08191afa，426 insn）。
// 已修：补 `newItem.m_addInfo = resultItem->get_index()`（ORIG +0x2fc，原缺此步）；
// make_pair 改 `(unsigned long&)m_addInfo` 左值直传复现 ORIG 符号
// `_ZSt9make_pairIRmRiE…`（0x08111a37）；put_byte 实参改
// `Singleton<EmblemCompoundServer>::Get()->IsResultSelectable(grade0)`（ORIG +0x555，
// 非 this，且去 `?1:0` 物化）；resultIdx 改 unsigned int 复现 find_item 实参先装
// %ebx 跨 receiver call（P290/P52）；cost/invW 去具名局部（P157 重取）。
// 残余 N：ORIG +0x3e4 `movzwl; not; shr $0xf,%ax; test %al; jne`（4.1 对 short<0
// 的符号位提取 lowering）；4.4 对 `< 0` 恒 `cmpw $0; jns`，对 (unsigned short)~x>>15
// 形恒提升 32 位（movzwl+sar 6 条，实测）⇒ 跨版本 lowering 封顶（P196 负例族），
// gap 3 全在此段；其余助记符序列逐条同构（mnem-diff 仅此 1 块）。
#include "EmblemCompoundServer/Process.inc"
}
