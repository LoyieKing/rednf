// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STEquipmentScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— STEquipmentScript（G5 装备脚本表）
// 逐函数对照 docs/class_func_reports/：
//   STEquipmentScript/            ctor 0x0898db82 / dtor 0x0898e298 /
//                                 Clear 0x0898e9e4 / addAniInfo 0x0898efa8 /
//                                 getAniInfo 0x0898ef1a / isUpgradableLevel
//                                 0x0898efc8 / isExistUpgradableLevel
//                                 0x0898f066
//   STEquipmentScript__artifact_info/  ctor 0x0898f084 / isUsable 0x0898f0fe
//   EquipmentAniInfoScript/       ctor 0x0898d03e / clear 0x0898d142
//   _lt_global_gt_/ImportEquipmentScript.md  T 0x08991aeb（9821 insn）
// 支撑类型生命周期（ORIG 同编译单元段）：
//   STEquipmentAniScript::C1/clear   T 0x0898ca36 / 0x0898ce62
//   stAvatarTypeSelect_t::init       T 0x0898d1fc
//   GrowthEquipmentInfo::C1/clear    W 0x089c2fc4 / 0x089c300c
//   Position::Position               W 0x089c2c42
//   effect::STEffect::clear          W 0x089c2c64
// ============================================================================
// [Lane23 2026-09-10 收口] 本轮 8 件 DIFF/NEAR → AE（四数 710/1439/2/7/464；
//   基线 710/1431/3/14/464 ⇒ ae+8 / near−1 / diff−7，零回退）：
//   EquipmentScriptSpecialMonsterDrop 176→172（共享 `return true`：if→else-if 链）；
//   GetAuraAvatarOption 259→256、GetExpertJobTypeList 329→327（P333 static 三件套
//   移入重查循环体 + P164 内层 for 替 goto + 独立 int v 判别值）；
//   importArtifactInfo 450→448（find 内联 c_str 实参 + break 出循环 + 循环外 return）；
//   importEffect 314→307（[/module]/[file name] 改落链尾，消 4.4 的 nop+jmp 反向块）；
//   importCommand 259→254（while((token=getToken())!=0) 条件段单点取 token +
//   name 内联为 find 实参）；
//   GetExpertJobValueMaterialVariation 29→27（x87 比较写 `-100.0f >= field` 交换
//   操作数方向，复现 4.1 `test $0x5,%ah; sete`）；
//   setEquipmentAniScriptLay NEAR→AE（rbegin() 具名 reverse_iterator 复位深槽）。
//   余 2 件游戏域：ImportEquipmentScript（帧 0x19fc vs 0x1a3c，N82 槽复用族）、
//   GetItemCategory（state 占 4.1 深槽 -0x78 vs 4.4 浅槽 -0x20）。
//   [补 2026-09-10] ImportEquipmentScript prologue 顺序修正：stopFlag/curParam/
//   setAbilityBackup 的声明点移到 m_str664 赋值之后（ORIG idx 132..138 同序，
//   消旧形 prologue 多发的 12 条），upgMin/upgMax/upgType 删声明处初值
//   （ORIG 初值在 [upgrade] 段 idx 7992..8000 每次进入时重置，本文件同序已有）。
//   四数不变（710/1439/2/7/464）；该件余量仍为全帧 0x40 槽差（1451 差异块/5579 行）。
// [SweepSTEquip 2026-09-09 单文件泳道收口判词] 本轮 13 件逐件读数：
//   ■ 真缺口修复 8 件（gap 降序）：
//     setEquipmentAniScriptLay 44→2（find 传 key->c_str() 临时 string /
//       rbegin()->push_back 语义纠错 / 迭代器先声明后赋值 / 两循环复用 it）；
//     GetItemCategory 32→5、GetAuraAvatarOption 32→5、GetExpertJobTypeList
//       18→10（三件同为 P333「首次建表后查表」switch 状态机还原：哨兵 case 0
//       与首 key 同体 + 每 case `movzbl __inited; xor $1` + fallthrough 链 +
//       尾块 leave→__inited→goto retry；余量为 4.4 EH landing（EXTRA
//       _Unwind_Resume×2，N104）与帧差（N115/N126））；
//     STEquipmentAniScript::C1Ev 22→0（375=375）、C1ERKS_ 433=433、
//       D1Ev 332=332、aSERKS_ 180=180、clear ?→0（118=118）——P151 数组成员
//       建模：+0x10/+0x14→m_arr10[2]、+0x2c..+0x38→m_arr2c[4]、
//       +0x3c..+0x48→m_arr3c[4]（.h 改动已广播；sizeof 0x7c/偏移零变化）。
//   ■ 余量为结构性 N（p167 调用点差集已 CLEAR，仅剩 C1↔C2/D1↔D2 同址别名命名差）：
//     importEffect gap 4、setEquipmentAniScriptLay gap 2（帧 0x25c vs 0x26c，
//     N126/N115）、GetExpertJobTypeList gap 10（guard 位置跨版本 + 帧 0xac vs 0xbc）、
//     importCommand gap −4（4.4 多 4 条槽/声明序）、EquipmentScriptSpecialMonsterDrop
//     gap −4、GetExpertJobValueMaterialVariation gap −2（N85 x87 lowering）。
//   ■ 前泳道已修、本泳道未重开：importArtifactInfo（P333 已落）、ImportEquipmentScript。
//   TU：check_tu_routed.sh COMPILE OK 0 错误，identical=710 ae=1431 near=2
//     diff=15 missing_in_game=464（基线 709/1426/2/20/468 ⇒ ae+5、diff−5、miss−4，零回退）。
//   P0：本文件零 `register asm` 绑定、零裸 register（清绑数 0）。
// ============================================================================

