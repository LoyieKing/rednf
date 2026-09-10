// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EquipmentParameterInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— EquipmentParameterInfo（G5 装备参数脚本）
// 布局依据 ORIG clear（0x089193c0）/ 析构（0x0891a4c6）；访问器对照
// 0x089c2e48..0x089c2e96（返回装备能力数组元素引用）。
// 解析函数（importEquipmentParameterInfo/readAction/readCondtion/
// scanRandomOptionValue/scanIf/scanThen/importAllSkillItem/
// importItemOverpowerDesc/exportEquipmentParameterScript）为真实语义的
// 简化实现（关键字行驱动，ORIG 0x0899cd6c..0x089bf67f）。
// ============================================================================
// [b28d-solo-EquipmentParameterInfo] 本 TU 判词表（verdict = game_func_report
//   --no-compile 新鲜 .o + compare_tu_game_orig；P167 = 调用点差集口径）：
//   getStatusType 0x089a74a2              ✅ AE（本波 P211 档①：本 TU 镜像声明
//     `unsigned int getChangeStatusType(...)` 触发 RHS-first 中转，ORIG 0x089a74dc
//     `mov %eax,%edx; mov this,%eax; mov %edx,(mem)` 逐条同形；返回类型不入 mangled、
//     ABI 同；本文件 `(float)` 消费点加 `(int)` 内层 cast 保持 fildl，零回归）
//   SASPROPERTY::operator= 0x083dbdfc     ✅ AE（本波 P246/P308：m_head[8] 拆 8 个独立
//     int 成员 ⇒ 隐式 op= 逐条重载双基址 = ORIG 形；探针 /tmp/b28d_epi/p10.cpp SC vs SA）
//   addString 0x089c2e98                  ✅ AE（本波：b 改 bool ⇒ 内层 `while(b)`
//     `cmpb $0,-0xd; jne` 直判；外层改 `if (b) {} else {…}` 复现 bool 的直判形
//     （bool 的 `!b` 恒产 xor，探针 p6/p7/p8 w19））
//   GetStatValue 0x089914b9               ✅ AE（具名 fval 先物化 + `if(!ok)` 负极性早退）
//   GetStatByCondition 0x08991384         ✅ AE（同）
//   SASPROPERTY::C1ERKS0_ 0x083cb038      ✅ AE（同 op=：独立 int 成员隐式 C2ERKS 逐条
//     重载双基址 + vector/string C1 + EH landing 逐条同形）
//   GetApplyStatusInRange 0x08991229      ✅ AE（单一 bool ok + 两次 ScanInt 共用 +
//     具名 f1/f2 紧跟 fildl/fstps + 负极性早退）
//   GetSkillLevelUp 0x08991085            ✅ AE（具名 f1/f2；`(float)(unsigned int)`
//     getCharacterJob 复现 fildll 零扩展；P223 goto 尾标签共享 return 1；本波返回
//     int→bool（P248 档①），调用点 `xor $1; test; je`，自身仍 123=123 全等）
//   scanThen 0x089a123c                   ✅ AE（补 ORIG 两个 dead local：
//     vector<vector<float>> + string；P223 goto 尾标签共享 return true）
//   importItemOverpowerDesc 0x089bf290    ✅ **IDENTICAL**（266=266，P167 CLEAR 35/35）：
//     P165/P177 贯穿建表链 + 双检 goto 重试 + case 0/0x3001 共享 [stat] insert 调用点
//     + 德摩根尾条件 `!(terminated && !empty())`；**真语义修正**：ORIG 是 terminated 且
//     m_428 **非空**才置 m_424/m_425=1（旧实现/旧判词判反）
//   importSkillDataUp 0x088c049c           ✅ **IDENTICAL**（393=393）：P165/P177 同模式
//     + 四处 `e.m_v8 = ScanInt(...); e.m_v8 = 0;` 死写照抄（P171）
//   GetSkillPhase 0x089be70c              ✅ **IDENTICAL**（317=317）：P165/P177 同模式
//     （`if(!ScanStr) return false;` 负极性 + `int phase` 值槽 + 声明序 `int phase; int id;`）
//   clear 0x089193c0                      ✅ **离开 DIFF（identical/AE）**：本波修复 =
//     5 组标量 union 视图 + 按 ORIG 循环结构改写（+0xb0/4、+0xc4/17、+0x230/3 float、
//     第二次 0x30 循环含 growthGrade/growthRepair）+ 函数级共用循环变量 i（双初始化
//     P191 形，四循环复用槽 -0x28）+ **真语义修正**：补漏清 m_allActiveStatusResistance
//     （ORIG 0x891960b，旧实现整段缺失）。506=506 逐条对齐、P167 CLEAR 20/20。
//   importAllSkillItem 0x089bec41         DIFF ndiff=2（P167 CLEAR 43/43）：P165/P177 结构
//     还原 + 四 callee bool 化（GetItemGrowtype/GetSkillApplyCondition/GetSkillPhase/
//     GetSkillLevelUp）⇒ 318→2；残差 = 4.4 外层 return 0 中继块落点（N121/P63 族）。
//     附真语义修正：s_first static→局部（ORIG 0x089becbd 局部标志）
//   GetSkillApplyCondition 0x089bd8ea     DIFF P167 CLEAR：ORIG 为多块贯穿建表链
//     （嵌套三表），我方为简化形 ⇒ 超大件 N82 口径（849/792，槽号/EH 布局）。
//     [本波] result/terminated 声明序与极性对齐 ORIG（0x89bd914 `movb $0,-0x2e` /
//     0x89bd918 `movb $1,-0x2d`；命中 0x2f31 时 0x89bdbe5/be9 写 result=1/terminated=0）
//     ⇒ gap 55→57（形态对齐、指令数 -2，N82 族；语义等价）。
//   exportEquipmentParameterScript 0x089acf78 DIFF：本波 p167 真缺口清零 ——
//     ①avatar job 串 `std::string job("")`（补 allocator C1Ev/C1EPKcRKSaIcE/D1Ev + EH，
//     ORIG 0x89ae343..0x89ae368）②判空改 `m_avatar264.size() != 0` 正极性
//     （then=avatar / else=roomList，ORIG 0x89ae286 `call size; test eax; setne`）⇒
//     gap 61→40；残余 = x87 fabs/fucompp 段（N85）+ 帧 0x180/0x170（N115/N95）。
//   readCondtion 0x0899ce3b               DIFF P167 **CLEAR**（calls 511=511，仅 C1/C2、
//     D1/D2 镜像）：本波未改动；gap 3653/3673（-20）= P63 内联失败块（我方 `je` 跳本地
//     块 vs ORIG `jne` 共享出口，每检查点 +2 条）+ switch 叶块物理序（N 族形态）。
//   importEquipmentParameterInfo 0x089a75ea DIFF：本波**p167 MISSING 清零**（补
//     LevelUpSkill lu 循环外构造+循环内 clear、push_backEOi 右值直传、0x19d6/0x19f4
//     拆成两个独立分支各一次 getActiveStatusToEnumValue）；残余 EXTRA string D1×6/
//     ScanInt×6/operator==×5 等 = switch 后死 if 链等历史结构差 ⇒ N（P165 结构还原未做）
//   readAction 0x089a13e5                 DIFF：本波 p167 **CLEAR**（calls 634=634，仅
//     C1/C2、D1/D2 镜像豁免）；本波修 3 处真语义缺口 —— ①[active status] 子表（ORIG
//     0x89a2af7）用**已读状态名 token** 查 __lookup_0 一次 + switch(sid) 分派；旧实现
//     `for(;;)+ScanTypeOrInt` 多读一个 token、且首建表回跳前误执行 body（多扫参数）⇒ 改
//     P165/P333 状态机（贯穿 insert 链 + case 0 哨兵 + 3 处 goto 跳过 body + 链尾回跳 +
//     float 0.0f 槽）②[skill]/[perform skill]/[end skill]/[skill super armor]/
//     [skill unbreakable] 具名 float 先物化、末尾连续 push_backERKf（ORIG 槽 -0x9dc../
//     -0xa40/-0xa44）③[swap stat] 删 body 局部串 s、改 ScanStr(&token)（ORIG 0x89a6b07）、
//     lhs/rhs 子表改状态机（ORIG 0x89a6d4e/0x89a6f20）。gap 5339/5320（+19）；残余 =
//     P63 内联失败块 vs ORIG 共享出口 + P232 float 初始化槽（N 族形态）。
//   共同归因：4.1/4.4 寄存器分配·栈槽复用·EH 布局差（N7/N18a/N76/N82/N90/N104 族）。
//   P167 CLEAR 件按 §6.2 判「语义完成、byte 层 N 封顶」。
//   P0/N96：本文件全域无 `register asm` 绑定（grep 零命中）；`asm(".set …")` 仅用于
//   P203 点号匿名类型符号别名（非绑定）。
//   [D47 委派件] EquipmentParameterInfo::operator= (W 隐式) 0x083662ca
//     ✅ **离开 DIFF（identical/AE）**：本波修复 = ①头内 7 组相邻标量收进匿名 union
//     数组视图（P246/P247：+0xb0×4、+0xc4×17、+0x120×2、+0x230×3、+0x288×4、
//     +0x3e0×2、+0x408×2；探针 /tmp/b28d_epi_probe/mix.cpp 实证 4.4.6 块拷贝形与 ORIG
//     全等）②`t_119::operator=` / `tt_159::operator=` 用 **asm 标签**直指 ORIG 点号
//     别名 `._159aSERKS0_` / `._119aSERKS0_`（推翻 P203「operator= 只能 .set」；
//     探针 /tmp/b28d_epi_probe/asmlabel.cpp 证明标签对 op= 有效且调用点重定位同步）
//     ③删 m_pad441[3]（P26）。716=716 逐条对齐、P167 CLEAR 19/19。
// ============================================================================
// ============================================================================

