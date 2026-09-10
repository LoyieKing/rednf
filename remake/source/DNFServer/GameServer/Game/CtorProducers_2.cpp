// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CtorProducers_2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CtorProducers_2.cpp — Ctor_Group2 批次：构造/装载 producer 真实实现
//
// 本 TU 提供以下 ORIG 符号（全部经 docs/class_func_reports 报告 + objdump 转写）：
//   DisjointScript         C1 0x08375d4c / clear 0x08375bca / D1 0x0837dff8
//   DisJoint               C1 0x0837e532 / LoadDisJoint T 0x0847300c
//   StrikerScript          getInsance/ctor/dtor/clear/create/destory（instance_
//                          B 0x09500978）/ importScript 系列声明见头，定义待续批
//   CharacManageScript     instance_ B 0x094f6b98 / getInsance/ctor/dtor/clear/importScript
//   SDCActiveStatusSyncCommon ctor W 0x08234870 / dtor W 0x08234876（空体）
//   SDCActiveStatusSyncServer ctor W 0x0823487c / dtor W 0x08234890 /
//                          loadActiveStatusScript T 0x0808e13c
//
// 不在本 TU 提供（避免链接重复，归属既有注册 TU）：
//   LotteryInfo C1/clear/D1 → CStackableItem.cpp 已定义（ORIG 0x083706d4/069c/4cfc）。
//
// 声明先行（定义待对应域批次，见交付报告）：
//   importDisjointScript / importCompoundAvatarTable / importActiveStatusTable /
//   ContentsType::getType / importSkillDataUp
#include "DisJoint.h"
#include "StrikerScript.h"
#include "NpcScript.h"          // STScriptFileList 完整定义（create 用）
#include "CharacManageScript.h"
#include "CItemUpgrade_Separate.h"
#include "STStackableScript.h"        // ENUM_MODULE_TYPE（DataManagerInitDepends 前置要求）
#include "DataManagerInitDepends.h"   // SDCActiveStatusSyncServer / WongWork::CCompoundAvatar
#include "PvPTypes.h"                 // CMTRand
#include "DNFLexWrapper.h"            // ScanType/ScanInt/ScanFloat/ScanStr/loadRDARScriptFile

#include <cstdio>
#include <ctime>
#include <new>
#include <string>
#include <vector>

// ============================ DisjointScript ============================


#include "CtorProducers_2/LoadDisJoint.inc"
}

// ---- DisjointSkillScript（ORIG W 0x084735fe/0x084735d2；布局 4×int）----
#include "CtorProducers_2/DisjointSkillScript.inc"
}

#include "CtorProducers_2/clear.inc"
}

// ---- DisjointScript::._381（命名类 t_381；成员 +0x00 int m_count / +0x04 vector<int> m_items）----
// ORIG ctor _ZN14DisjointScript5._381C1Ev t 0x08348088 (0x16)：仅构造 +4 vector<int>；
// dtor _ZN14DisjointScript5._381D1Ev t 0x0834809e：逆序析构 vector。
// GCC 对 ctor/dtor 的 asm 别名无效（硬限制），故在命名类自然名处重建原 ctor/dtor，
// 末尾 `.globl`+`.set` 汇编别名强制 ORIG 带点号符号（mangling 必要性，非 §4.5 违例）。
// 命名类取 5 字符 `t_381`（自然 mangled `5t_381*`）+ 别名 global 化：objdump 对同址
// 符号按 (1) binding 强 (2) strcmp 小 选标注名，`5._381*` 第 20 字节 '.' < 't' 胜出，
// 于是 DisjointScript C1/D1 内 4 处 call 标注与 compare 的 nm 切片都命中 ORIG 点号名。
#include "CtorProducers_2/t_381.inc"
}

#include "CtorProducers_2/dtor_t_381.inc"
}

#include "CtorProducers_2/DisjointScript.inc"
}

#include "CtorProducers_2/clear_1.inc"
}

#include "CtorProducers_2/dtor_DisjointScript.inc"
}

// ============================ DisJoint ============================

#include "CtorProducers_2/DisJoint.inc"
}

