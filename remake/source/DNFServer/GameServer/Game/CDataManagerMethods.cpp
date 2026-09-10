// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDataManagerMethods/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CDataManager 独立方法 TU（Prod_CDataManager 批次）
// ---- b28d-solo-CDataManagerMethods 判词汇总（本 TU 5 件复核后状态）----
// hasSpecialCharacter 0x08363140 → IDENTICAL（修法：const_iterator+缓存 end、
//   2 字节 union 视图拼 big-endian、for+else 内 ++i、r.m_max>=p[0] 比较方向）；
// initFairPVPSkillPoint 0x0835f1d6 → IDENTICAL（修法：删引用局部 s、展开
//   perIt->second、push_back 改 pair 直构、continue 改正向包裹、cMyTrace 改
//   P221 全表达式临时链）；
// load_map_files 0x0835fe02 → NEAR（修法：dungeon/map 提升函数级、迭代器先声明
//   后赋值、两处后置 it++、pos 整体赋值；残差全帧槽号平移 0x10 = N82/P112）；
// set_reward_sp 0x08360bde → DIFF 封 N76/N94（P167 CLEAR；两循环 4.4 恒 LHS-first
//   「先装 this 再求下标」，ORIG 4.1 为 RHS-first+`mov %eax,%edx` 中转；探针
//   f_int/h_int 证明成员函数下类型异型化不翻，寄存器指派亦不可齐）；
// initRandomOptionItem 0x083580aa → DIFF 封 N107/N18a（P167 CLEAR；唯一残差
//   `&innerIt->second.m_field0` ORIG `lea 0x4(%eax),%ecx` vs 4.4 恒
//   `add $0x4,%eax; mov %eax,%edx`；六形探针含字段 unsigned long 化全产 add）。
// 仅承载以下六个真实 ORIG 符号，逐指令对照
// docs/class_func_reports/CDataManager/ 各报告转写：
//   set_reward_sp                              _ZN12CDataManager13set_reward_spEv        0x08360bde
//   initRandomOptionItem                       _ZN12CDataManager20initRandomOptionItemEv 0x083580aa
//   initFairPVPSkillPoint                      _ZN12CDataManager21initFairPVPSkillPointEv 0x0835f1d6
//   load_map_files(vector<PVPMAP_DATA>&)       _ZN12CDataManager14load_map_filesERSt6vectorI11PVPMAP_DATASaIS1_EE 0x0835fe02
//   hasSpecialCharacter                        _ZN12CDataManager19hasSpecialCharacterEPKc 0x08363140
//   loadAdvanceAltarTimeLineParameter          _ZN12CDataManager33loadAdvanceAltarTimeLineParameterERN12advancealtar22StageTimeLineParameterE 0x08365010
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

// ImportMapScript：ORIG T 0x089dcf54，定义于 STMapScript.cpp。
#include "CDataManagerMethods/ImportMapScript.inc"
struct stSpPerLevelTable : public std::vector<unsigned int> {};
#include "CDataManagerMethods/importSpPerLevelReferenceTable.inc"
class CDataManagerBaseItemView
{
public:
    char m_pad[0x4e70];
    std::map<int, std::map<int, BaseItemValue> > m_baseItemInfo;  // +0x4e70
    std::map<unsigned long, int> m_lookupIndex;                   // +0x4e88
};

}  // namespace

// ===================== set_reward_sp =====================
// [b28d-solo-CDataManagerMethods] DIFF 封 N76/N94：P167 CLEAR（6/6）。
// ORIG 两循环均为「先求下标/RHS、后装 this」（第二循环值经 eax→edx 中转）；
// 4.4 成员函数恒 LHS-first（探针 f_int/f_uint/h_int/h_zero：类型异型化不翻），
// 第一循环仅 mov 装载序差、第二循环求值序差 ⇒ 语义等价封顶。
// ORIG 0x08360bde：导入 Etc/spTable.etc 至局部 stSpPerLevelTable；
// 导入失败返回 0。成功时将 +0xDA90 表清零后自 index1 起回填表值
// （index0 恒为 0），返回 1。逐次重读 this（与 O0 成员访问一致）。
#include "CDataManagerMethods/set_reward_sp.inc"
}

// ===================== initRandomOptionItem =====================
// [b28d-solo-CDataManagerMethods] DIFF 封 N107/N18a：P167 CLEAR（14/14）。
// 唯一残差 = `&innerIt->second.m_field0`：ORIG `lea 0x4(%eax),%ecx` 单条，
// 4.4 恒 `add $0x4,%eax; mov %eax,%edx` 两段；探针（引用/解引用/指针算术/
// 字段 unsigned long 化）全产 add ⇒ 源形不可择。
// ORIG 0x083580aa：遍历 +0x4e70 的 map<int,map<int,BaseItemValue>>，
// 对每个内层元素以 make_pair(值, 递增序号) 插入 +0x4e88 反查表；
// 任一插入冲突立即返回 0，全部成功返回 1。（计数器每个外层键重置。）
#include "CDataManagerMethods/initRandomOptionItem.inc"
}