#include "EquipmentParameterInfo.h"

#include <stdio.h>
#include <string.h>
#include <math.h>   // [A18 本波] fabsf（ORIG export separate-rate 段 x87 fabs 内联形）
#include <limits>   // ORIG export @0x89adeab 尾区：numeric_limits<float>::epsilon() 调用两处
#include <algorithm>  // ORIG export 0x089ae30a：std::min<int>（_ZSt3minIiERKT_S2_S2_）

#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "ScriptValue.h"
#include "SyncScriptDeps.h"   // toTString / getCharacterJob
#include "CreatureScript.h"  // ENUM_ELEMENT（getElementName 形参）
#include "Quest.h"           // ENUM_CHARACTERJOB 的权威声明
#include "CEnvironment.h"    // less_TCHAR / SpinLock（GetSkillPhase static map）
#include "EffectAnimationInfo.h"  // readCondtion [attack success effect animation] 局部对象

// ORIG 全局记账（0x949b28c/0x949b294，C1/C2 递增、D1 递减）：
// 实例计数与按 sizeof 累积的分配字节数。
int g_EquipmentParameterInfoCount = 0;
int g_EquipmentParameterInfoSize = 0;

// ORIG C1（0x8919ce8）：成员默认构造后，先更新全局记账
// （count++、size = count * sizeof），再调 clear。
#include "EquipmentParameterInfo/EquipmentParameterInfo.inc"
}

// ORIG C2（0x891a0d4）：成员默认构造 + 函数体 `*this = other`（0x891a2a2 call op=）。
// ORIG C2（0x891a0d4）：成员默认构造 + 全局记账 + `*this = other`。
#include "EquipmentParameterInfo/EquipmentParameterInfo_1.inc"
}

// ORIG D1（0x891a4c6）：先递减全局记账，再析构成员（编译器生成）。
#include "EquipmentParameterInfo/dtor_EquipmentParameterInfo.inc"
}

// ---- EquipmentParameterInfo::._119（命名类 t_119；成员 +0x00 SASPROPERTY vector /
//      +0x0c/+0x0d char / +0x10 vector<vector<float>>）----
// ORIG _ZN22EquipmentParameterInfo5._119C1Ev t 0x088fd884（仅默认构造两 vector，
// char 不初始化）；~._119 t 0x088fd8ca（逆序析构）；aSERKS0_ t 0x088fd910（向
// vector<SASPROPERTY> op=、char、char、vector<vector<float>> op= 拷贝，无 self-check）。
// GCC 对 ctor/dtor 的 asm 别名无效（硬限制），故以命名类 `t_119`(5t_119) 重建，
// 末尾 `.set` 汇编别名强制 ORIG 带点号符号（mangling 必要性，非 §4.5 违例）。
#include "EquipmentParameterInfo/t_119.inc"
}

#include "EquipmentParameterInfo/dtor_t_119.inc"
}

EquipmentParameterInfo::t_119&
EquipmentParameterInfo::t_119::operator=(const t_119& other)
{
    // ORIG 0x088fd910：vector<SASPROPERTY> op= → +0xc char → +0xd char →
    // vector<vector<float>> op=；无 self-check，返回 *this。
    m_418 = other.m_418;
    m_424 = other.m_424;
    m_425 = other.m_425;
    m_428 = other.m_428;
    return *this;
}

