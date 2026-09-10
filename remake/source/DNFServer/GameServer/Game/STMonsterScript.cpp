// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STMonsterScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— STMonsterScript 类方法（G5 怪物脚本表）。
// 逐函数依据 ORIG objdump 转写（地址随注）：
//   Clear T 0x08a398aa / C1 W 0x08368e14 / D1 W 0x08378dac / op= W 0x08377994
//   ImportMonsterBaseTable T 0x08a3c736（全量转写）
//   getMonsterTableIndex T 0x088ba1c8 / getRandomChampionElement T 0x088b9cce
//   setBasicAbilityByLevel T 0x088b9603（全量转写）/ isNamedMonster W 0x088ba18c
//   ImportMonsterScript T 0x08a12827 / exportMonsterScript T 0x08a22520
// [b28d-23 N·N90] STMonsterScript::operator= W 0x08377994 为编译器隐式拷贝
// （1200/1272，ORIG 大 dword 成员分块启发式跨版本差）⇒ N。
// [L14 round2 复核] operator= 1200/1200、p167 CALLSITE CLEAR（89/89）；
// setBasicAbilityByLevel 481/482、p167 CALLSITE CLEAR（25/25）⇒ 语义完成，残余 N90/N82。
// exportMonsterScript 6791/6792：p167 仅 C1↔C2 同址别名（STWarp/vector）+ EXTRA 族
// （_ZNSsD1Ev +22 / allocator +10 等 std::string 临时构造析构）⇒ N89 结构性噪音。
// [b29-SweepSTMonsterScript2] ①ImportMonsterScript 五处 sound 字段补
//   `if (!ScanStr(&token)) return 0;` + `script->m_xxx = token`（ORIG 0x8a13b44..
//   0x8a13dee 五处 `_ZNSsaSERKSs` 目标 +0x6ac/+0x6a8/+0x6a4/+0x69c/+0x6a0）⇒
//   p167 该符号差集 -5→0、OUR calls 1700→1705；②MonsterBaseParameter::clear 与
//   ImportMonsterBaseTable 改二维 m_grid 视图 ⇒ 双件 DIFF→AE（旧 N·P30 判词作废）。
// 四数（check_tu_routed fresh）：803/1909/2/11/508 → 803/1911/2/9/508（零回退）。
//   （两者 >30KB 汇编：骨架+标签分派级复原，覆盖面见各函数注记）
// ============================================================================

#include "STMonsterScript.h"
#include "NpcScript.h"        // STScriptFileList::FindIndexByFullScan

#include <stdio.h>
#include <string.h>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/getDirectoryPath/ScanType/ScanInt
#include "STMapScript.h"     // ENUM_DRAWLAYER + getDrawLayerName 声明

#include "GameEnums.h"       // ENUM_CATEGORY
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/getTagByName
#include "SyncScriptDeps.h"           // toTString
#include "CreatureScript.h"           // getElementName(ENUM_ELEMENT)
#include "GameScriptEnumHelpers.h"    // getCategoryToEnumValue（warp [category of target]）

#include "CEnvironment.h"             // class SpinLock / struct less_TCHAR（ImportMonsterScript 18 张函数局部静态查找表）

#include "STMonsterScript/ScanStrOrInt.inc"
extern char g_szMonsterScriptBaseDirectory[0x100];              // B 0x094ff160
extern std::map<int, std::string> g_stMonsterScriptFileList;    // B 0x094ff260
extern std::vector<std::vector<MonsterBaseParameter> >
    g_monsterBaseParameters;                                    // B 0x094ff278
#include "STMonsterScript/GetLinearDataForMonsterAbility.inc"
#include "STMonsterScript/processModuleTagSet.inc"
#include "STMonsterScript/getScriptFileFullPath.inc"
#include "STMonsterScript/isValidExtension.inc"
#include "STMonsterScript/getObjectCategory.inc"
#include "STMonsterScript/convertBinaryToText.inc"
#include "STMonsterScript/isExistAiPattern.inc"
#include "STMonsterScript/exportAIPatternPath.inc"
#include "STMonsterScript/importAIInfoScript.inc"
extern "C" const char* ACTIVEOBJECT_STATE_STRING[16];           // D 0x093f4d80（CharacterAIA2.cpp）
extern "C" const char* ACTIVESTATUS_ATTACK_TAG[20];             // D 0x093f54e0（CharacterAIA2.cpp）

// ===========================================================================
// Clear —— ORIG T 0x08a398aa（逐条转写，顺序保持汇编原序）
// ===========================================================================
#include "STMonsterScript/Clear.inc"
}

// ===========================================================================
// ctor / dtor / operator=
// ===========================================================================
// ORIG C1 W 0x08368e14：类类型成员逐个默认构造（POD 标量区不动），末尾统一
// call Clear()。此处编译器默认构造 + Clear() 与之语义一致。
#include "STMonsterScript/STMonsterScript.inc"
}