// ===================== initFairPVPSkillPoint =====================
// [b28d-solo-CDataManagerMethods] IDENTICAL（orig=506 our=506 ndiff=0）。
// 关键修法：删 `PvPSkillTreeParameterScript& s` 引用局部（P0 表达式直用）、
// perIt->second 逐使用点展开（补 5 处 operator-> 调用点）、push_back 改
// `std::pair<int,int>(rem->first, rem->second)` 直构（P192）、两处 continue
// 改正向 if 包裹、三处 cMyTrace 改 P221 全表达式临时链翻槽序。
// ORIG 0x0835f1d6：
// 1) m_pvpSkillTree.m_map0 非空时取内层最大等级 → m_field48(+0xa848)；
// 2) m_pvpSkillTree.m_map18 非空时取外层最大键 → m_field4c(+0xa84c)；
// 3) m_pvpSkillTree.m_map30 非空时逐 FAIR 条目核算：
//    加技能表累计 SP 总和并建 total[skill]=sp；
//    减技能表扣减 total 对应项（负值告警 9081）、回填 giveSkills(+0x34)、
//    累计差值告警 9104；收尾总额复核告警 9135；点数写 m_pointCost(+0x40)。
//    find_skill 经全局 G_CDataManager()（与 ORIG 调用形态一致），
//    参数序 (job, skillId) 为汇编实证（ebx=+0x00 字节, esi=map key）。
#include "CDataManagerMethods/initFairPVPSkillPoint.inc"
}

// ===================== load_map_files(vector<PVPMAP_DATA>&) =====================
// [b28d-solo-CDataManagerMethods] NEAR（orig=316 our=316 gap=0，P167 CLEAR 44/44）。
// 关键修法：dungeon/map 提升为函数级具名局部、it/stageIt 先声明后赋值
// （P50 复活）、两处后置 `it++`（ORIG 083416e4=ppEi）、pos 改 pair 整体赋值
// （单次 operator[]）。残差 = 全帧槽号平移 0x10（4.4 把 set_map 按值参数临时
// 放浅、具名局部随之浅移）⇒ N82/P112 槽分配族封顶。
// ORIG 0x0835fe02：遍历 g_stMapScriptFileList（map<int,string>）。
// 键==50000 仅 Clear；否则 ImportMapScript 导入（失败打印并返回 0）。
// 每个文件 new CMap + set_map(拷贝)。m_list4 为空时仅以 (15,15) 注册进
// m_mapList；否则逐 dungeon 下标（find_dungeon 失败 trace 后返回 0）
// 按 m_recoveryObjects 各坐标注册进 dungeon 与 m_mapList；m_dungeonStartArea.m_w>0 时额外 insertStart。
#include "CDataManagerMethods/load_map_files.inc"
}

// ===================== hasSpecialCharacter =====================
// [b28d-solo-CDataManagerMethods] IDENTICAL（orig=235 our=235 ndiff=0）。
// 关键修法：四循环改 const_iterator + for-init 缓存 end（P96/P13）、双字节值
// 改 2 字节 union 视图逐字节写（movw $0 + 两个 movb + movzwl）、while 改
// for+else 内 ++i（复现两条 addl $1）、比较方向 `r.m_max >= p[0]`（翻 jb）。
// ORIG 0x08363140：逐字符扫描（宽字符步进 2）。单字节走 m_invalidChar 的
// uchar 区间表(+0x0c)/单字表(+0x00)；双字节拼 big-endian u16 后走 ushort
// 区间表(+0x24)/单字表(+0x18)。命中即 true；扫完返回 false。
#include "CDataManagerMethods/hasSpecialCharacter.inc"
}

// ===================== loadAdvanceAltarTimeLineParameter =====================
// ORIG 0x08365010（静态）：reset → importStageTimeLineParameter(
// Etc/AdvanceAltar/AdvanceAltarSummonData.etc)；非 0 打印 Fail(%d)，
// 否则 debugLog；透传返回值。
#include "CDataManagerMethods/loadAdvanceAltarTimeLineParameter.inc"
}

// ===================== _isOneByteCharacter =====================
// ORIG 0x08362c0e（_ZN12CDataManager19_isOneByteCharacterEh）：
// sub $0x4,%esp 后直接 mov $0x1,%eax 返回 —— 恒真（ORIG 为单字节恒定实现，
// 双字节分支由 hasSpecialCharacter 的步进 2 路径承担）。
#include "CDataManagerMethods/_isOneByteCharacter.inc"
}