#include "STEquipmentScript.h"
#include "STSkillScript.h"

#include <string.h>

#include "CCubeStatistic.h"    // getItemGroupNameTable()（ORIG 0x088c0409）
#include "DNFLexWrapper.h"

// ---- 跨 TU 符号（ORIG 真实符号；定义归各自属主批次，此处仅按 ABI 引用） ----
#include "SyncScriptDeps.h"    // toTString（importCommand 键名转换）
#include "CEnvironment.h"      // SpinLock / less_TCHAR（GetExpertJobTypeList）
#include "NpcScript.h"

// ---- 跨 TU 符号（InitScriptProducers_5.cpp 提供） ----
#include "STEquipmentScript/InitMapEquipmentAniScript.inc"
extern std::map<std::string, STEquipmentAniScript>* g_mapEquipmentAniScript;  // B 0x094f7bf8
// ORIG 全局脚本文件列表（权威视图同 NpcScript.h：pad 0x18 + 查找方法）

extern STScriptFileList g_stEquipmentScriptFileList;        // ORIG B 0x94fac00

#include "STEquipmentScript/scanModuleType.inc"
#include "STEquipmentScript/getModuleType.inc"
#include "STEquipmentScript/getCalculationType.inc"
#include "STEquipmentScript/scanWorldType.inc"
#include "STEquipmentScript/GetEquipmentType.inc"
#include "STEquipmentScript/getAvatarEmblemType.inc"
#include "STEquipmentScript/importEffect.inc"
#include "STEquipmentScript/importCommand.inc"
static bool GetItemCategory(STItemScript* item);                   // t 0x089bfdc5
static bool GetAuraAvatarOption(STEquipmentScript* script);        // t 0x089c019b
static bool GetExpertJobTypeList(STItemScript* item);       // t 0x089bf6ca
static bool GetExpertJobValueMaterialVariation(STItemScript* it);  // t 0x089bfc2f
static bool GetExpertJobValueCompoundRate(STItemScript* item);     // t 0x089bfc84
static bool GetExpertJobResultVariation(STItemScript* item);       // t 0x089bfcb5
static bool GetExpertJobDisjointBigWinRate(STItemScript* item);    // t 0x089bfd0b
static bool GetExpertJobDisjointResultVariation(STItemScript* i);  // t 0x089bfd3c
static bool GetExpertJobAdditionalGainExp(STItemScript* item);     // t 0x089bfd6d
#include "STEquipmentScript/getCharacterJob.inc"
#include "STEquipmentScript/getCharacterJobToEnumValue.inc"
#include "STEquipmentScript/getEquipmentTypeToEnumValue.inc"
#include "STEquipmentScript/loadAppendageType.inc"
#include "STEquipmentScript/processModuleTagSet.inc"
#include "STEquipmentScript/setEquipmentAniScriptLay.inc"
#include "STEquipmentScript/importEquipmentParameterInfo.inc"

}  // namespace

