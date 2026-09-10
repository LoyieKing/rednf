// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W8LT_PassiveObjectScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W8LT_PassiveObjectScript.cpp — PassiveObjGenerator* / RandomItemFairy /
// STPassiveObjectScript 构造与 clear 实现
//
// 来源（df_game_r，elf32-i386）：
//   PassiveObjGeneratorObject::PassiveObjGeneratorObject()   T 0x08a3ae56
//     （docs/class_func_reports/PassiveObjGeneratorObject/PassiveObjGeneratorObject.md）
//   PassiveObjGeneratorPattern::PassiveObjGeneratorPattern() T 0x08a3ad70
//     （docs/class_func_reports/PassiveObjGeneratorPattern/PassiveObjGeneratorPattern.md）
//   PassiveObjGeneratorSequence::PassiveObjGeneratorSequence() T 0x08a3ae0a
//     （docs/class_func_reports/PassiveObjGeneratorSequence/PassiveObjGeneratorSequence.md）
//   RandomItemFairy::RandomItemFairy/clear                W 0x08a3db5e/0x08a3db72
//   STPassiveObjectScript::STPassiveObjectScript()        T 0x08a3a538
//     （docs/class_func_reports/STPassiveObjectScript/STPassiveObjectScript.md）
//   STPassiveObjectScript::clear()                        T 0x08a3a9e6
//     （docs/class_func_reports/STPassiveObjectScript/clear.md）
// 拷贝构造/operator=/析构为 ORIG 弱符号（W），由编译器隐式生成即与 ORIG
// 形态一致（声明序逐字段，无 pad 空洞），故本 TU 不定义。
// ============================================================================
#include "W8LT_PassiveObjectScript.h"
#include "STMapScript.h"      // ENUM_DRAWLAYER/MAP_FOLLOW_TYPE/PASSAGETYPE + 名字函数声明
#include "GameEnums.h"       // ENUM_CATEGORY
std::string getObjectCategoryString(ENUM_CATEGORY cat);   // T 0x08a3ca81（ScriptDepsA.cpp）
#include "STPassiveObjectAdditionalParticleInfo.h"
#include "RDARScriptStringManager.h"  // getTagByName（[name] 段）
#include "SyncScriptDeps.h"           // toTString
#include <stdio.h>

// ---- TU-local 视图：ORIG export 0x08a2b44b 以「对象首地址 + int 索引 (k+4)」
//      访问 +0x10..+0x18 三 int（P30 两步寻址实证：`add $0x4,%edx;
//      mov (%eax,%edx,4),%eax`）。仅复现该寻址形，不改变布局/不读写前四项。
struct STPassiveObjectAdditionalParticleInfoIntView
{
    int m_ints[7];   // +0x00..+0x18（[4]/[5]/[6] 即 m_i10/m_i14/m_i18）
};

// ---- 跨 TU 依赖声明 ----
extern char g_szPassiveObjectScriptBaseDirectory[0x100];   // InitScriptProducers_4.cpp（B 0x094f6ac0）
#include "W8LT_PassiveObjectScript/getScriptFileFullPath.inc"
#include "W8LT_PassiveObjectScript/convertBinaryToText.inc"
extern RDARScriptStringManager g_scriptStringManager_;            // RDARScriptStringManager.cpp
// ---- WarningDistance（ORIG C1/C1Eiii T 0x08a3aeca/0x08a3aeec）----
// C1：三字段 movl $0 直写，函数体逐句赋值（每字段重读 this 0x8(%ebp)）；
// C1Eiii：参数 edx 中转 store，同函数体赋值形态。
#include "W8LT_PassiveObjectScript/WarningDistance.inc"
}

#include "W8LT_PassiveObjectScript/WarningDistance_1.inc"
}

// ORIG 0x08a3ae56：+0=0（int 直写）、+4=0.0f（float 经 edx 中转）、+8 string("")
#include "W8LT_PassiveObjectScript/PassiveObjGeneratorObject.inc"
}

// ORIG 0x08a3ad70：初始化列表 m_b00=1 → m_s04 string → m_i08=0 → m_i0c=0 →
// m_v10 vector<uchar>，函数体 m_s04.clear() 再 m_v10.clear()（含 unwind 清理路径）。
#include "W8LT_PassiveObjectScript/PassiveObjGeneratorPattern.inc"
}