// ORIG D1 W 0x08378dac：成员按声明逆序自动析构，无附加逻辑。
#include "STMonsterScript/dtor_STMonsterScript.inc"
#include "STMonsterScript/STMonsterEquipment.inc"
}

#include "STMonsterScript/clear.inc"
}

// ---- stEnamyDropAvatarInfo_t 成员（ORIG W 0x088cc10c / op= W 0x08366ea8）----
// [b28d-solo-STMonsterScript] 修法（P30 反例解 / P291 具名数组成员视图）：第二循环索引
// `m_field08[i+4]` 在 4.4.6 恒折 `mov i,%eax; lea 0x4(%eax),%r`（this 装载被排到 lea 后），
// 与 ORIG 的 `mov i,%edx; mov this,%eax; lea 0x4(%edx),%ecx` 反序。改走头内匿名 union 的
// 二维视角 `m_field08v[1][i]`（行宽 4 ⇒ 行偏移 add/lea $0x4）后，clear 第二循环与 ORIG
// 逐条全等（探针 /tmp/solo_probe1.cpp clear_v2，4.4.6）。i=4 时 [1][4] 越界至 +0x28 第 9 个
// float —— ORIG 原码即如此写（P171 照抄），内存仍在 union 36B 内。
// ORIG 0x088cc10c
#include "STMonsterScript/clear_1.inc"
}

// [b28d-solo-STMonsterScript] 修法同 clear：ORIG 对两处 a[i+4] 就地 `mov i,%reg;
// add $0x4,%reg`（4.1 复用 i 所在寄存器），4.4 一维形恒 `lea`；二维视角 `m_field08v[1][i]`
// 逐条复现 ORIG add 形（探针 /tmp/solo_probe1.cpp op_v2，4.4.6）。
// ORIG 0x08366ea8
void stEnamyDropAvatarInfo_t::operator=(const stEnamyDropAvatarInfo_t& other)
{
    m_field00 = other.m_field00;
    m_field04 = other.m_field04;
    for (int i = 0; i <= 3; ++i)
        m_field08v[0][i] = other.m_field08v[0][i];
    for (int i = 0; i <= 4; ++i)
        m_field08v[1][i] = other.m_field08v[1][i];
    m_field2c = other.m_field2c;
    m_elements = other.m_elements;   // vector op=
}

// ===========================================================================
// isNamedMonster —— ORIG W 0x088ba18c（全量转写）
// ===========================================================================
#include "STMonsterScript/isNamedMonster.inc"
}

// ===========================================================================
// getMonsterTableIndex —— ORIG T 0x088ba1c8（全量转写）
// ===========================================================================
#include "STMonsterScript/getMonsterTableIndex.inc"
}

// ===========================================================================
// getRandomChampionElement —— ORIG T 0x088b9cce（全量转写）
// ===========================================================================
#include "STMonsterScript/getRandomChampionElement.inc"
}

// ===========================================================================
// ImportMonsterBaseTable —— ORIG T 0x08a3c736（全量转写）
// ===========================================================================
// [b28d-solo-STMonsterScript] 修法：①循环退出改 `if(!ok) break;` + 循环外 return true
// （P164 尾部共享块落点，消 OURS 多出的 `mov $1; jmp`）；②`row > size()` 保持无符号比较
// （ORIG `seta`，原 static_cast<int> 产有符号 `setl`）；③else 分支内 resize 后共享
// `vec[row] = param`（P276 共享调用点，消重复 rep movsl 段）。ORIG 0x08a3c736
// [b29-SweepSTMonsterScript2] 两处列组循环由一维 `m_values[i+12]/[i+16]` 改二维
// `m_grid[3][i]/m_grid[4][i]`（P311/S85）⇒ 复现 ORIG 0x8a3c8a8/0x8a3c8d9 的
// `mov i,%edx; mov this,%eax; add $0xc/$0x10,%edx; mov %eax,-0xe8(%ebp,%edx,4)`，本件 DIFF→AE。
#include "STMonsterScript/ImportMonsterBaseTable.inc"
}

// ===========================================================================
// setBasicAbilityByLevel —— ORIG T 0x088b9603（全量转写）
// ActiveStaticInfo 槽位映射依据现占位头布局：
//   +0x38 elemAttack、+0x58 elemDefense、+0x78 attr17Base、+0x100+8k statBaseK、
//   +0x14 flag14；+0x1a8 float 槽现落 m_statBase27（位型访问【推断】）。
// ===========================================================================
// [b28d-solo-STMonsterScript] 修法（ndiff 424→33，gap=1，p167 25/25 CLEAR）：
// ①`!empty()` → `size() != 0`（P292：ORIG `test %eax,%eax; setne` 为非 bool callee 形）；
// ②四元素抗性表改 for 循环（ORIG 0x203 为循环，此前误作 4 次展开，多 ~28 条）；
// ③**真语义 bug ×2**：ORIG 0x3b7/0x3df 读 param+0x34+4i / param+0x44+4i 即
//   m_value[13+i]/[17+i]，原码误写 [12+i]/[16+i]（差 1 个字段）；
// ④补 `char framePad[0x8]`（P232）使帧 0x64→0x74、bool spill 槽 -0x3c/-0x40→-0x4c/-0x50
//   与 ORIG 精确对齐。
// 残余 N：P209（signed 局部 `if(t==4)` 4.4 恒 mem 直比）、P30/N4（索引 add↔lea 分解）、
// N7/N18a（bool→char 赋值 4.4 多 `mov %eax,%edx` 中转）、P223（循环出口 jmp/nop 岛位次）。
// ORIG 0x088b9603
#include "STMonsterScript/setBasicAbilityByLevel.inc"
}