#include "STEquipmentScript/STEquipmentStatInfo.inc"
}

#include "STEquipmentScript/clear.inc"
}

#include "STEquipmentScript/STEquipmentAniScript.inc"
}

#include "STEquipmentScript/clear_1.inc"
}

#include "STEquipmentScript/init.inc"
}

#include "STEquipmentScript/GrowthEquipmentInfo.inc"
}

#include "STEquipmentScript/clear_2.inc"
}

#include "STEquipmentScript/EquipmentAniInfoScript.inc"
}

#include "STEquipmentScript/clear_3.inc"
}

#include "STEquipmentScript/Position.inc"
}

#include "STEquipmentScript/STEffect.inc"
}

#include "STEquipmentScript/clear_4.inc"
}

// ===================== STEquipmentScript 本体 =====================

#include "STEquipmentScript/STEquipmentScript.inc"
}

#include "STEquipmentScript/dtor_STEquipmentScript.inc"
}

#include "STEquipmentScript/Clear.inc"
}

#include "STEquipmentScript/addAniInfo.inc"
}

#include "STEquipmentScript/getAniInfo.inc"
}

#include "STEquipmentScript/isUpgradableLevel.inc"
}

#include "STEquipmentScript/isExistUpgradableLevel.inc"
}

#include "STEquipmentScript/artifact_info.inc"
}

#include "STEquipmentScript/isUsable.inc"
}

// ===================== 脚本导入（T 0x08991aeb，9821 insn） =====================
// ORIG 用 static map<char const*,int> 查找表 + SpinLock 双检锁把关键字分派为三段
// switch（主属性 local_98 / 动画扩展 local_70 / 升级子键 local_3c）；未命中依次落
// 到 importEquipmentParameterInfo（参数区解析器）与 processModuleTagSet。
// 此处按同文件其它 ST*Script 导入惯例以字符串比较实现等价分派（见 ImportMapScript）。
//
// 控制流实证（汇编 0x08991d82/0x0899c9bb/0x0899c9e2/0x0899c9f1）：
//   while (ScanType(token)) { 分派…; if (stopFlag) break; } → 收尾 return true；
//   ScanType 失败（EOF）同样跳收尾（jne 0x0899c9f1→0x0899c9f2），并非失败路径；
//   失败路径：参数解析器 false 且双计数 <0 → return 0（不 unload），以及各 case
//   内显式失败（unaff_EBX=0 → cleanup return 0，不 unload）。
//
// 标签→body 对齐方法：以 ORIG switch 路由表（cmp id; je LAB）为准取各 LAB 的
// inited 分支体；关键样本已用 objdump 逐块复核（[weight]/[icon]/[attack info]/
// [hide layer] 族/[usable period] 等）。

#include "STEquipmentScript/ImportEquipmentScript.inc"
}

#include "STEquipmentScript/EquipmentScriptSpecialMonsterDrop.inc"
}

#include "STEquipmentScript/importArtifactInfo.inc"
}

// ===================== Wave3Deps：脚本解析依赖组 =====================
// ORIG 同段（ImportEquipmentScript 解析器引用的文件级 helper）：

// ---- ORIG B 0x0949b348：键命令名称→索引表（importCommand 查询；填充方为
//      ORIG TU 内 static importKeyNameIndexTable，随脚本装载批次接入） ----
std::map<std::string, int> g_mapKeyNameIndexTable;

// ---- ORIG W 0x088c414c ----
#include "STEquipmentScript/clear_5.inc"
}

// ---- ORIG W 0x088c4174（vector 自构造后 clear 置默认值） ----
#include "STEquipmentScript/STKeyCommandUnit.inc"
}

// ---- ORIG T 0x089c05dd：惰性初始化的全局动画表指针 ----
std::map<std::string, STEquipmentAniScript>* getMapEquipmentAniScript()
{
    if (g_mapEquipmentAniScript == 0)
        InitMapEquipmentAniScript();
    return g_mapEquipmentAniScript;
}