// ORIG 另一 TU 的 `._159aSERKS0_`（0x08348024/0x0861c9b8）与本 TU `._119aSERKS0_`
// 函数体逐条相同（norm 实测相等）；独立类使函数体独立地址，objdump 方能同时
// 标注 `5._119aSERKS0_` 与 `5._159aSERKS0_`（P203 附注：同址只标一个）。
EquipmentParameterInfo::tt_159&
EquipmentParameterInfo::tt_159::operator=(const tt_159& other)
{
    m_418 = other.m_418;
    m_424 = other.m_424;
    m_425 = other.m_425;
    m_428 = other.m_428;
    return *this;
}


// ORIG 0x089193c0（0x928 字节）：标量清零 + 数组循环清零 + 容器 clear。
// 所有已解析的数组/标量均通过 EquipmentParameterInfo 具名字段访问；重复清零
// 保留 ORIG 的最终状态，字段类型与偏移以 EquipmentParameterInfo.h 为准。
// [b28d-solo-EquipmentParameterInfo] clear：✅ 离开 DIFF（identical/AE）——5 组标量 union
// 视图 + ORIG 循环结构（含 P191 双初始化共用循环变量 i）+ 补漏清
// m_allActiveStatusResistance（真语义修正）；506=506、P167 CLEAR 20/20。
#include "EquipmentParameterInfo/clear.inc"
}

// ORIG 0x089c2e98：遍历查找 str 已存在的 key（无则 size()+1），m[key]=str 后返回 key。
// 残差（3 条）：ORIG 4.1 对内层 if(b) 做块旋转（deref 后 jmp 岛 + b 复测回跳 L3），
// 4.4 恒将 b 守卫前置于比较块，源形不可翻（同 N 族块排布）。
// [b28d-solo-EquipmentParameterInfo] addString DIFF N：1 字节局部条件物化跨版本差（三形实测）。
#include "EquipmentParameterInfo/addString.inc"
}

#include "EquipmentParameterInfo/getEquipmentPhysicalAttack.inc"
}

#include "EquipmentParameterInfo/getEquipmentPhysicalDefense.inc"
}

#include "EquipmentParameterInfo/getEquipmentMagicalAttack.inc"
}

#include "EquipmentParameterInfo/getEquipmentMagicalDefense.inc"
}

#include "EquipmentParameterInfo/RebirthAppendageinfo.inc"
}

#include "EquipmentParameterInfo/clear_1.inc"
}





// ---- ORIG 全局枚举转换 helper（真实符号，定义在各自 TU；此处仅声明） ----
#include "EquipmentParameterInfo/getCharacterJobToEnumValue.inc"
#include "EquipmentParameterInfo/getActiveStatusToEnumValue.inc"
#include "EquipmentParameterInfo/getStateToEnumValue.inc"
#include "EquipmentParameterInfo/getEquipmentEventTiming.inc"
#include "EquipmentParameterInfo/getElementToEnumValue.inc"
#include "EquipmentParameterInfo/getAttackConditionToEnumValue.inc"
#include "EquipmentParameterInfo/getCategoryToEnumValue.inc"
#include "EquipmentParameterInfo/getGradeToEnumValue.inc"
#include "EquipmentParameterInfo/getObjectTypeToEnumValue.inc"
#include "EquipmentParameterInfo/getTargetToEnumValue.inc"
#include "EquipmentParameterInfo/scanModuleType.inc"
#include "EquipmentParameterInfo/ScanTypeOrStr.inc"
#include "EquipmentParameterInfo/getElement.inc"
#include "EquipmentParameterInfo/getActiveStatusType.inc"
#include "EquipmentParameterInfo/GetEquipmentTypeStringToEnum.inc"

static bool getTargetTag(int* out)   // [b28d-Main·P292] int→bool（ORIG 调用点 call;xor $0x1;test %al）
{
    // [b28c-lane24] ORIG 0x089a7413：多 return + 共享 ~s 尾块 ⇒ 4.4 用 %ebx 保存
    // 返回值（探针 /tmp/b28c_l24_probe2.cpp::pA 逐条同形）；正极性 ScanStr（无 xor）
    // + `if (v == 10) return 0;` 复现 jne 成功块 / je 失败岛布局。
    if (out == 0) return false;
    std::string s;
    if (ScanStr(&s))
    {
        int v = getTargetToEnumValue(s);
        if (v == 10) return false;
        *out = v;
        return true;
    }
    return false;
}

// [b28d-solo-EquipmentParameterInfo] DIFF N76：store RHS 值先不可翻（P211 三档旋钮试尽）。
static bool getStatusType(int* out)   // [b28d-Main·P292] int→bool
{
    // [b28c-lane24] ORIG 0x089a74a2：`if (*out != 0x4d) return 1;` ⇒ je 失败块 +
    // 成功块 fall-through（与 getTargetTag 同族）；*out 由调用表达式直存（无 v 槽，P0）。
    if (out == 0) return false;
    std::string s;
    if (ScanStr(&s))
    {
        // [b28d-solo-EquipmentParameterInfo] N76 封顶：ORIG 为 RHS 值先 +
        // `mov %eax,%edx` 中转，4.4 恒 LHS 地址先。已试尽 P211 三档旋钮：
        // ①调用侧 (unsigned int)/(ENUM) /双层 cast/`out[0]`/unsigned 左值视图
        //   （末者反多 `mov 0x8(%ebp),%ebx` 一条）；②callee 跨 TU 定义于
        //   GameScriptEnumHelpers.cpp（返回类型异型化需改他人 .cpp，超泳道）；
        //   ③目标为 int* 形参解引用，字段侧不可动 ⇒ 无源形可翻。
        *out = getChangeStatusType(s);
        if (*out != 0x4d)
            return true;
    }
    return false;
}

static bool getSign(int* out)   // [b28d-Main·P292] int→bool
{
    if (out == 0) return false;
    std::string s;
    if (!ScanStr(&s)) return false;
    if (s == "+") { *out = 0; return true; }
    if (s == "%") { *out = 1; return true; }
    return false;
}

// [b28d-solo-EquipmentParameterInfo] AE：具名 fval 先物化 + `if(!ok)` 负极性早退；P167 CLEAR。
#include "EquipmentParameterInfo/GetStatValue.inc"
}

#include "EquipmentParameterInfo/GetStat.inc"
}

// [b28d-solo-EquipmentParameterInfo] AE：同 GetStatValue；P167 CLEAR。
#include "EquipmentParameterInfo/GetStatByCondition.inc"
}

// [b28d-solo-EquipmentParameterInfo] AE：单一 bool ok + 具名 f1/f2；P167 CLEAR。
#include "EquipmentParameterInfo/GetApplyStatusInRange.inc"
}