// ===========================================================================
// ImportMonsterScript —— ORIG T 0x08a12827（0xfb08 字节，~13.8k 指令）。
// 骨架级复原：文件装载/Clear/目录路径/AI 目录传递/模块标签循环与 ORIG 一致；
// 标签分派表按 exportMonsterScript 的 fprintf 格式串反推（导入导出配对），
// 覆盖全部已命名标量/字符串标签；复杂容器标签（throw attack 等）按其元素
// 布局写入主字段。未覆盖的 ORIG 内部分支（17 个 static map 加速表等）以
// 线性比较等价实现 —— 「推断」点见行内注。
// ===========================================================================
namespace {

static const char* const kMonsterActiveStatusTags[17] = {
    "[slow resistance]", "[freeze resistance]", "[poison resistance]",
    "[stun resistance]", "[curse resistance]", "[blind resistance]",
    "[lightning resistance]", "[stone resistance]", "[sleep resistance]",
    "[burn resistance]", "[weapon break resistance]",
    "[bleeding resistance]", "[piercing resistance]", "[stuck resistance]",
    "[confuse resistance]", "[hold resistance]", "[armor break resistance]"
};

#include "STMonsterScript/scanValue.inc"
}

}  // namespace


// [BigMonsterScript 2026-09-09] 13759/13778，gap=−19；p167（1742/1700）：
// 游戏域差集已清零 ——
// ① 补 8 处缺失 ScanInt：ORIG 08a1e041/084/0c7/107/147 是五个 difficulty tag 的
//    **独立分支**（每分支 2×ScanInt(0) 写 -0x1b5c/-0x1b58），原合并条件只发 2 次；
// ② 删 1 处多余 importAIInfoScript（ORIG 仅 0x8a1d30a 一处；case 0xf2e 叶为空）。
// 残余（N 封顶，逐条已取证）：
// ③ +1 ScanStr（我们 46 stack/35 field vs ORIG 50/30：4~5 处 ORIG 先读临时 string，
//    本实现直传字段地址，属翻译形态差）；
// ④ 帧差 0x20 + 全帧槽号平移（N82/N126）；
// ⑤ STL 模板形态差（N130：ORIG C++11 右值 pair `C2*_OT_OT0_` / string `D2` 族，
//    本仓 C++98 头集无对应重载，语言级不可复现）。ORIG 0x08a12827
#include "STMonsterScript/ImportMonsterScript.inc"
}

// [BigMonsterScript 2026-09-09] 6790/6791，gap=−1；p167 CLEAR。
// i486 路由已由 Main 落地（dnf_game_cxx.sh i486 组）：9 处 x87 比较由
// `fucomip/fstp(+fxch)` 翻回 ORIG 的 `fucompp/fnstsw/sahf`；本轮另修 3 处：
// ① 08a27756 目标循环缺 break（原码置位后 `jmp` 循环出口）；
// ② 08a2976b 位测试 else-if 段保留 register 提示（删提示反落槽 +2，P248 边界）；
// ③ 08a29d34 while 判据实读 m_difficultyScales[i][1]（`add $0xcf8`）且比较式
//    写 `x < 0`（探针 g2 复现 `fucompp/test $0x45,%ah/sete`）。
// 残余 −1 = ORIG `mov %ebx,%eax; mov %eax,%edx`（4.1 保留 eax→edx 中转）vs
// 4.4 折单段（N18a）⇒ N 封顶。ORIG 0x08a22520
#include "STMonsterScript/exportMonsterScript.inc"
}
// ===========================================================================
// ORIG 中 STMonsterScript.h 各成员类型的 clear/C1/隐式拷贝均为 W 弱符号，
// 产生点由原 TU 中的实际调用触发；本 TU 无自然调用方（STMonsterScript 仅
// 以 vector 容器持有它们，容器操作不 odr-use 元素值类型），此处补 odr-use
// 以在本 TU 产出与 ORIG 符号集对齐的 W 符号（不改变任何语义/布局）。
// ===========================================================================
namespace
{
#include "STMonsterScript/EmitStMonsterScriptWeakSymbols.inc"
}
}  // namespace