// ---- ORIG t 0x089bfdc5：[item category] 标签序列 → item->m_set11c。
//      标签表惰性构建（ORIG 函数级静态 map @0x94fde28 + SpinLock @0x94fde40
//      + 就绪字节 @0x94fde44，双检锁形态）；未知标签令整个导入失败。 ----
static bool GetItemCategory(STItemScript* item)
{
    // [SweepSTEquip 2026-09-09 P333 状态机还原] ORIG 0x089bfdc5 是「首次建表、
    // 之后查表」的 switch 状态机，不是直线双检锁：
    //   ① 循环体首 `state(-0x78)=0`（0x089bfde1），retry 锚 0x089bfde8；
    //   ② 已初始化走 find → `state = it!=end ? it->second : -1`（0x089bff7e
    //      汇入 switch）；未初始化 enter 后若已被他线程建表则 leave+回跳
    //      （0x089bffa3 nop / 0x089bffa4 jmp retry），否则 `state = 0` 哨兵；
    //   ③ switch(state)：case 0 与 case 0x30ae 同体，每 case 体头
    //      `movzbl __inited; xor $1; test; je INSERT`；已初始化则
    //      `state = 1/2/3` 后 break，否则 insert 并 fallthrough 下一 case
    //      （0x089c002d/0x089c0087/0x089c00de 的 jmp 下一 case 体头）；
    //   ④ default：`if (__inited) return false`（0x089c00e9），未初始化则落尾；
    //   ⑤ 尾块 `if (!__inited) { leave; __inited = true; goto retry; }`
    //      （0x089c00fe..0x089c011f），随后 `if (state != 0) insert`。
    // 旧译文把建表写成一次性三连 insert，缺整棵 switch 与回跳 ⇒ gap 32。
    // [SweepSTEquipScript 2026-09-09] 三处 case 体极性反转（同 GetExpertJobTypeList）：
    // gap 5 → -1（254/255）。残差 1 条 = ORIG `mov slot,%eax; test %eax,%eax; je`
    // 被 4.4 折成 `cmpl $0,slot; je`（P209/N18b 跨版本折叠，无源码解）。
    // [P28/P212 全头部声明 2026-09-10] ORIG 的 state 占深槽 -0x78（与 token -0x74
    // 同区），声明在函数级；循环体首只做 `state = 0`（ORIG 0x089bfde1）。
    // [E06 2026-09-10 实验·翻案失败] state 标量形恒落浅区 -0x20、POD 聚合包装版
    // 254（劣 1 条且槽未回深区）、unsigned 形 253 与 int 形同值 ⇒ 按 P321-S1
    // （赌输恢复直证形）恢复 int，N82附/N93（4.4 聚合优先低槽+标量分区）封顶。
    int state;
    std::string token;
    int v;                      // switch 判别值（ORIG -0x1c）
    while (ScanStr(&token))
    {
        state = 0;
        // [P164 真解 2026-09-10，Main 广播] `retry: … goto retry` 会令 4.4 在循环头
        // 前插 entry jmp+nop；改为内层 `for (;;)`（guard 竞争 `continue`、重派发块
        // `if(!__inited){…} else {break;} continue;`，continue 置于内层体末）。
        for (;;)
        {
        // [P333 修正 2026-09-10] ORIG 的 static 三件套声明在**循环体内**（retry 锚
        // 处）：guard 检查随 `goto retry` 逐轮重放（ORIG 0x089bfe62/0x089bfedc 的
        // `mov $__lookup_guard`/`movzbl; test; jne` 位于 retry 锚 +0x023），
        // 函数入口只有 token 构造 + `jmp 循环条件`（0x089bfdd1..0x089bfddc）。
        // 旧形把三件套声明在函数头 ⇒ guard 段被提到函数入口（token 构造之前），
        // 与 ORIG 顺序相反。声明位置不影响静态初始化语义（仍只初始化一次）。
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        // [P333 修正 2026-09-10] ORIG 有两个 int：`state`（-0x78，case 体里写
        // ITEM_CATEGORY_1/2/3）与 switch 判别值 `v`（-0x1c），两者同为函数级。
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            v = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                              // 自旋重试（P333：重放 guard）
            }
            v = 0;                                     // 建表哨兵
        }
        switch (v)
        {
        case 0:                                        // 与首 key 同体（P163）
        case 0x30ae:                                   // "monster card"
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("monster card", 0x30ae));
            }
            else
            {
                state = STItemScript::ITEM_CATEGORY_1;
                break;
            }
            /* FALLTHRU */
        case 0x30b1:                                   // "cannot bind"
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("cannot bind", 0x30b1));
            }
            else
            {
                state = STItemScript::ITEM_CATEGORY_2;
                break;
            }
            /* FALLTHRU */
        case 0x30b4:                                   // "boss drop"
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("boss drop", 0x30b4));
            }
            else
            {
                state = STItemScript::ITEM_CATEGORY_3;
                break;
            }
            /* FALLTHRU */
        default:
            if (__inited)
                return false;                          // 未命中 → 导入失败
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
        }
        else
        {
            break;                                     // 已建表 → 退出内层重查循环
        }
        continue;                                      // 建表后重查本 token
        }
        if (state != 0)
        {
            item->m_set11c.insert(
                static_cast<STItemScript::ENUM_ITEM_CATEGORY>(state));
        }
    }
    return true;
}