// [b28d-solo-EquipmentParameterInfo] AE：具名 f1/f2 + (float)(unsigned int) fildll + P223 goto 尾标签。
#include "EquipmentParameterInfo/GetSkillLevelUp.inc"
}

#include "EquipmentParameterInfo/getBuffToEnumValue.inc"
}

#include "EquipmentParameterInfo/getSkillDataExpansionToEnumValue.inc"
}

#include "EquipmentParameterInfo/getEquipmentTypeToEnumValue.inc"
}

// ---- importSkillDataUp（ORIG 0x88c049c，readAction [skill data up] 依赖） ----
// scanScriptValue/pushScriptValue/ScriptValue 由 ScriptValue.h/ScriptValue.cpp
// （FixScriptValuePipeline）提供；importSkillDataUp 在本文件末尾定义。

// ============================================================================
// readAction —— ORIG 0x89a13e5（0x602e 字节）逐分支转写。
// param 映射：line=param_1（仅 [target] 失败日志用）、info=param_2（字段写入）、
// out=param_3（关键字，dispatch 依据）、values=param_4（float 输出栈）、idx=param_5。
// 每个关键字 push 一组 float 并消费对应 token（ScanInt/ScanStr/ScanFloat/
// ScanTypeOrInt/ScanTypeOrStr/ScanType）；任一扫描失败返回 false（0），
// 成功返回 true（1）。ORIG 的 __lookup 静态 map + 惰性初始化仅为编译期
// 优化，行为等价于按关键字字符串 if/else；此处省略惰性初始化直接分派。
// ============================================================================
// [b28d-solo-EquipmentParameterInfo] N：P165 循环抽象 vs ORIG 物理展开 + N82 全帧槽号；
// p167 MISSING ScanFloat×2 / push_backERKf×4（我方右值实参选 EOf，重载错位）+
// EXTRA _Unwind_Resume×2/string D1×2/ScanInt/ScanTypeOrInt/size 等。5275 vs 5339。
#include "EquipmentParameterInfo/readAction.inc"
}

// ORIG 0x0899ce3b（0x4401 字节）。真实结构：static map<const char*,int> 惰性
// 注册各关键字 → 分支 id，另以 __init_lock/__inited 双检锁保护首次注册；
// 注册完成后按分支 id 逐条 if 分派。注册/加锁仅为编译期优化，行为等价于
// 按关键字字符串 if/else-if 链（与本文件 readAction/importSkillDataUp 注释
// 约定一致），此处省略 map/锁直接分派。每个命中分支按 ORIG 反编译顺序
// Scan* 消费词法流并向 values push「tag 常量 + 参数」；失败的分支按其
// ORIG 语义处理（多数分支不检查扫描结果；[is stat]/[equipment upgrade]
// 任一步失败即直接 return，不再 push）。
// [b28d-solo-EquipmentParameterInfo] P167 仅 EXTRA _Unwind_Resume×2 ⇒ N104 EH 布局封顶。
#include "EquipmentParameterInfo/readCondtion.inc"
}

#include "EquipmentParameterInfo/scanRandomOptionValue.inc"
}

#include "EquipmentParameterInfo/scanIf.inc"
}


// [b28d-solo-EquipmentParameterInfo] AE：补 ORIG 两个 dead local + P223 goto 尾标签。
#include "EquipmentParameterInfo/scanThen.inc"
}

// ---- SASPROPERTY 子解析 helper（ORIG static，本 TU） ----
static bool GetItemGrowtype(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (sp == 0) return false;
    std::string s;
    if (!ScanStr(&s)) return false;
    sp->m_head0 = getCharacterJob(s.c_str());
    if (!ScanInt(&sp->m_head1)) return false;
    return true;
}

// [skill apply condition] 块直至 "[/skill apply condition]"。ORIG 用三张
// 函数局部静态 map<char const*,int,less_TCHAR>（主分派 __lookup、
// [skill group] 子表 __lookup_0、[extra condition] 子表 __lookup_1）以表查找
// 分派（报告 0x089bd8ea 实证：三组 __cxa_guard/SpinLock/__inited 双检 +
// 13 处 pair<char const(&)[N],int>::insert）。语义：
//   0x2f31 [/skill apply condition]→结束并返回 1；
//   0x2f35 [lower bound level]→ScanInt→m_head3（+0xc）；
//   0x2f39 [upper bound level]→ScanInt→m_head4（+0x10）；
//   0x2f3d [value]→ScanInt→m_head5（+0x14）；
//   0x2f41 [skill group]→ScanStr 查子表：0x2f49 passive→1 /
//     0x2f4c active→0 / 0x2f4f all→-1，写 m_head2（+0x8）；
//   0x2f59 [extra condition]→ScanStr 循环查子表：0x2f60 [normal skill]|1 /
//     0x2f63 [consume item skill]|2 / 0x2f66 [ex skill]|4 /
//     0x2f69 [except for ex skill]|3 位或入 flags，==0 返回 0，否则写
//     m_head6（+0x18）。
// 任一 ScanType/ScanInt/ScanStr 失败、或子表/主表未命中 → 返回 0。
// [b28d-solo-EquipmentParameterInfo] P167 CLEAR（本波补 2 个 SpinLock::leave 点）⇒ 超大件 N82 封顶。
static bool GetSkillApplyCondition(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (sp == 0) return 0;
    std::string token;
    // ORIG 0x89bd914/0x89bd918：先 `movb $0,-0x2e`（result）再 `movb $1,-0x2d`
    // （terminated）⇒ 声明序 result 在前、terminated 初值 1；命中 0x2f31 时
    // 0x89bdbe5/0x89bdbe9 写 result=1 / terminated=0。
    bool result = false;
    bool terminated = true;
    while (terminated && ScanType(token, true))
    {
        int id;
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                }
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[/skill apply condition]", 0x2f31));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[lower bound level]", 0x2f35));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[upper bound level]", 0x2f39));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[value]", 0x2f3d));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[skill group]", 0x2f41));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[extra condition]", 0x2f59));
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                }
            }
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        if (id == 0x2f39)
        {
            if (!ScanInt(&sp->m_head4)) return 0;
        }
        else if (id < 0x2f3a)
        {
            if (id == 0x2f31)
            {
                result = true;
                terminated = false;
            }
            else if (id == 0x2f35)
            {
                if (!ScanInt(&sp->m_head3)) return 0;
            }
            else
            {
                return 0;   // 主表未命中
            }
        }
        else
        {
            if (id == 0x2f3d)
            {
                if (!ScanInt(&sp->m_head5)) return 0;
            }
            else if (id == 0x2f41)
            {
                std::string str;
                if (!ScanStr(&str)) return 0;
                int gid;
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    if (!__inited)
                    {
                        // [b28d-solo-EquipmentParameterInfo] P167：ORIG 0x089bdfcb enter
                        // → 0x089bdfe2 `if (__inited) leave`（点1）→ inserts →
                        // 0x089be186 `if (!__inited) { leave; __inited = true; }`（点2）
                        // 与主表块同形（两个 leave 调用点）。
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                        }
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[passive]", 0x2f49));
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[active]", 0x2f4c));
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[all]", 0x2f4f));
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                        }
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(str.c_str());
                    gid = (it != __lookup.end()) ? it->second : -1;
                }
                int group;
                if (gid == 0x2f49)      group = 1;
                else if (gid == 0x2f4c) group = 0;
                else if (gid == 0x2f4f) group = -1;
                else                    return 0;   // 子表未命中
                sp->m_head2 = group;
            }
            else if (id == 0x2f59)
            {
                unsigned int flags = 0;
                std::string str;
                while (ScanStr(&str))
                {
                    int eid;
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (!__inited)
                        {
                            // 同块2：ORIG 0x089be41f enter → 0x089be436 leave（点1）→
                            // inserts → 0x089be60a `leave; __inited = true`（点2）。
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                            }
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[normal skill]", 0x2f60));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[consume item skill]", 0x2f63));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[ex skill]", 0x2f66));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[except for ex skill]", 0x2f69));
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                            }
                        }
                        std::map<const char*, int, less_TCHAR>::const_iterator it =
                            __lookup.find(str.c_str());
                        eid = (it != __lookup.end()) ? it->second : -1;
                    }
                    if (eid == 0x2f60)      flags |= 1;
                    else if (eid == 0x2f63) flags |= 2;
                    else if (eid == 0x2f66) flags |= 4;
                    else if (eid == 0x2f69) flags |= 3;
                    else                    return 0;   // 子表未命中
                }
                if (flags == 0) return 0;
                sp->m_head6 = flags;
            }
            else
            {
                return 0;   // 主表未命中
            }
        }
    }
    return result ? 1 : 0;
}

