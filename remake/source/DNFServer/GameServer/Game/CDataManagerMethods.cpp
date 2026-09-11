// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDataManagerMethods/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"
#include "CItemList.h"   // BaseItemValue（initRandomOptionItem 视图元素类型）
#include "CSkill.h"      // CSkill::get_need_sp_for_level
#include "CMap.h"        // STMapScript / CMap / ImportMapScript 声明域
#include "CDungeon.h"    // CDungeon::InsertMap / insertStart

// ---- 跨 TU 依赖（定义在各自权威 TU；此处仅按 ORIG mangled 引用）----

// g_stMapScriptFileList：ORIG B 0x094fe3a0。定义于 InitScriptProducers_4.cpp。
// ORIG 实型为 std::map<int,std::string>（load_map_files 汇编 Rb_tree 遍历 +
// pair.second(std::string)::c_str 实证；sizeof(std::map)==0x18 与该处
// STScriptFileList 占位同尺寸，布局兼容）。
extern std::map<int, std::string> g_stMapScriptFileList;

#include "CDataManagerMethods/ImportMapScript.inc"

// importSpPerLevelReferenceTable：ORIG T 0x08910505（脚本导入域实现）。
// stSpPerLevelTable 在 ORIG 中 W ctor/dtor 与空 vector<unsigned int> 一致
// （sizeof 0xc），本 TU 以派生视图获得内联生命周期与继承 operator[]，
// 引用形参产生与 ORIG 一致的 mangled 名 R17stSpPerLevelTable。
struct stSpPerLevelTable : public std::vector<unsigned int> {};
#include "CDataManagerMethods/importSpPerLevelReferenceTable.inc"

namespace
{

// initRandomOptionItem 访问视图（与 CItemList.cpp 的 CDataManagerExt 同一
// 证据链：insert_item 写 +0x4e70；initRandomOptionItem 读 +0x4e70/+0x4e88）。
class CDataManagerBaseItemView
{
public:
    char m_pad[0x4e70];
    std::map<int, std::map<int, BaseItemValue> > m_baseItemInfo;  // +0x4e70
    std::map<unsigned long, int> m_lookupIndex;                   // +0x4e88
};

}  // namespace

#include "CDataManagerMethods/set_reward_sp.inc"


#include "CDataManagerMethods/initRandomOptionItem.inc"


#include "CDataManagerMethods/initFairPVPSkillPoint.inc"


#include "CDataManagerMethods/load_map_files.inc"


#include "CDataManagerMethods/hasSpecialCharacter.inc"


#include "CDataManagerMethods/loadAdvanceAltarTimeLineParameter.inc"


#include "CDataManagerMethods/_isOneByteCharacter.inc"

