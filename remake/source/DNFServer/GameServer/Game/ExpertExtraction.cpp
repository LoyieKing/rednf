// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExpertExtraction/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// expert_extraction 域 —— ORIG 19 符号实现（报告
// docs/class_func_reports/expert_extraction/ 与 expert_extraction__C* 各子目录）。
// 类声明见 CExtraction.h / CAlchemistExtraction.h / CEnchanterExtraction.h /
// CDollControllerExtraction.h / CExtractionMgr.h（ctor/dtor 定义 GlobalData.cpp）。
// vtable 槽序（vptr 指向 vtable+8，ctor vptr 赋值 + extraction 族虚调用实测）：
//   +0x08 extraction / +0x0c check_error / +0x10 inven_process /
//   +0x14 _check_need_slot / +0x18 get_extraction_count / +0x1c _check_error
// ============================================================================

#include "CExtraction.h"
#include "CAlchemistExtraction.h"
#include "CEnchanterExtraction.h"
#include "CDollControllerExtraction.h"
#include "CExtractionMgr.h"

#include "CUser.h"          // CUserCharacInfo / CUser::CheckItemLock / InventoryMemory
#include "CInventory.h"     // CInventory / CSecu_ProtectionField / GlobalData::s_pSecuProtectionField
#include "CDataManager.h"   // G_CDataManager / find_item / GetExpertJobScript
#include "CItem.h"
#include "CEquipItem.h"     // CEquipItem::m_itemType（+0x234）
#include "CStackableItem.h" // m_int334/+0x334、m_int338/+0x338、m_int33c/+0x33c
#include "STExpertJobScript.h"
#include "STItemScript.h"   // ENUM_EXPERT_JOB_TYPE
#include "GameWorld.h"      // G_GameWorld / IsPVPChannel
#include "GameTypes.h"      // cMyTrace / LogManager::logFormat
#include "PvPTypes.h"       // CMTRand::randInt

#include <utility>

namespace expert_extraction
{

// ============================================================================
// GetExtracterType —— ORIG T 0x084a2904（报告 GetExtracterType.md）
//   专业职业类型 → 提取器索引：2→0(Alchemist)/4→2(DollController)/1→1(Enchanter)/其它→3
// ============================================================================
#include "ExpertExtraction/GetExtracterType.inc"
}

// ============================================================================
// CExtraction::_check_need_slot —— ORIG T 0x084a29f4
//   两个空槽位（ITEM_TYPE 10 与 0）任一存在即可。
// ============================================================================
#include "ExpertExtraction/_check_need_slot.inc"
}

// ============================================================================
// CExtraction::check_error —— ORIG T 0x084a2a5e（报告 check_error.md）
//   PVP 频道拒 0x60；职业不符 0x13；材料1 必须存在且可堆叠（0x11）；
//   材料2（被提取装备）：非头像/非生物/类型合法（0x11）、稀有度==2（0x8c）、
//   已鉴定（0xae）、可分解（0x17）、非绑定（0x11）、未锁（0xd5）、
//   安全保护 CheckItemRarity（0x28）返回其错误码。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：attachType 改单 case switch，复现 ORIG
//   `mov -0x10(%ebp),%eax; cmp $0x2,%eax; jne`（P286/P209；原 `if` 恒折 cmpl mem 直比）。
//   残余 N（P75 跨版本实参装载调度）：CheckItemRarity 末段 ORIG 4.1 序为
//   `c2→%edx; s_pSecuProtectionField→%eax; &item2→%ecx`，4.4.7 恒
//   `this→%eax; &item2→%edx; c2→%edx`；cast/(CUser*)/reinterpret_cast receiver/
//   单行合并 4 形实测均不翻。p167 17/17 CLEAR（语义完整，gap=0）。
#include "ExpertExtraction/check_error.inc"
}