// ---- ORIG t 0x089c019b：[aura ability] 单标签分派 → script->m_880 模式 +
//      m_884 参数。同型惰性标签表（map @0x94fde08 / SpinLock @0x94fde20 /
//      就绪字节 @0x94fde24）。ScanStr 返回值不检查——空 token 必然 miss，
static bool GetAuraAvatarOption(STEquipmentScript* script)
{
    // [SweepSTEquip 2026-09-09 P333 状态机还原] ORIG 0x089c019b 与 GetItemCategory
    // 同型：单次 ScanStr（返回值不检查）→ guard → retry 锚 0x089c01ce →
    // switch(state)：case 0 与 0x30d0 同体；每 case 体头
    // `movzbl __inited; xor $1; test; je INSERT`，已建表分支做业务写入
    // （0x089c0419 m_880=0 + ScanInt / 0x089c048a m_880=1 m_884=0 /
    // 0x089c04f4 m_880=2 + ScanInt）后 break；未建表 insert 并 fallthrough；
    // default `if (__inited) return false`；尾块 `if (!__inited) { leave;
    // __inited = true; goto retry; }`（0x089c0526..0x089c0547）。
    // 旧译文建表三连 insert 直线形，缺整棵 switch 与回跳 ⇒ gap 32。
    // [SweepSTEquipScript 2026-09-09] 三处 case 体极性反转（同 GetExpertJobTypeList）：
    // gap 5 → -1（258/259）。残差 1 条 = P209/N18b 零比较折叠（同上件）。
    if (script == 0)
        return false;

    bool ok = true;
    std::string token;
    ScanStr(&token);

    // [P164/P333 修正 2026-09-10] static 三件套移入重查循环体内（ORIG guard 段
    // 0x089c0248/0x089c02c2 在 retry 锚 +0x033；`goto retry` 改内层 `for (;;)`
    // 消 entry jmp+nop）；switch 判别值用独立无初始化 `int v`（ORIG -0x1c）。
    for (;;)
    {
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    int v;
    if (__inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(token.c_str());
        v = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();
        if (__inited)
        {
            __init_lock.leave();
            continue;                              // 自旋重试（P333：重放 guard）
        }
        v = 0;                                     // 建表哨兵
    }
    switch (v)
    {
    case 0:                                        // 与首 key 同体（P163）
    case 0x30d0:                                   // "[party teleport]"
        if (!__inited)
        {
            // P61/P81：ORIG 用 pair 直构（MISSING pair<char const(&)[17],int> converting
            // ctor），make_pair 会实例化另一模板并多一次转换临时。
            __lookup.insert(std::pair<char const* const, int>("[party teleport]", 0x30d0));
        }
        else
        {
            script->m_880 = 0;
            script->m_884 = ScanInt(&ok);
            break;
        }
        /* FALLTHRU */
    case 0x30d5:                                   // "[none]"
        if (!__inited)
        {
            __lookup.insert(std::pair<char const* const, int>("[none]", 0x30d5));
        }
        else
        {
            script->m_880 = 1;
            script->m_884 = 0;
            break;
        }
        /* FALLTHRU */
    case 0x30db:                                   // "[solo teleport]"
        if (!__inited)
        {
            __lookup.insert(std::pair<char const* const, int>("[solo teleport]", 0x30db));
        }
        else
        {
            script->m_880 = 2;
            script->m_884 = ScanInt(&ok);
            break;
        }
        /* FALLTHRU */
    default:
        if (__inited)
            return false;                          // 未命中 → 导入失败
        break;
    }
    if (!__inited)
    {
        __init_lock.leave();
        __inited = true;
    }
    else
    {
        break;                                     // 已建表 → 退出内层重查循环
    }
    continue;                                      // 建表后重查本 token
    }
    return ok;
}

