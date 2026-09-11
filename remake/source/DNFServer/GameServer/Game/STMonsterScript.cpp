#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STMonsterScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
                 bool consume);                                      // T 0x088bd0d5

// ---- 跨 TU 符号（ORIG 地址随注；定义在各自所属批次 TU）----
extern char g_szMonsterScriptBaseDirectory[0x100];              // B 0x094ff160
extern std::map<int, std::string> g_stMonsterScriptFileList;    // B 0x094ff260
extern std::vector<std::vector<MonsterBaseParameter> >
    g_monsterBaseParameters;                                    // B 0x094ff278
#include "STMonsterScript/GetLinearDataForMonsterAbility.inc"
                                   int span);                   // T 0x08891bae
#include "STMonsterScript/processModuleTagSet.inc"
                         int type, bool flag);                  // T 0x08a97637
#include "STMonsterScript/getScriptFileFullPath.inc"
                           const char* dir, const char* name);  // T 0x088bba4e
#include "STMonsterScript/isValidExtension.inc"
std::string getObjectCategoryString(ENUM_CATEGORY cat);         // T 0x08a3ca81（ScriptDepsA.cpp）
#include "STMonsterScript/getObjectCategory.inc"
#include "STMonsterScript/convertBinaryToText.inc"
#include "STMonsterScript/isExistAiPattern.inc"
#include "STMonsterScript/exportAIPatternPath.inc"
#include "STMonsterScript/importAIInfoScript.inc"
                        bool byLevel);                              // T 0x08a390db
extern "C" const char* ACTIVEOBJECT_STATE_STRING[16];           // D 0x093f4d80（CharacterAIA2.cpp）
extern "C" const char* ACTIVESTATUS_ATTACK_TAG[20];             // D 0x093f54e0（CharacterAIA2.cpp）

#include "STMonsterScript/Clear.inc"


#include "STMonsterScript/STMonsterScript.inc"


#include "STMonsterScript/dtor_STMonsterScript.inc"

// ORIG op= W 0x08377994 为编译器合成版（无自赋值检查、rep movsl 分组拷贝、
// 尾部 eax=this），头文件中以 = default 声明，由编译器按成员序生成。

#include "STMonsterScript/STMonsterEquipment.inc"


#include "STMonsterScript/clear.inc"


#include "STMonsterScript/clear_1.inc"


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

#include "STMonsterScript/isNamedMonster.inc"


#include "STMonsterScript/getMonsterTableIndex.inc"


#include "STMonsterScript/getRandomChampionElement.inc"


#include "STMonsterScript/ImportMonsterBaseTable.inc"


#include "STMonsterScript/setBasicAbilityByLevel.inc"