#include "CtorProducers_2/dtor_DisJoint.inc"
}

// ============================ StrikerScript ============================


// ORIG 静态成员单例指针（nm B 数据；getInsance 懒加载落点）。
StrikerScript* StrikerScript::instance_ = 0;              // _ZN13StrikerScript9instance_E B 0x09500978
CharacManageScript* CharacManageScript::instance_ = 0;    // _ZN18CharacManageScript9instance_E B 0x094f6b98


#include "CtorProducers_2/getInsance.inc"
}

#include "CtorProducers_2/clear_2.inc"
}

#include "CtorProducers_2/create.inc"
}

#include "CtorProducers_2/destory.inc"
}



#include "CtorProducers_2/StrikerScript.inc"
}

#include "CtorProducers_2/dtor_StrikerScript.inc"
}

// ============================ CharacManageScript ============================

#include "CtorProducers_2/getInsance_1.inc"
}

#include "CtorProducers_2/CharacManageScript.inc"
}

#include "CtorProducers_2/clear_3.inc"
}

#include "CtorProducers_2/dtor_CharacManageScript.inc"
}

#include "CtorProducers_2/importScript.inc"
}

// ============================ SDCActiveStatusSync Common/Server ============================

#include "CtorProducers_2/SDCActiveStatusSyncCommon.inc"
}

#include "CtorProducers_2/dtor_SDCActiveStatusSyncCommon.inc"
}

#include "CtorProducers_2/loadActiveStatusScript.inc"
}

// ============================ DisjointScript::._381 别名 ============================
// GCC 对 ctor/dtor 的 asm 别名（asm("...")）无效（硬限制）；对匿名嵌套 struct 又按
// 本 TU/行号计数命名（如 4._44），与 ORIG `5._381` 不一致。故在 DisjointScript 类内
// 命名 `t_381`（5 字符），用 `.globl`+`.set` 把自然 mangled `5t_381*` 强制改名为 ORIG
// 带点号 `5._381*`。
// 必须 `.globl`+`.type @function`+`.size`：objdump 打印段头/解析 call 目标时，同址多
// 符号先比 binding（global > local，探针 /tmp/bl9p/al2.so：local 别名输给自然名），
// 同 binding 再取 strcmp 最小名（探针 al4.so/al5.so：`2Aaa` 胜 `3run`，`4Zzz` 负）。
// 5 字符类名使自然名 `5t_381*` 与别名 `5._381*` 前 19 字节相同，第 20 字节 '.'(0x2e)
// < 't'(0x74) ⇒ 别名为标注名（= ORIG nm 行一致，compare 亦能按点号名切出函数体）。
// cf. docs/class_func_reports/DisjointScript__._381/（ORIG 0x08348088/0x0834809e）。
asm(".globl _ZN14DisjointScript5._381C1Ev\n"
    ".set _ZN14DisjointScript5._381C1Ev, _ZN14DisjointScript5t_381C1Ev\n"
    ".type _ZN14DisjointScript5._381C1Ev, @function\n"
    ".size _ZN14DisjointScript5._381C1Ev, 22");
asm(".globl _ZN14DisjointScript5._381C2Ev\n"
    ".set _ZN14DisjointScript5._381C2Ev, _ZN14DisjointScript5t_381C2Ev\n"
    ".type _ZN14DisjointScript5._381C2Ev, @function\n"
    ".size _ZN14DisjointScript5._381C2Ev, 22");
asm(".globl _ZN14DisjointScript5._381D1Ev\n"
    ".set _ZN14DisjointScript5._381D1Ev, _ZN14DisjointScript5t_381D1Ev\n"
    ".type _ZN14DisjointScript5._381D1Ev, @function\n"
    ".size _ZN14DisjointScript5._381D1Ev, 22");
asm(".globl _ZN14DisjointScript5._381D2Ev\n"
    ".set _ZN14DisjointScript5._381D2Ev, _ZN14DisjointScript5t_381D2Ev\n"
    ".type _ZN14DisjointScript5._381D2Ev, @function\n"
    ".size _ZN14DisjointScript5._381D2Ev, 22");