// ---- ORIG T 0x08990f1f：[equipment type] 字符串→枚举（26 项，失配→0x1a；
//      表 0x93f4fc0 的 26 个字面量按序展开） ----
#include "STEquipmentScript/GetEquipmentType_1.inc"
}

// ---- ORIG T 0x088c227b：[calculation type]（'+'→0、'%'→1、其余→2） ----
#include "STEquipmentScript/getCalculationType_1.inc"
}

// ---- ORIG T 0x088c1492：world 标签→类型 ----
#include "STEquipmentScript/getWorldType.inc"
}

// ---- ORIG T 0x088c1430 ----
#include "STEquipmentScript/scanWorldType_1.inc"
}

// ---- ORIG T 0x088c167a：[command] 键序列解析 ----
// 循环体按 token 分派：5=终止收集；6=键名查表入 m_ids；2=[speed] 十进制；
// 7=[type] DOWN/DOWN ONCE/UP/UP ONCE → m_18=0..3；8=连接符（'&' 把 m_ids 并入
// command->m_vecC，'!'','~' 设超时后把 unit 追加进 m_vec0）。收尾再追加残 unit。
#include "STEquipmentScript/importCommand_1.inc"
}

// ---- ORIG T 0x088c1a0d：[effect] 条目解析 ----
#include "STEquipmentScript/importEffect_1.inc"
}

// ---- ORIG T 0x089c1cb9：动画层提交（normal/level/job 三层 + def 模板逐槽拼接） ----
#include "STEquipmentScript/setEquipmentAniScriptLay_1.inc"

#define SETANI_SLOT(member)                                                    \
    do                                                                         \
    {                                                                          \
        /* ORIG 0x89c1e63..：格式串 = def.member（脚本模板），参数 = path */     \
        snprintf(buf, sizeof(buf), def.member.c_str(), path->c_str());         \
        ani->member = buf;                                                     \
    } while (0)

    // ORIG 拼接顺序（88c1e63..25c2）：…2c、3c、30、40、34、44、38、48 交错。
    SETANI_SLOT(m_str04);
    SETANI_SLOT(m_str08);
    SETANI_SLOT(m_str0c);
    SETANI_SLOT(m_arr10[0]);   // +0x10
    SETANI_SLOT(m_arr10[1]);   // +0x14
    SETANI_SLOT(m_str18);
    SETANI_SLOT(m_str1c);
    SETANI_SLOT(m_str20);
    SETANI_SLOT(m_str24);
    SETANI_SLOT(m_str28);
    SETANI_SLOT(m_arr2c[0]);   // +0x2c
    SETANI_SLOT(m_arr3c[0]);   // +0x3c
    SETANI_SLOT(m_arr2c[1]);   // +0x30
    SETANI_SLOT(m_arr3c[1]);   // +0x40
    SETANI_SLOT(m_arr2c[2]);   // +0x34
    SETANI_SLOT(m_arr3c[2]);   // +0x44
    SETANI_SLOT(m_arr2c[3]);   // +0x38
    SETANI_SLOT(m_arr3c[3]);   // +0x48
    SETANI_SLOT(m_str4c);
    SETANI_SLOT(m_str50);
    SETANI_SLOT(m_str54);
    SETANI_SLOT(m_str58);
    SETANI_SLOT(m_str5c);