// ===========================================================================
// ImportMonsterScript —— ORIG T 0x08a12827（0xfb08 字节，~13.8k 指令）。
// 骨架级复原：文件装载/Clear/目录路径/AI 目录传递/模块标签循环与 ORIG 一致；
// 标签分派表按 exportMonsterScript 的 fprintf 格式串反推（导入导出配对），
// 覆盖全部已命名标量/字符串标签；复杂容器标签（throw attack 等）按其元素
// 布局写入主字段。未覆盖的 ORIG 内部分支（17 个 static map 加速表等）以
// 线性比较等价实现 —— 「推断」点见行内注。
// ===========================================================================
namespace {

// [SymActStatus·2026-09-11 引用点闭环] 原 `kMonsterActiveStatusTags[17]`（本命名空间
//   内拷贝）已删除 ⇒ ImportMonsterScript 的 17 项扫描改用 ORIG 真表
//   `ACTIVESTATUS_TAG`（文件级唯一定义，ORIG 0x093f5480；ORIG 引用点 0x08a221fb）。
// [GiantA-R8 only-NEW 清理] 原匿名命名空间 `int scanValue()`（`_ZN12_GLOBAL__N_19scanValueEv`，
//   20B）ORIG 全库无同名、全 TU **零调用点**（grep 仅命中 include 行）⇒ 删除 include + 函数体
//   （消 only-NEW 1 条；该函数仅 `return ScanInt(0)`，删除无符号发射连带）。
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
// [W16C 符号名轴·2026-09-11] ORIG 0x093f5640 `PARAMETER_CATEGORY_NAME`（D 段、全局
//   **非 static**、size 0x38 = 14 指针）：原 `s_catName`（ImportMonsterScript 内 static）
//   与 `s_abilityCategoryName`（exportMonsterScript 内 static）是该表的两份发明拷贝
//   ⇒ 合并为单一全局表，名字/作用域/段型与 ORIG 对齐（OUR-only −2；四数逐项不变）。
const char* PARAMETER_CATEGORY_NAME[14] = {
    "[ATTACK_SPEED]", "[MOVE_SPEED]", "[PHYSICAL_ATTACK]",
    "[PHYSICAL_DEFENSE]", "[MAGICAL_ATTACK]", "[MAGICAL_DEFENSE]",
    "[EQUIPMENT_PHYSICAL_ATTACK]", "[EQUIPMENT_PHYSICAL_DEFENSE]",
    "[EQUIPMENT_MAGICAL_ATTACK]", "[EQUIPMENT_MAGICAL_DEFENSE]",
    "[HP MAX]", "[HIT_RECOVERY]", "[STUCK]", "[HP MAX BOSS]"
};

// [SymActStatus·2026-09-11 跨 TU 合并] ORIG 唯一一份 `_ZL16ACTIVESTATUS_TAG`
//   @0x093f5480（d 段 / size 0x44 = 17 指针）。其 4 个引用点 0x08a0fa5d /
//   0x08a0fdc2（ImportCharacterScript / exportCharacterStatusScript）与
//   0x08a221fb / 0x08a22e1f（ImportMonsterScript / exportMonsterScript）都能解析
//   到这一个 local 符号 ⇒ ORIG 侧四函数同属一个 TU（local 符号不可跨 .o）。
//   我方此前 ScriptDepsA.cpp 的 `static const char* const ACTIVESTATUS_TAG` 与
//   exportMonsterScript 内的 `s_linearAbilityName` 是该表的两份同内容拷贝
//   ⇒ 合并为本处唯一定义：外部链接 + 去外层 const ⇒ .data（段 d）/ 0x44，
//   与 ORIG 段型一致；ScriptDepsA.cpp 侧改 extern 引用（消费点形不变）。
//   注：跨 TU 引用 internal linkage 不可能 ⇒ 符号名以 `_Z16ACTIVESTATUS_TAG`
//   取代 ORIG 的 `_ZL16ACTIVESTATUS_TAG`（binding 差异不可消，段/size 已对齐）。
const char* ACTIVESTATUS_TAG[17] = {
    "[slow resistance]", "[freeze resistance]", "[poison resistance]",
    "[stun resistance]", "[curse resistance]", "[blind resistance]",
    "[lightning resistance]", "[stone resistance]", "[sleep resistance]",
    "[burn resistance]", "[weapon break resistance]", "[bleeding resistance]",
    "[piercing resistance]", "[stuck resistance]", "[confuse resistance]",
    "[hold resistance]", "[armor break resistance]"
};

// ORIG `ACTIVESTATUS_SHAKABLE` D 0x0936e494 [0x12]（.data 初值实证：
// 00 01 00 01 00 00 00 01 00 00 00 00 00 00 00 00 00 00；紧邻 ACTIVESTATUS 域
// 常数 0x0936e484..0x0936e491 之后、ACTIVESTATUS_DURATION B 0x093f58a0 之前）；
// ORIG 全库 0 引用（死数据）⇒ 纯符号轴补发射，零消费点、零 codegen 影响。
// [Main·PoolH 数据符号普查 2026-09-11] 段/大小与 ORIG 对齐（.data / 0x12）。
unsigned char ACTIVESTATUS_SHAKABLE[18] = {
    0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#include "STMonsterScript/ImportMonsterScript.inc"


#include "STMonsterScript/exportMonsterScript.inc"

// ===========================================================================
// ORIG 中 STMonsterScript.h 各成员类型的 clear/C1/隐式拷贝均为 W 弱符号，
// 产生点由原 TU 中的实际调用触发；本 TU 无自然调用方（STMonsterScript 仅
// 以 vector 容器持有它们，容器操作不 odr-use 元素值类型），此处补 odr-use
// 以在本 TU 产出与 ORIG 符号集对齐的 W 符号（不改变任何语义/布局）。
// ===========================================================================
namespace
{
#include "STMonsterScript/EmitStMonsterScriptWeakSymbols.inc"

}  // namespace
