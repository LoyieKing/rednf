// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 SkillShopGuideTextData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— SkillShopGuideTextData 域（独占 TU）
//
// 报告：docs/class_func_reports/_lt_global_gt_/importSkillShopGuideText.md（已完整阅读
// 汇编 + 反编译 C 两段）。
// 类方法（W 符号）布局证据：objdump 0x0891cb4c/0x08932804/0x089328e6；
//   分配尺寸 0x10（new_allocator::construct movl $0x10），C1/D1 仅 string 成员，
//   拷贝 ctor/op= 逐字段 int,int,string,byte,byte,byte。
//   [判据㉓ 本轮] 四件套定义已移入 SkillShopGuideTextData.h 类内（ORIG nm 全 W：
//   C1/C2 0x0891cb4c、D1/D2 0x0891cb62、C1ERKS_/C2ERKS_ 0x08932804、aSERKS_ 0x089328e6；
//   我方原 .cpp 外联定义发射 T）⇒ nm 绑定 342 共有符号 bind-diff=0；C1ERKS_ 逐条全等
//   （33=32+对齐 nop）；sizeof/偏移/mangled 零变化；四数不回退。
//   [P327/P338 本轮] 断 DNFLexWrapper.h 链：其 DNFLex.h:16 `#include <iostream>` 使本 TU
//   发射 std::ios_base::Init __ioinit + `_GLOBAL__I_<本 TU 首强符号>`（ORIG 全 509 个 GI
//   无 SkillShop 键名 ⇒ 仅 NEW 符号）⇒ 改局部权威声明四个 DNFLex 函数（mangled 与 ORIG
//   实证一致）。nm 确认 GI/ioinit/static_init 全消，四数 diff 3→2、missing_in_game 62→61。
//   [Main 排查 2026-09-10] 脚本循环 `goto/break` 目标逐点核：goto loop → ORIG jmp
//   08914553（guard 段回边）一致；ScanType 失败 break → ORIG jne 08914d40（函数出口）一致；
//   Tree 内层 break → ORIG jmp 089143e5（外层 continue）一致。全 TU jmp/jcc 目标方向
//   （F/B）逐位核对 239 符号仅 2 处差异（P164 两跳/单跳、岛中转 vs 直跳），无回边→出口反转。
// importSkillShopGuideText：ORIG T 0x08914483，无调用者（全量 objdump 无 call 命中），
//   语义：装载 SkillShopGuideText.etc，段键经 static map 查找（首次 SpinLock 保护
//   注册键值），"[character job]"+growType 为键、"[text]/[color]/[level range]"
//   填充数据向量后 insert 进 out map。
// ============================================================================

#include "SkillShopGuideTextData.h"
#include "CEnvironment.h"    // class SpinLock / less_TCHAR
#include "LogManager.h"
#include "Quest.h"           // enum ENUM_CHARACTERJOB
#include "SyncScriptDeps.h"  // int getCharacterJob(const char*)

#include <map>
#include <vector>
#include <utility>

// DNFLexWrapper.h 在本 TU 只提供 loadRDARScriptFile/ScanType/ScanInt/ScanStr 四个
// 函数声明，但其 include 链 DNFLex.h:16 `#include <iostream>` 会带出
// std::ios_base::Init __ioinit 与 `_GLOBAL__I_<本 TU 首强符号>`（ORIG 本 TU 无 GI，
// P327/P338；属「仅 NEW」符号）⇒ 改用局部权威声明断链。签名由 ORIG mangled 实证：
// _Z18loadRDARScriptFilePKcS0_ / _Z8ScanTypeRSsb / _Z7ScanStrPSs / _Z7ScanIntPb；
// 返回类型由调用点判据定（P292/P168a：`xor $1;test;je` ⇒ bool；ScanInt 单字节/32 位
// 两处使用 ⇒ int）。
#include "SkillShopGuideTextData/loadRDARScriptFile.inc"
#include "SkillShopGuideTextData/ScanType.inc"
#include "SkillShopGuideTextData/ScanStr.inc"
#include "SkillShopGuideTextData/ScanInt.inc"
#include "SkillShopGuideTextData/getGrowTypeIndex.inc"
#include "SkillShopGuideTextData/importSkillShopGuideText.inc"
}
// ============================================================================
// importSkillShopTree —— ORIG T 0x0891427f（范围 [0x0891427f, 0x08914482]）
// 报告 docs/class_func_reports/_lt_global_gt_/importSkillShopTree.md
// 返回 int：0=文件加载失败，1=成功。与 importSkillShopGuideText 同属
//   RDARScriptEtcParameter.cpp 日志域；装载 SkillShopTree.etc 后 clear out，
//   逐 token："[skill tree]" 段内循环 ScanStr 职业名 → getCharacterJob 转枚举、
//   ScanStr 树名，pair 构造后 insert 进 out map；ScanStr 失败跳出内层循环回到
//   外层 do；遇到 "[/skill tree]" 或 EOF 结束返回 1。
// ---------------------------------------------------------------------------
// b28d-solo-SkillShopGuideTextData 判词：语义完成（p167 CLEAR 24=24；帧 0x50、
//   槽位 token/-0x20、bool/-0x9、job/-0x24、treeStr/-0x28、jobStr/-0x2c、
//   pair/-0x14、insert 返回/-0x1c 与 ORIG 全等）。byte 层 N 封顶：内层 ScanStr
//   检查 ORIG 为「xor $1;test;je body;jmp outer」两跳形，4.4 恒产
//   「xor $1;test;jne outer」单跳 + 中继 nop 岛（P164 循环旋转族）；外层/析构段
//   另有 N7 %esi↔%ebx 角色差。源码形不可择，不再耗轮次。
// ============================================================================
#include "SkillShopGuideTextData/importSkillShopTree.inc"
}

// ============================================================================
// getSkillMaxLevelOnSteal —— ORIG T 0x08a9af4f（0x53 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillMaxLevelOnSteal.md
// 与 getSkillSpendSPOnSteal（SkillSlot.cpp）共用 .rodata 常量 0x8e2a180 = 100.0f：
//   f = penalty / 100.0f；r = (int)(sp * f)（fistpl 控制字改 0x0c → RC=11 截断
//   向零，与 C 浮点转整一致）；返回 std::max(1, r)。
// ============================================================================
#include "SkillShopGuideTextData/getSkillMaxLevelOnSteal.inc"
}

// ============================================================================
// getSkillNeedLevelOnSteal —— ORIG T 0x08a9aff5（0xe 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillNeedLevelOnSteal.md
// 汇编仅 lea (%edx,%eax,1) + ret：直接返回 param_1 + param_2。
// ============================================================================
#include "SkillShopGuideTextData/getSkillNeedLevelOnSteal.inc"
}