// GetSkillPhase —— ORIG 0x89be70c（static local）。消费 [skill phase] 块：
// 惰性初始化 static map（"[all]"→0x2f8c … "[fourth]"→0x2f9b），ScanStr 读
// token 查表，命中写 sp->m_head7 = phase（0..5）并返回 1，未命中返回 0。
// [b28d-solo-EquipmentParameterInfo] GetSkillPhase：✅ **IDENTICAL**（0x089be70c，
// 317=317）。P165/P177 结构还原（`if(!ScanStr) return false;` 负极性 + goto guard_retry
// 双检 + `int phase` 值槽 + 六 case 链式共享 insert 调用点，case 0 与 0x2f8c 共享）；
// 关键旋钮 = `int phase; int id;` 声明序（4.4 先声明占深槽 -0x20/-0x1c 与 ORIG 反，
// 交换后逐槽全等）。旧判词「删 continue 真 bug + 残差 N104」已由本波结构还原消解。
static bool GetSkillPhase(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (!sp) return false;
    std::string token;
    // [b28d-solo-EquipmentParameterInfo] P165/P177 结构还原（照 ORIG 0x089be70c 逐条
    // 转写）：`if (ScanStr(&token))` 正向包裹 + `for(;;)` 双检（find 在 `if(__inited)`
    // 分支内、双检命中 `continue` 回 guard 重查）+ `int phase` 值槽 + 六 case 链式
    // 共享 insert 调用点（case 0 与 0x2f8c 共享）+ switch 后统一
    // `if(!__inited){leave; __inited=true; goto guard_retry;}` 再 `sp->m_head7 = phase`。
    if (!ScanStr(&token)) return false;
    guard_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int phase;
        int id;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto guard_retry;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x2f8c:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[all]", 0x2f8c));
                goto L_2f8f;
            }
            phase = 5;
            goto after_switch;
    L_2f8f:
        case 0x2f8f:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[common]", 0x2f8f));
                goto L_2f92;
            }
            phase = 4;
            goto after_switch;
    L_2f92:
        case 0x2f92:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[first]", 0x2f92));
                goto L_2f95;
            }
            phase = 0;
            goto after_switch;
    L_2f95:
        case 0x2f95:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[second]", 0x2f95));
                goto L_2f98;
            }
            phase = 1;
            goto after_switch;
    L_2f98:
        case 0x2f98:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[third]", 0x2f98));
                goto L_2f9b;
            }
            phase = 2;
            goto after_switch;
    L_2f9b:
        case 0x2f9b:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[fourth]", 0x2f9b));
                goto L_default;
            }
            phase = 3;
            goto after_switch;
    L_default:
        default:
            if (__inited)
                return false;
            break;
        }
    after_switch:
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto guard_retry;
        }
        sp->m_head7 = phase;
        return true;
}