// ============================================================================
// CExtraction::inven_process —— ORIG T 0x084a2cec（报告 inven_process.md）
//   副本背包试删材料 → find_item 构造新物品（make_item + set_add_info + 物品索引）
//   → 副本试插（0x35,0,1）；成功后真实背包删材料再插入，outSlots 输出
//   {插入槽, 物品索引, add_info}。
// ============================================================================
// [b28d-solo-ExpertExtraction] NEAR 修复：声明序改 copy→mem（ORIG 构造序 CInventory
//   先于 InventoryMemory）、delete_item 结果改 bool 具名局部（P158 复现
//   `movzbl; xor $1; test; je`）、slot 单变量复用（ORIG 两次 insert 同落 -0x20，
//   P160-162 槽复用律）。残余 N95（1 字节栈对齐）：mem 槽基 ORIG -0xc957（4.1 按
//   0xc13f 不补齐）vs 4.4 -0xc958（按 4 对齐）；删 CUser.h 的 m_tailC13f 亦不能上移
//   （4.4 强制对齐，且 CUser 中该成员因对齐仍占 0xc140、布局不变）。p167 23/23 CLEAR。
#include "ExpertExtraction/inven_process.inc"
}

// ============================================================================
// CExtractionMgr::get_extracter —— ORIG T 0x084a3166（报告 get_extracter.md）
// ============================================================================
#include "ExpertExtraction/get_extracter.inc"
}

// ============================================================================
// CAlchemistExtraction::get_alchemist_extraction_type —— ORIG T 0x084a355c
//   m_itemType(+0x234) 跳表：10→2；0xc..0x10→0；0x11..0x15→1；其它→3
// ============================================================================
#include "ExpertExtraction/get_alchemist_extraction_type.inc"
}

// ============================================================================
// CAlchemistExtraction::get_extraction_count —— ORIG T 0x084a3596
//   grade / 5，0 → 1。
// ============================================================================
#include "ExpertExtraction/get_extraction_count.inc"
}

// ============================================================================
// CAlchemistExtraction::_check_error —— ORIG T 0x084a35f2
//   材料1（槽 a）的 +0x334 等级字段 → 脚本允许等级；材料2（槽 b）的
//   usableLevel（0 时 grade）；超出 → 0xd。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：mat1->m_int334 提升为具名 level 局部
//   （P0：ORIG `mov 0x334(%eax),%eax; mov %eax,-0x14(%ebp)` 槽往返后回读传参）
//   + 比较改 `if (need > limit)` 复现 ORIG `mov need,%eax; cmp limit,%eax; jle`
//   （原 `limit < need` 产 jge 反向，P196/P47）。三派生类同族（Enchanter 0x084a3b92 /
//   DollController 0x084a4286 同形）。p167 13/13 CLEAR。
#include "ExpertExtraction/_check_error.inc"
}

// ============================================================================
// CAlchemistExtraction::check_error —— ORIG T 0x084a3716
// b28d-54 判词：ORIG 0x084a3716 尾部 `call *%ecx; mov %eax,-0xc(%ebp);
//   mov -0xc(%ebp),%eax`（槽往返）⇒ 原码有具名局部 `e = _check_error(...); return e;`
//   （P0：ORIG 有槽往返 ⇒ 必须声明局部；直接 `return f()` 丢 store/load 对）。
// ============================================================================
#include "ExpertExtraction/check_error_1.inc"
}

// ============================================================================
// CAlchemistExtraction::extraction —— ORIG T 0x084a3182（报告 extraction.md）
//   按结果表随机选一条目（CMTRand），命中则 get_extraction_count 作数量，
//   needItems 输出 {itemId, count}。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复（三旋钮）：①`pick = randInt(range); ++pick;`
//   两句复现 ORIG `mov %eax,-0x18; addl $1,-0x18`（内存自增）；②三处 cMyTrace 改
//   全表达式临时链 `cMyTrace(...)(fmt, vec.size())`（P227：ORIG 在 ctor 前求值 size()
//   并入 %ebx 跨 ctor 保活；P221 槽方向随之翻正）；③删具名 count，把
//   `(int)get_extraction_count(item)` 内联进 make_pair 第二实参——ORIG 的 -0x44 是
//   `(int)` 右值临时（mangled 实证 make_pair<int&,int>(int&,int&&) @0x080dd799），
//   具名局部会令 4.4 再复制一份到临时并使全帧深移 4（-0x34 vs ORIG -0x30）。
//   p167 27/27 CLEAR。
#include "ExpertExtraction/extraction.inc"
}