// ORIG 0x08a3ae0a：初始化列表 m_s00 string → m_i04=0，函数体 m_s00.clear()。
#include "W8LT_PassiveObjectScript/PassiveObjGeneratorSequence.inc"
}

// ---- SpeedCtrlPassiveObjInfo（ORIG T 0x08a3ad5a）----
// +0=-1（movl 直写）、+4 byte=0（movb）；两次重读 this → 函数体逐句赋值。
// D1/D2 为 ORIG W 0x08a4eab2 trivial，由编译器隐式生成，本 TU 不定义。
#include "W8LT_PassiveObjectScript/SpeedCtrlPassiveObjInfo.inc"
}

// ---- PassiveIObjSniperInfo（ORIG C1/clear T 0x08a3af0c/0x08a3afc0）----
// C1：4 × vector<int> 构造 → clear()（带 unwind 清理路径）；
// clear：4 × vector::clear → 标量 0x30/0x34/0x38/0x40/0x3c 清零 →
// 循环 i=0..5 清 +0x44..+0x58 → 循环 i=0..2 清 +0x74..+0x7c →
// 0x5c/0x60/0x64/0x6c/0x70 清零（+0x68 不重置）。
#include "W8LT_PassiveObjectScript/PassiveIObjSniperInfo.inc"
}

#include "W8LT_PassiveObjectScript/clear.inc"
}

// ---- STPassiveObjectDestroy（ORIG C1 W 0x08a3d926 / D1 W 0x08a3e04c）----
// C1：5 × vector 构造（+0x08/+0x14/+0x20/+0x2c/+0x44）→ clear()（带 unwind 清理路径）；
// D1：空体，逆序析构 5 × vector（带 unwind 清理路径）。
#include "W8LT_PassiveObjectScript/STPassiveObjectDestroy.inc"
}

#include "W8LT_PassiveObjectScript/dtor_STPassiveObjectDestroy.inc"
}

// ORIG W 0x08a3d8c2：+0/-1、+4/0、+0x38/+0x3c/+0x40/-1，再 clear +8/+0x2c int vec 与 +0x14 string vec。
#include "W8LT_PassiveObjectScript/clear_1.inc"
}


// ---- STPassiveObjectAddEffect（ORIG C1 W 0x08a3da0c / D1 W 0x08a3e110）----
// C1：2 × vector 构造（+0x00 string/+0x0c int）→ clear()（带 unwind 清理路径）；
// D1：空体，逆序析构 2 × vector（带 unwind 清理路径）。
#include "W8LT_PassiveObjectScript/STPassiveObjectAddEffect.inc"
}

#include "W8LT_PassiveObjectScript/dtor_STPassiveObjectAddEffect.inc"
}

#include "W8LT_PassiveObjectScript/clear_2.inc"
}
// ---- RandomItemFairy（ORIG C1/C2 W 0x8a3db5e / clear W 0x8a3db72）----
// C1：仅调 clear()（无成员构造，纯标量）；clear：+0 movl $0 直写 →
// 循环 i=0..2（setle）清 +4+i*4。D1/D2 W 0x8a4f436 空体（头内联）。
#include "W8LT_PassiveObjectScript/RandomItemFairy.inc"
}

#include "W8LT_PassiveObjectScript/clear_3.inc"
}

// ---- STPassiveObjectScript（ORIG C1 T 0x08a3a538 / clear T 0x08a3a9e6）----
// C1：按声明序构造全部非平凡成员（string/vector/子对象，call 序实证）→ clear()；
// 标量由 clear 统一赋值。D1/D2 W 0x08a3e25a 为编译器隐式生成（逆序析构成员），
// 本 TU 不定义（P12）。
#include "W8LT_PassiveObjectScript/STPassiveObjectScript.inc"
}