// ORIG：static map<const char*,int> 懒初始化——首次调用丢弃第一个 token（仅
// 触发初始化），其后未知 token 一律返回 0（default 分支）；[skill levelup] 的
// GetSkillLevelUp 直接填充 sp.m_vec20（+0x20），非局部 vector。
// [b28d-solo-EquipmentParameterInfo] importAllSkillItem：P165/P177 结构还原（贯穿建表
// 链 + 双检 goto 重试 + 五 case 链式共享 insert 调用点，P167 35/35 CLEAR）+ 四个
// callee bool 化（P248 档①：GetItemGrowtype/GetSkillApplyCondition/GetSkillPhase/
// GetSkillLevelUp 返回 int→bool，调用点 `call; xor $1; test; je` 逐条同形）⇒
// ndiff 318→2。残差 = 4.4 把外层 `return 0` 中继块落在成功块之后（ORIG 落函数末尾
// 与 && 短路失败共享）——块物理位置差（N121/P63 族），语义等价。
// 调用点 ORIG 0x89ac54e `call; xor $1,%eax; test %al,%al; je` ⇒ 返回 bool（P168a）。
static bool importAllSkillItem(EquipmentParameterInfo* info)
{
    EquipmentParameterInfo::SASPROPERTY sp;
    sp.m_head0 = 0xb; sp.m_head1 = 0; sp.m_head2 = 0xffffffff;
    sp.m_head3 = 0;   sp.m_head4 = 0; sp.m_head5 = 0;
    sp.m_head6 = 7;   sp.m_head7 = 5;
    std::string token;
    bool terminated = false;
    // [b28d-solo-EquipmentParameterInfo] 真语义修正：ORIG 0x089becbd
    // `movb $0x1,-0x1d(%ebp)` 是**局部**标志（每次调用都丢弃首 token），
    // 原 static 只在首次调用丢弃 ⇒ 语义偏差。
    bool s_first = true;
    int id;
    // [b28d-solo-EquipmentParameterInfo] P165/P177 结构还原（照 ORIG 0x089bec41 逐条
    // 转写）：while(s_first) 底部条件 + 循环体内 static 三件套（每轮穿 __cxa_guard）
    // + 双检 `if(__inited){leave; goto guard_retry}` + id=0 贯穿建表：case 0/0x2fc1
    // 共享块插入首项后 goto 到 case 0x2fc5 的 `if(!__inited)` 检查，各 case 的 insert
    // 块再链式 goto 到下一 case（P167 实证每项 insert 仅一个调用点）。
    while (s_first)
    {
        if (!ScanType(token, true))
            break;
    guard_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto guard_retry;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x2fc1:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[/all skill item]", 0x2fc1));
                goto L_2fc5;
            }
            terminated = true;
            s_first = false;
            goto after_switch;
    L_2fc5:
        case 0x2fc5:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[item growtype]", 0x2fc5));
                goto L_2fc9;
            }
            if (!GetItemGrowtype(&sp)) return 0;
            goto after_switch;
    L_2fc9:
        case 0x2fc9:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[skill apply condition]", 0x2fc9));
                goto L_2fcd;
            }
            if (!GetSkillApplyCondition(&sp)) return 0;
            goto after_switch;
    L_2fcd:
        case 0x2fcd:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[skill phase]", 0x2fcd));
                goto L_2fd1;
            }
            if (!GetSkillPhase(&sp)) return 0;
            goto after_switch;
    L_2fd1:
        case 0x2fd1:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[skill levelup]", 0x2fd1));
                goto L_default;
            }
            if (!GetSkillLevelUp(&sp.m_vec20)) return 0;
            goto after_switch;
    L_default:
        default:
            if (__inited)
                return 0;             // ORIG 0x89bf13f：未知 token（已初始化）→ 失败
            break;
        }
    after_switch:
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto guard_retry;
        }
    }
    // ORIG 0x089bf192：`if (!terminated) return 0;`（xor 物化 + 共享 return 0）。
    if (!terminated)
        return 0;
    if ((sp.m_head0 == 0xb && sp.m_head7 != 5) && sp.m_head7 != 4) return 0;
    // [b28d-solo-EquipmentParameterInfo] P222 两形互试定案：ORIG 0x89bf1f5 是三条失败
    // 路径（head3>0x46 / head4>0x46 / head3>head4）共享的 `mov $0,%ebx; jmp`，与函数末
    // return 0 同出口。正向包裹形（`if(h3<=h4){…}` 嵌套）4.4 反产 `jg`+独立 mov 块
    // （389 vs 391，更差）；早返形 391=391 仅余该共享块落点差 ⇒ N 封顶（N121/P63 族）。
    // [A18 本波 三形实测定稿] ①包裹形（本形）ndiff=**2**；②提前 return 形
    // （`if(h3>=0x47||h4>=0x47) return 0;`）劣化 **4**（ORIG 0x089bf1f5 两条 `ja`
    // 共享同一失败块，4.4 恒拆 `jbe`+独立块）；③C05① 多层嵌套形
    // （`if(a){ if(b){ … return 1; } } return 0;`）与包裹形**逐条等价、ndiff 仍 2**。
    // 残差 = 成功块后多发 1 个 `mov $0,%ebx; jmp` 外块（ORIG 该 return 0 落点与他块
    // 共享/后置，4.4 无法逐点控制）⇒ N121/P63 族封顶，gap 恒 +2。
    if ((unsigned)sp.m_head3 < 0x47 && (unsigned)sp.m_head4 < 0x47)
    {
        if (sp.m_head3 > sp.m_head4) return 0;
        if (sp.m_head5 < 1) return 0;
        info->m_119.m_418.push_back(sp);
        return 1;
    }
    return 0;
}

// ---- importItemOverpowerDesc（ORIG 0x089bf290，static local） ----
// 循环 ScanType 读 token；"[stat]"→局部 vector+GetStat（失败不 push）后 push
// 到 info->m_428；"[/item overpower desc]"→结束。ORIG 结尾语义：token 耗尽
// （未 terminated）返回 0；terminated 且 m_428 **非空**时置 m_424=m_425=1 返回 1；
// terminated 且 m_428 为空返回 0（0x089bf618 `je` 落 mov $0 块实证）。未知 token
// 返回 0（default）；首次调用以 id=0 贯穿建表后重查（P177）。
// [b28d-solo-EquipmentParameterInfo] importItemOverpowerDesc：✅ **IDENTICAL**
// （0x089bf290，266=266，P167 CLEAR 35/35）。本波 P165/P177 结构还原（贯穿建表链
// + 双检 goto 重试 + case 0/0x3001 共享 [stat] insert 调用点）+ 真语义修正（见尾部）。
// 调用点 ORIG 0x89ac5cb 同形 `xor $1` ⇒ 返回 bool（P168a）。
static bool importItemOverpowerDesc(EquipmentParameterInfo* info)
{
    std::string token;
    bool terminated = false;
    // ORIG 0x089bf2a8 `movb $0x1,-0x1d(%ebp)`：局部标志（同 importAllSkillItem）。
    bool s_first = true;
    int id;
    // [b28d-solo-EquipmentParameterInfo] P165/P177 结构还原（照 ORIG 0x089bf290
    // 逐条转写）：while(s_first) 底部条件 + 循环体内 static 三件套（每轮穿
    // __cxa_guard 检查）+ 双检 `if(__inited){leave; goto guard_retry}` +
    // `id=0` 贯穿建表（case 0 与 0x3001 共享块，首次插入两项后 goto 重试）+
    // switch 后 `if(!__inited){leave; __inited=true; goto guard_retry}`。
    while (s_first)
    {
        if (!ScanType(token, true))
            break;
    guard_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto guard_retry;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x3001:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[/item overpower desc]", 0x3001));
                // ORIG 0x089bf4fc `jmp 089bf50b`：第一项插入后**跳到 case 0x3005
                // 的 `if (!__inited)` 检查块**（共享 [stat] insert 调用点，
                // P167 实证 ORIG 该 insert 仅一个调用点）。
                goto L_insert_stat;
            }
            else
            {
                terminated = true;
                s_first = false;
            }
            break;
    L_insert_stat:
        case 0x3005:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[stat]", 0x3005));
            }
            else
            {
                // ORIG 0x89bf559：局部 vector C1 + GetStat（内部 push 12.0f +
                // GetStatValue，返回值取 %eax 残留）成功才 push 到 m_428；D1。
                std::vector<float> v;
                if (GetStat(&v))
                    info->m_119.m_428.push_back(v);
            }
            break;
        default:
            if (__inited)
                return 0;         // ORIG 0x89bf5b2：未知 token（已初始化）→ 失败
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto guard_retry;
        }
    }
    // ORIG 0x089bf5fb 段实证：`movzbl terminated; xor $1; test; jne <mov $1>` +
    // `call empty; test %al; je <mov $0>` ⇒ cond = `!terminated || empty()`；
    // cond==0（terminated 且 m_428 **非空**）才置 m_424/m_425=1 返回 1。
    // [真语义修正] 旧实现（及旧判词「空时置 1」）把条件判反：ORIG 是**非空**置 1。
    // 德摩根取反包裹形（P258）复现 cond 物化 + 共享 return 0 块，逐条全等。
    if (!(terminated && !info->m_119.m_428.empty()))
        return 0;
    info->m_119.m_424 = 1;
    info->m_119.m_425 = 1;
    return 1;
}