#undef SETANI_SLOT

    // ORIG 0x089c25c7：迭代器先默认构造（`__normal_iterator::C1Ev`，P50 复活）
    // 再 `it = begin()`（0x089c25cd C1Ev → 0x089c25e5 beginEv → 0x089c25f3 赋值）；
    // 两个循环**复用同一 it 槽 -0x48**（P160 复用槽律）。
    std::vector<std::string>::const_iterator it;
    it = def.m_vec64.begin();
    for (; it != def.m_vec64.end(); ++it)
    {
        // ORIG 0x89c2601..0x89c2634：格式串 = it 元素，参数 = path
        snprintf(buf, sizeof(buf), it->c_str(), path->c_str());
        ani->m_vec64.push_back(std::string(buf));
    }
    it = def.m_vec70.begin();
    for (; it != def.m_vec70.end(); ++it)
    {
        // ORIG 0x89c272b..0x89c275e：同上
        snprintf(buf, sizeof(buf), it->c_str(), path->c_str());
        ani->m_vec70.push_back(std::string(buf));
    }
}

// ---- 专家职业解析组（ORIG t 局部静态；关键字映射表/SpinLock/inited 三件套） ----

// ORIG t 0x089bf6ca：[usable list] —— 关键字→typeSel(1..5) + 每项等级下限。
static bool GetExpertJobTypeList(STItemScript* item)
{
    // [SweepSTEquip 2026-09-09 P333 状态机还原] ORIG 0x089bf6ca 是「首次建表、
    // 之后查表」的 switch 状态机（与 GetItemCategory/GetAuraAvatarOption 同族）：
    //   ① 循环条件 0x089bfbd2 为 `scanning && ScanStr(&token)`（scanning 是
    //      0x089bf704 `movb $0x1,-0x21` 的永不修改死变量，P171 照抄）；
    //   ② 循环体首 `typeSel(-0x20)=0`（0x089bf726），retry 锚 0x089bf72d；
    //      已初始化走 const_iterator find（`typeSel = it!=end ? it->second : -1`），
    //      未初始化 enter 后若已被建表则 leave+回跳（0x089bf8??），否则 typeSel=0；
    //   ③ switch(typeSel)：case 0 与 case 0x303a 同体，每 case 体头
    //      `movzbl __inited; xor $1; test; je INSERT`；已初始化则 `typeSel=1..5`
    //      后 break 到 0x089bfb38，否则 insert 并 fallthrough 下一 case；
    //   ④ default：`if (__inited) return false`；
    //   ⑤ 尾块 `if (!__inited) { leave; __inited = true; goto retry; }`；
    //   ⑥ 业务块 0x089bfb5f：`int level=0; if(!ScanInt(&level)) return false;
    //      if (typeSel != 0) { entry...; push_back; }` 后回循环条件；
    //      收尾 `return !item->m_expertJob.m_usable.empty();`。
    // 旧译文把 find/switch 拆到两个分支（EXTRA 非 const find/end/c_str ×1），
    // 且建表三连 insert 直线形，缺整棵 switch 与回跳 ⇒ gap 18。
    // [SweepSTEquipScript 2026-09-09] 五处 case 体极性反转（ORIG 每 case
    // `movzbl __inited; xor $1; test; je INSERT` ⇒ 源形 `if(!__inited){insert}else{...}`）：
    // gap 10 → 0（330/330）。残余 DIFF 为槽号/寄存器差（N82）。
    if (item == 0)
        return false;
    item->m_expertJob.m_usable.clear();

    std::string token;
    bool scanning = true;
    while (scanning)
    {
        // ORIG 0x089bf70d 是循环体开头（ScanStr 在此），0x089bfbd2 的条件只测
        // scanning 后跳回体头 ⇒ 源形是 `while (scanning) { if(!ScanStr) break; }`，
        // 不是 `while (scanning && ScanStr(&token))`（后者把 ScanStr 挪进条件段）。
        if (!ScanStr(&token))
            break;
        int typeSel = 0;
        // [P164/P333 修正 2026-09-10] 同 GetItemCategory：static 三件套移入重查
        // 循环体内（ORIG guard 段 0x089bf72d/0x089bf7a7 在 retry 锚 +0x063），
        // `goto retry` 改内层 `for (;;)`（消 4.4 entry jmp+nop）；switch 判别值
        // 用独立无初始化 `int v`（ORIG -0x1c，业务值 typeSel 在 -0x20）。
        for (;;)
        {
        static std::map<const char*, int, less_TCHAR> __lookup;  // 0x94fde48
        static SpinLock __init_lock;                             // 0x94fde60
        static bool __inited = false;                            // 0x94fde64
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator found =
                __lookup.find(token.c_str());
            v = (found != __lookup.end()) ? found->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                              // 自旋重试（P333：重放 guard）
            }
            v = 0;                                     // 建表哨兵
        }
        switch (v)
        {
        case 0:                                        // 与首 key 同体（P163）
        case 0x303a:                                   // enchanter
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("enchanter", 0x303a));
            }
            else
            {
                typeSel = 1;
                break;
            }
            /* FALLTHRU */
        case 0x303d:                                   // alchemist
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("alchemist", 0x303d));
            }
            else
            {
                typeSel = 2;
                break;
            }
            /* FALLTHRU */
        case 0x3040:                                   // disjointer
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("disjointer", 0x3040));
            }
            else
            {
                typeSel = 3;
                break;
            }
            /* FALLTHRU */
        case 0x3043:                                   // doll_controller
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("doll_controller", 0x3043));
            }
            else
            {
                typeSel = 4;
                break;
            }
            /* FALLTHRU */
        case 0x3046:                                   // all
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("all", 0x3046));
            }
            else
            {
                typeSel = 5;
                break;
            }
            /* FALLTHRU */
        default:
            if (__inited)
                return false;                          // 未命中 → 导入失败
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
        }
        else
        {
            break;                                     // 已建表 → 退出内层重查循环
        }
        continue;                                      // 建表后重查本 token
        }

        int level = 0;
        if (!ScanInt(&level))
            return false;
        if (typeSel != 0)
        {
            // ORIG 0x089bfb8e/0x089bfb98 先 `movl $0x0,-0xac/-0xa8` 两条清零，
            // 再写 m_field0/m_field4 ⇒ value-init（POD 聚合零初始化）形。
            STItemScript::SEXPERTJOB::SUSABLE entry = STItemScript::SEXPERTJOB::SUSABLE();
            entry.m_field0 = static_cast<ENUM_EXPERT_JOB_TYPE>(typeSel);
            entry.m_field4 = level;
            item->m_expertJob.m_usable.push_back(entry);
        }
    }
    return !item->m_expertJob.m_usable.empty();
}