// clear：语句顺序严格照 ORIG 汇编（含 +0x61 先于 +0x60、+0x15c 先于 +0x158、
// +0x140 vector clear 先于 +0x9c、float 字段 edx 中转形态）。
#include "W8LT_PassiveObjectScript/clear_4.inc"
}
// ===========================================================================
// exportPassiveObjectScript —— ORIG T 0x08a29f69
// （docs/class_func_reports/_lt_global_gt_/exportPassiveObjectScript.md；
// 偏移经 objdump 复核，与 W8LT_PassiveObjectScript.h 成员一一对应）
// 导出 STPassiveObjectScript 为中间文本（fopen "wb" + fprintf/fwrite），
// 收尾 convertBinaryToText 转最终脚本；fopen 失败返 0，成功返 1。
// ===========================================================================
// [b28d-51 判词；SweepW8LTDBTask2 2026-09-10 复核] ORIG 0x08a29f69（2254 条）
// vs 我方 2256 条。§6.2 三件套：p167 调用点差集 CLEAR（310=310，仅 C1/C2、
// D1/D2 同址别名）；jcc 序列全 TU 260 函数逐位核对仅本件与
// _Vector_base<uchar>::D1 不同；立即数多重集仅后者差帧 0x10/0x18。
// 本轮修复（真 bug + P0 真缺口，均已 fresh 复编验证）：
//  ① (int)(short)m_us150 —— ORIG 0x08a2b9b9 `movzwl + cwtl` 符号扩展；原写
//     (int) 零扩展，值 ≥0x8000 时输出不同（P321-T1/T2）。
//  ② follow 循环删指针缓存 —— ORIG 0x08a2b446 每轮重取 operator[]，并以对象
//     首地址 + int 索引 (k+4) 访问 +0x10..+0x18（P30 两步寻址/P183）；k 为
//     unsigned（ORIG 0x08a2b471 setbe）。
//  ③ category 循环补具名局部 int cat —— ORIG 0x08a2a2be 有槽往返（P0）。
//  ④ [name] 的 m_name.c_str() 内联实参 —— ORIG 0x08a2a02c 无槽（P0）。
//  ⑤ passName/followName 合并为函数级 nameString=0 —— ORIG 0x08a29f74 开头
//     清零、0x08a2a1df/0x08a2bbaf 复用同一槽 -0x54（P0）。
//  ⑥ soundVol/effectCnt 删具名局部 —— ORIG 0x08a2b0e5/0x08a2b1c9 用 %ebx 跨
//     调用保活（P0/P329）。
//  ⑦ m_b1c 字段 bool 化 —— ORIG 0x08a2b4a9 直测无 setne（P73/P321-T3；
//     STPassiveObjectAdditionalParticleInfo.h §8.3，宽度 1B/sizeof 不变）。
//  ⑧ speedName 分支极性 —— ORIG 0x08a2bcbd `test;je <[down]>`，[up] then 直落
//     （P222）。
//  ⑨ [string data] 照抄 *it->c_str() —— ORIG 0x08a2a9e8 movzbl+movsbl 取首字符
//     当 %s（P171 原码遗留行为照抄）。
//  ⑩ needDestroy 删具名 bool —— ORIG 0x08a2abd9 仅 %al 物化后直测（P0）。
//  ⑪ m_ib0 改浮点比较 —— ORIG 0x08a2b696 fildl（P171 照抄显式转换）。
//  ⑫ m_b60 三目 (int)(b60?1:0) —— ORIG 0x08a2a65f je/mov$1/jmp/mov$0（P47）。
//  ⑬ [sound category] 合并短路语句表达式 —— ORIG 0x08a2afea 物化 mov$1/mov$0 后
//     test（P214①/P316）。
// 剩余 N 族（逐块比对，语义等价，无源码解）：
//  a. quest type if 链块发射序（0x8a2aa8c/0x8a2aa97/0x8a2aae1/0x8a2aaf0，N93附/P46）；
//  b. m_i40 0/1 链块序（0x8a2ad12/0x8a2ad40，同族）；
//  c. homing follow 2/3/1/else 链块序（0x8a2b59d/0x8a2b618/0x8a2b633，同族）；
//  d. x87 比较 fucompp/fnstsw/sahf vs fucomip/fstp ×7（0x8a2b643/0x8a2b659/
//     0x8a2b6aa/0x8a2b711/0x8a2b747/0x8a2b77d/0x8a2bb43，N85）；
//  e. category 读槽与 lea 的求值序（0x8a2a2c1/0x8a2a2c7，P75/N-G）。
// 同 TU 另件 _ZNSt12_Vector_baseIhSaIhEED1Ev（ORIG 39 条 vs 我方 24 条）：ORIG
// libstdc++ 版含 _M_deallocate 的 EH landing + push esi/ebx，4.4 头无 ⇒ N89。
#include "W8LT_PassiveObjectScript/exportPassiveObjectScript.inc"
}