// ============================================================================
// importEquipmentParameterInfo —— ORIG 0x089a75ea（_Z28importEquipmentParameterInfo...
//   PKcRiS4_，单 token 分派，非文件加载）。param_2(path) 为当前关键字 token；
//   param_3(dir) 供 [then]/[multiple then] 递归 scanThen(dir,...) 用；
//   param_4(a)/param_5(b) 为 [if]/[then] 嵌套深度计数。每个命中分支写对应
//   具名字段并消费 token；结尾对非 [if]/[then] token 调 scanRandomOptionValue
//   （ORIG 经 param_1+0x434 子指针，字段未建模则跳过），返回 1。
//   本版覆盖已具名字段关键字；其余关键字落入结尾（返回 1 不中断脚本）。
// ============================================================================
// [b28d-solo-EquipmentParameterInfo] N（结构差封顶）：本波 p167 **MISSING 清零**
// （LevelUpSkill lu 循环外构造+循环内 clear、push_backEOi 右值直传、0x19d6/0x19f4 拆成
// 两个独立分支各一次 getActiveStatusToEnumValue）；残余 EXTRA string D1×6/ScanInt×6/
// operator==×5 等 = switch 后死 if 链等历史结构差（P165 结构还原未做）。5088 vs 4801。
// ============================================================================
// [BigEqParamTail 2026-09-09] 大件专项（4801 指令）逐段收敛：改前 4800/ORIG 4801
// （gap +1，slot 归一后结构差 **90 块**）→ 改后 4837/4801（gap −36，结构差 **43 块**）。
// 已修段（每段 ORIG 地址 → 类型 → 修法）：
//  ① 0x89a7842 switch 决策树含未列 case 0 → 补 `case 0:`（P163）⇒ 二分树/范围逐点对齐。
//  ② 0x89a8249 [if]：i 无槽（edx 直传）→ `scanIf(info, a++)` 实参内联（P0）。
//  ③ 0x89a82d8/0x89a8303/0x89a83b1 [then]：ORIG `jg <then>`+else 直落 → 正向包裹
//     `if (a<=b){a=-a;b=-b;return false;}`（P222）+ `b++` 实参内联（P0）+
//     `if (!scanThen(...)) return false;` 负极性（P168a X 族）⇒ 逐条同形。
//  ④ 0x89abdeb..0x89abf7a [active status control info]：10 分支恢复具名 int
//     v0..v13（P0 槽往返，ORIG -0xa4..-0x70 逐槽；含未使用值仍落槽）。
//  ⑤ 0x89a89da [recovery status]：`for(;;){v2=ScanFloat(&ok); if(!ok) break; …}` +
//     ok 声明先于 i（P164/P283 循环旋转、P212 声明序）。
//  ⑥ 0x89ab366/0x89aaacd/0x89aabba：三处三目改 if/else 两段直写成员（P47）。
//  ⑦ 0x89ac54e/0x89ac5cb：importAllSkillItem/importItemOverpowerDesc int→bool
//     （P168a 档①；调用点 `call; xor $1; test; je` 直证）。
//  ⑧ 0x89abaa3/0x89abbd9 [active status probability/level]：`for(j=0;j<=0x11;++j)
//     if(st==j) arr[j]=val;` 循环查找形 + 删冗余 st 范围守卫（P51/P175）。
//  ⑨ 真语义修复（0x89ace7e 共享块）：default 与 [variable stat] insert 后统一
//     `goto L_unknown` ⇒ `if (__inited) return false;`——原实现 break 直达
//     chain_end，已初始化 + 未知 token 会错误返回 true。
// 剩余 43 块判词（ORIG 地址区间 → 类型 → 封 N 依据）：
//  · 帧 0xadc vs 0xa9c（+0x40）+ 槽号整体平移 + 缺 `movl $0,-0xa10`：ORIG 多一个
//    跨 case 复用的 int 临时（[active status] 循环），补它需重构跨 case 共享临时 ⇒
//    N82/N126 版本帧基址族封顶（帧数达标≠槽号归位，P332）。
//  · 0x89a842a..0x89a866（[multiple then]）：ORIG 两个 string 局部（-0xa18/-0xa1c）+
//    完整 EH landing/回滚段，我方 4.4 单 string + 不同 EH 布局 ⇒ N91/N104 封顶。
//  · 0x89abfe7..0x89ac04a 注册链插入段与下个 case 标签的相对位置：指令集合一致，
//    仅 EH 段长度造成位置错位（difflib 伪差）⇒ N104。
//  · 0x89ac11c/0x89ac16b/0x89ac25d/0x89ac2af：两循环写 +0x10/+0xc 的语句顺序相反
//    （ORIG 先 +0x10）⇒ P324 叶块序族，改动连锁槽位，封 N。
//  · 0x89ac33e..0x89ac456：ORIG 用 for 循环写数组（回边 + setle 尾测），我方 4.4
//    展开为直写 ⇒ P165「循环抽象 vs 物理展开」封 N。
//  · 0x89ac9e8..0x89aca18：x87 比较 `fucompp/fnstsw/sahf` vs 4.4 `fucomip/fstp`
//    ⇒ N85 封顶。
//  · 0x89acab6/0x89acbca/0x89acbde：float 比较物化 + EH 段布局 ⇒ N85/N104。
//  · 0x89ace9c：L_chain_end 的 jmp 中继条数（我方多 2 条）⇒ N112 对齐填充。
// 验收（check_tu_routed fresh）：identical=446 / ae=1011 / near=3 / diff=14 /
//   missing_in_game=313——与本轮前完全一致（零回退）；p167 调用点差集保持 CLEAR。
// ============================================================================
#include "EquipmentParameterInfo/importEquipmentParameterInfo.inc"
}