// ORIG t 0x089bfc2f：-100.0 哨兵（0x8e1a574）视为未设置。
#include "STEquipmentScript/GetExpertJobValueMaterialVariation.inc"
}

// ORIG t 0x089bfc84
#include "STEquipmentScript/GetExpertJobValueCompoundRate.inc"
}

// ORIG t 0x089bfcb5：第一 float 成功即返回；否则读第二槽。
static bool GetExpertJobResultVariation(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_variation.m_field0 = ScanFloat(&ok);
    if (!ok)
        return ok;   // ORIG：xor$1 测试 !ok，!ok 分支 movzbl 返回
    item->m_expertJob.m_variation.m_field4 = ScanFloat(&ok);
    return ok;
}

// ORIG t 0x089bfd0b
static bool GetExpertJobDisjointBigWinRate(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field10 = ScanFloat(&ok);
    return ok;
}

// ORIG t 0x089bfd3c
static bool GetExpertJobDisjointResultVariation(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field14 = ScanFloat(&ok);
    return ok;
}

// ORIG t 0x089bfd6d：float(+0x140) 失败时回退读 int(+0x144)。
static bool GetExpertJobAdditionalGainExp(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_additionalExp.m_field0 = ScanFloat(&ok);
    if (!ok)              // ORIG：xor$1 测试 !ok，!ok 分支 movzbl 返 ok
        return ok;
    item->m_expertJob.m_additionalExp.m_field4 = ScanInt(&ok);
    return ok;
}

// ---- ORIG T 0x08990f66：纹章槽前缀→位标志（strncmp 前缀匹配；[All socket]
//      长度 12；其余 10；失配→0） ----
#include "STEquipmentScript/getAvatarEmblemType_1.inc"
}