// ============================================================================
// CEnchanterExtraction::get_extraction_count —— ORIG T 0x084a3b34：grade / 10。
// ============================================================================
#include "ExpertExtraction/get_extraction_count_1.inc"
}

// ============================================================================
// CEnchanterExtraction::_check_error —— ORIG T 0x084a3b92（+0x338 等级字段）
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：同 CAlchemistExtraction::_check_error
//   （具名 level 局部 + `need > limit` 方向；字段 m_int338 / 脚本类型 1）。p167 13/13 CLEAR。
#include "ExpertExtraction/_check_error_1.inc"
}

// ============================================================================
// CEnchanterExtraction::check_error —— ORIG T 0x084a3cb6
// b28d-54 判词：同 CAlchemistExtraction::check_error（ORIG 尾部槽往返 ⇒ 具名局部）。
// ============================================================================
#include "ExpertExtraction/check_error_2.inc"
}

// ============================================================================
// CEnchanterExtraction::extraction —— ORIG T 0x084a37c6
//   无 result_type 分支（Enchanter 单结果表）。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：同 AlchemistExtraction::extraction 三旋钮
//   （pick 两句 + cMyTrace 全表达式链 + 内联 count；单结果表无 result_type 分支）。
//   p167 25/25 CLEAR。
#include "ExpertExtraction/extraction_1.inc"
}

// ============================================================================
// CDollControllerExtraction::get_doll_controller_extraction_type —— ORIG T 0x084a3d66
//   usableLevel（0 时 grade）区间：1..0x1e→0；0x1f..0x32→1；0x33..0x50→2；其它→3
// ============================================================================
#include "ExpertExtraction/get_doll_controller_extraction_type.inc"
}

// ============================================================================
// CDollControllerExtraction::get_extraction_count —— ORIG T 0x084a41a8
//   按类型：0→grade/10；1→(grade-0x14)/10；2→(grade-0x28)/10；0 → 1。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：switch case 改数值升序 0/1/2 ——
//   ORIG 叶块物理序 = case 书写序（P46）：case0 块直落决策树之后、case1/case2 各跳；
//   原书写 1/2/0 使 case0 块排到末尾（多 1 jmp + 反向 je）。p167 6/6 CLEAR。
#include "ExpertExtraction/get_extraction_count_2.inc"
}

// ============================================================================
// CDollControllerExtraction::_check_error —— ORIG T 0x084a4286（+0x33c 等级字段）
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：同 CAlchemistExtraction::_check_error
//   （具名 level 局部 + `need > limit` 方向；字段 m_int33c / 脚本类型 4）。p167 13/13 CLEAR。
#include "ExpertExtraction/_check_error_2.inc"
}

// ============================================================================
// CDollControllerExtraction::check_error —— ORIG T 0x084a43aa
// b28d-54 判词：同 CAlchemistExtraction::check_error（ORIG 尾部槽往返 ⇒ 具名局部）。
// ============================================================================
#include "ExpertExtraction/check_error_3.inc"
}

// ============================================================================
// CDollControllerExtraction::extraction —— ORIG T 0x084a3dce
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：同 AlchemistExtraction::extraction 三旋钮
//   （pick 两句 + cMyTrace 全表达式链 + 内联 count；字段 m_field8、
//   stDollControllerResult 表）。p167 27/27 CLEAR。
#include "ExpertExtraction/extraction_2.inc"
}

}  // namespace expert_extraction