// exportEquipmentParameterScript —— ORIG 0x089acf78（对照报告 1527-2007 行逐段转写）。
// 各段条件与标签文本照 ORIG：无名称的状态类型（如 0x12 默认）不输出任何内容。
// 注：ORIG 的 %.2f/%.1f 传参经 fistpl 取整后按 int 传入（Ghidra ROUND 伪影，原
// 生即为未定义 vararg 用法）；此处以可读 double 直出（数值舍入差异记录于报告）。
// avatar 能力标签表与 importEquipmentParameterInfo 内一致（AVTVARIATION_TAG_NAME）。
// [b28d-solo-EquipmentParameterInfo] 修 it++（P28）/std::min<int>；残差 avatar 段 string 形 + empty 结构 ⇒ N。
// [A18 本波] ndiff **500→360**（ORIG 1506 / OURS 1468）。四修 + 真语义修正 ×1：
//  ①**删多余判空**（P171 照抄）：ORIG 头部无 `if(!file||!info)`，直接
//     `mov 0xc(%ebp),%eax; mov 0x4(%eax),%eax; test; je` 判 info->m_field4；
//  ②**真语义修正**（ORIG 0x089add6d..0x089ade75 常量序实证）[active status] 名称段：
//     ORIG 链序 = 0..0xa, **0x11**, 0xb, 0xc, 0xd, 0xe（armor break@0x11、
//     bleeding@0xb、haste@0xc、bless@0xd、element@0xe；无 0xf/0x10）——
//     旧实现 armor break@0xb、bleeding..element 顺移一格（11..15）整体错位；
//     并改 **else-if 链**（ORIG 每条 fwrite 后 `jmp 0x89ade9d` 共享尾）；
//  ③**删具名 t 缓存**（P157/P183）：ORIG 每链 `mov 0x144(%eax),%eax; test/cmp $K`
//     字段直读（我方旧形 `mov %eax,-0x24; cmpl $K,-0x24`）；
//  ④separate-rate 段 x87 形（P243/P252/N85）：`fabs((double))` + 比较写 **`>`**
//     （`fabsf(sa-1.0f) > eps*fabsf(sa)`）⇒ 复现 ORIG 先算左值存 double 槽
//     `fstpl` + 后算 `epsilon()*fabs(sa)` 的求值序与 `fsubrp` 装载序；
//     残余 = **N85** x87 比较 lowering（ORIG `fucompp/fnstsw/test $5/sete/xor`
//     vs 4.4 `fucomip/fstp/seta`）+ 帧 0x180/0x170、槽平移 8（N95/N115 族）
//     + skill-levelup 迭代器三槽（ORIG 单槽 -0x3c，N86/P50 族）。
#include "EquipmentParameterInfo/exportEquipmentParameterScript.inc"
}

// ============================================================================
// importSkillDataUp —— ORIG 0x088c049c（_Z17importSkillDataUpRSt6vectorI20EquipmentSkillDataUpSaIS0_EE）。
// 从当前词法流逐条解析 equipment skill 数据。每条记录（ORIG 按偏移写入）：
//   job(+0x00)=getCharacterJob(名字)、value(+0x04)=ScanInt、module(+0x18)=scanModuleType、
//   下一 token 为标签；按标签（[static]/[level]/[cooltime]/[mp]/[maintain mp]/
//   [skill consume item]/[casting time]）分派设置 key(+0x14)=0..6 并消费一个 ScanInt 到
//   level(+0x08)；再读下一 token 置 percent(+0x0c)=（token!="+"），ScanInt 到 value(+0x10)。
//   ORIG 的 __lookup 静态 map + __inited 惰性初始化仅为编译期优化（每条首遍注册标签），
//   行为等价于按标签字符串 if/else（同本文件 readAction 注释约定）。对象在函数顶部构造
//   一次、逐条覆写复用；未知标签不消费 level token，key/level 保持上一轮/ctor 默认。
// ============================================================================
// [b28d-solo-EquipmentParameterInfo] importSkillDataUp：P167 CLEAR ⇒ 语义完成、byte 层 N 封顶。
#include "EquipmentParameterInfo/importSkillDataUp.inc"
}

// ============================ EquipmentParameterInfo::._119/._159 别名 ============================
// GCC 对 ctor/dtor 的 asm 别名（asm("...")）无效（硬限制）；匿名嵌套 struct 又按本 TU
// 行号计数命名（如 4._85），与 ORIG `5._119`/`5._159` 不一致。故在类内命名 `_119`，
// 用 `.set` 把自然 mangled `5t_119*` 强制改名为 ORIG 带点号 `5._119*`（ctor/dtor 四件）。
// **operator= 例外（b28d-solo-EquipmentParameterInfo 实证）**：asm 标签对 operator=
// **有效**（探针 /tmp/b28d_epi_probe/asmlabel.cpp：声明带标签 ⇒ 定义与调用点重定位
// 均用标签名），故两个 op= 的符号名由头文件 asm 标签直接指定（t_119 → `._159aSERKS0_`
// 匹配 ORIG 0x08348024 的调用点；tt_159 → `._119aSERKS0_` 对应 ORIG 0x088fd910），
// 不再走 `.set`（P262：两别名须各有独立函数体，此式满足）。
// cf. docs/class_func_reports/EquipmentParameterInfo__._119
// （0x088fd884/0x0898c680）与 ._159（0x08348024/0x0861c9b8）。
asm(".set _ZN22EquipmentParameterInfo5._119C1Ev, _ZN22EquipmentParameterInfo5t_119C1Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119C1Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119C1Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119C1Ev,0x45");
asm(".set _ZN22EquipmentParameterInfo5._119C2Ev, _ZN22EquipmentParameterInfo5t_119C2Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119C2Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119C2Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119C2Ev,0x45");
asm(".set _ZN22EquipmentParameterInfo5._119D1Ev, _ZN22EquipmentParameterInfo5t_119D1Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119D1Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119D1Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119D1Ev,0x45");
asm(".set _ZN22EquipmentParameterInfo5._119D2Ev, _ZN22EquipmentParameterInfo5t_119D2Ev\n"
    ".globl _ZN22EquipmentParameterInfo5._119D2Ev\n"
    ".type _ZN22EquipmentParameterInfo5._119D2Ev,@function\n"
    ".size _ZN22EquipmentParameterInfo5._119D2Ev,0x45");
