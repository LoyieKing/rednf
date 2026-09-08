// ============================================================================
// df_game_r 还原 —— CDataManager 独立方法 TU（Prod_CDataManager 批次）
// ---- b28d-61 判词汇总（本 TU 5 件非 identical，全 N 封顶）----
// set_reward_sp 0x08360bde（P167 CLEAR；N18a/N4 lea-vs-add+寄存器指派）、
// initRandomOptionItem 0x083580aa（P167 CLEAR；N18a make_pair 首 ref lea 折叠）、
// hasSpecialCharacter 0x0836xxxx（P167 MISSING normal_iterator，N89）、
// load_map_files 0x0835fxxx（P167 MISSING Rb_tree_iterator ppEi，N89）、
// initFairPVPSkillPoint 0x0835f1d6（P167 MISSING Rb_tree_iterator×5 + P183 指针缓存）。
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
bool ImportMapScript(STMapScript* script, const char* path);

// importSpPerLevelReferenceTable：ORIG T 0x08910505（脚本导入域实现）。
// stSpPerLevelTable 在 ORIG 中 W ctor/dtor 与空 vector<unsigned int> 一致
// （sizeof 0xc），本 TU 以派生视图获得内联生命周期与继承 operator[]，
// 引用形参产生与 ORIG 一致的 mangled 名 R17stSpPerLevelTable。
struct stSpPerLevelTable : public std::vector<unsigned int> {};
int importSpPerLevelReferenceTable(const char* path, stSpPerLevelTable& table);

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

// ===================== set_reward_sp =====================
// ORIG 0x08360bde：导入 Etc/spTable.etc 至局部 stSpPerLevelTable；
// 导入失败返回 0。成功时将 +0xDA90 表清零后自 index1 起回填表值
// （index0 恒为 0），返回 1。逐次重读 this（与 O0 成员访问一致）。
int CDataManager::set_reward_sp()
{
    stSpPerLevelTable table;
    if (importSpPerLevelReferenceTable("Etc/spTable.etc", table))
    {
        return 0;
    }
    // 残差 N（N18a/N7 族）：两循环 4.4 寄存器指派与 store 序与 4.1 异形
    // （首循环 mov this 先于 mov i、常量拆 0x36a4+disp4；第二循环 ebx=this/
    // esi=i 且表值滞留 eax），int 循环变量探针反致 setle 破坏 setbe，不可翻。
    for (int i = 0; i < 0x60; ++i)
    {
        ((CDataManagerRewardSpView*)this)->m_sp8[0x36a2 + i] = 0;
    }
    for (unsigned long i = 1; i < 0x60; ++i)
    {
        ((CDataManagerRewardSpView*)this)->m_sp8[0x36a2 + i] = table[i];
    }
    return 1;
}

// ===================== initRandomOptionItem =====================
// ORIG 0x083580aa：遍历 +0x4e70 的 map<int,map<int,BaseItemValue>>，
// 对每个内层元素以 make_pair(值, 递增序号) 插入 +0x4e88 反查表；
// 任一插入冲突立即返回 0，全部成功返回 1。（计数器每个外层键重置。）
int CDataManager::initRandomOptionItem()
{
    // b28 BM4 判词（N18a/N4 在册）：fresh 实残 = make_pair 首 ref 实参 ORIG
    // `lea 0x4(%eax),%ecx` 单条折叠 vs 4.4 `add $0x4,%eax; mov %eax,%edx`
    // 两段+中转 hop（+1 行）；P30 穷举 add/lea 与寄存器名不可齐。其余
    // 迭代器/插入流逐条同形（含 ++count 逗号序）。语义等价封顶。
    int count = 0;
    for (std::map<int, std::map<int, BaseItemValue> >::iterator outerIt =
             ((CDataManagerBaseItemView*)this)->m_baseItemInfo.begin();
         outerIt !=
             ((CDataManagerBaseItemView*)this)->m_baseItemInfo.end();
         ++outerIt)
    {
        count = 0;
        // ORIG：++innerIt 先于 ++count 发射 ⇒ ++count 位于 for-third（逗号序）。
        for (std::map<int, BaseItemValue>::iterator innerIt =
                 outerIt->second.begin();
             innerIt != outerIt->second.end(); ++innerIt, ++count)
        {
            if (!((CDataManagerBaseItemView*)this)
                     ->m_lookupIndex.insert(
                         std::make_pair(
                             (unsigned long&)innerIt->second.m_field0,
                             count)).second)
            {
                return 0;
            }
        }
    }
    return 1;
}

// ===================== initFairPVPSkillPoint =====================
// ORIG 0x0835f1d6：
// 1) m_pvpSkillTree.m_map0 非空时取内层最大等级 → m_field48(+0xa848)；
// 2) m_pvpSkillTree.m_map18 非空时取外层最大键 → m_field4c(+0xa84c)；
// 3) m_pvpSkillTree.m_map30 非空时逐 FAIR 条目核算：
//    加技能表累计 SP 总和并建 total[skill]=sp；
//    减技能表扣减 total 对应项（负值告警 9081）、回填 giveSkills(+0x34)、
//    累计差值告警 9104；收尾总额复核告警 9135；点数写 m_pointCost(+0x40)。
//    find_skill 经全局 G_CDataManager()（与 ORIG 调用形态一致），
//    参数序 (job, skillId) 为汇编实证（ebx=+0x00 字节, esi=map key）。
bool CDataManager::initFairPVPSkillPoint()
{
    bool ok = true;
    PvPSkillTreeParameterScript& s = m_pvpSkillTree;

    if (!s.m_map0.empty())
    {
        for (std::map<PvPSkillTreeKey,
                      std::map<int, PvPSkillTreeValue> >::iterator it =
                 s.m_map0.begin();
             it != s.m_map0.end(); ++it)
        {
            for (std::map<int, PvPSkillTreeValue>::iterator lv =
                     it->second.begin();
                 lv != it->second.end(); ++lv)
            {
                if (s.m_field48 < lv->first)
                {
                    s.m_field48 = lv->first;
                }
            }
        }
    }

    if (!s.m_map18.empty())
    {
        for (std::map<int, std::map<int, int> >::iterator it =
                 s.m_map18.begin();
             it != s.m_map18.end(); ++it)
        {
            if (s.m_field4c < it->first)
            {
                s.m_field4c = it->first;
            }
        }
    }

    if (!s.m_map30.empty())
    {
        for (std::map<PvPSkillTreeKey,
                      FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator perIt =
                 s.m_map30.begin();
             perIt != s.m_map30.end(); ++perIt)
        {
            FAIR_PVP_SKILL_TREE_SKILL_PER_POINT& per = perIt->second;

            std::map<int, int> totalSp;
            int sum = 0;
            for (std::map<int, int>::iterator add = per.m_addSkills.begin();
                 add != per.m_addSkills.end(); ++add)
            {
                CSkill* skill =
                    G_CDataManager()->find_skill(
                        perIt->first.m_a, add->first);
                if (skill == 0)
                {
                    continue;
                }
                int sp = skill->get_need_sp_for_level(0, add->second);
                sum += sp;
                totalSp.insert(std::make_pair(add->first, sp));
            }

            int running = 0;
            for (std::map<int, int>::iterator rem = per.m_removeSkills.begin();
                 rem != per.m_removeSkills.end(); ++rem)
            {
                CSkill* skill =
                    G_CDataManager()->find_skill(
                        perIt->first.m_a, rem->first);
                if (skill == 0) {
                    continue;
                }
                int sp = skill->get_need_sp_for_level(0, rem->second);
                sum -= sp;

                std::map<int, int>::iterator found = totalSp.find(rem->first);
                if (found != totalSp.end())
                {
                    found->second -= sp;
                    if (found->second < 0)
                    {
                        cMyTrace trace(
                            __PRETTY_FUNCTION__,
                            0x2379, 0);
                        trace("skill point (-) error");
                        ok = false;
                    }
                }

                per.m_giveSkills.push_back(
                    std::make_pair(rem->first, rem->second));

                int current = 0;
                for (std::map<int, int>::iterator it = totalSp.begin();
                     it != totalSp.end(); ++it)
                {
                    current += it->second;
                }
                if (current + running != sum)
                {
                    running += sum - current;
                    cMyTrace trace(
                        __PRETTY_FUNCTION__, 0x2390,
                        0);
                    trace("skill point diff job(%d), skill(%d)",
                          (int)perIt->first.m_a, rem->first);
                    ok = false;
                }
            }

            int grand = 0;
            for (std::map<int, int>::iterator it = totalSp.begin();
                 it != totalSp.end(); ++it)
            {
                grand += it->second;
            }
            if (grand != sum)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x239f, 0);
                trace("skill point diff");
                ok = false;
            }

            per.m_pointCost = sum;
        }
    }

    return ok ? 1 : 0;
}

// ===================== load_map_files(vector<PVPMAP_DATA>&) =====================
// ORIG 0x0835fe02：遍历 g_stMapScriptFileList（map<int,string>）。
// 键==50000 仅 Clear；否则 ImportMapScript 导入（失败打印并返回 0）。
// 每个文件 new CMap + set_map(拷贝)。m_list4 为空时仅以 (15,15) 注册进
// m_mapList；否则逐 dungeon 下标（find_dungeon 失败 trace 后返回 0）
// 按 m_recoveryObjects 各坐标注册进 dungeon 与 m_mapList；m_dungeonStartArea.m_w>0 时额外 insertStart。
bool CDataManager::load_map_files(std::vector<PVPMAP_DATA>& pvpMaps)
{
    STMapScript script;
    const char* filename = 0;
    for (std::map<int, std::string>::iterator it =
             g_stMapScriptFileList.begin();
         it != g_stMapScriptFileList.end(); ++it)
    {
        filename = it->second.c_str();
        if (filename == 0)
        {
            break;
        }
        if (it->first == 50000)
        {
            script.Clear();
        }
        else if (!ImportMapScript(&script, filename))
        {
            printf("Error in script('%s')\n", filename);
            return 0;
        }

        // ORIG 同样不释放错误路径上的 CMap（忠实转写）。
        CMap* map = new CMap();
        map->set_map(script);

        if (script.m_list4.size() == 0)
        {
            m_mapList->insert_map(15, 15, map, pvpMaps);
            continue;
        }

        for (std::list<int>::iterator stageIt = script.m_list4.begin();
             stageIt != script.m_list4.end(); ++stageIt)
        {
            int stageIdx = *stageIt;
            CDungeon* dungeon = m_dungeonList->find_dungeon(stageIdx);
            if (dungeon == 0)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x254b, 5);
                trace("[Load Map] Not exist dungeon. [Index:%d] filename : [%s]",
                      stageIdx, filename);
                return 0;
            }

            if (script.m_recoveryObjects.size() == 0)
            {
                dungeon->InsertMap(15, 15, map);
                m_mapList->insert_map(15, 15, map, pvpMaps);
                if (script.m_dungeonStartArea.m_w > 0)  /* +0x148 = m_dungeonStartArea.m_w（新 0x1c MapArea 吸收旧独立 int）*/
                {
                    dungeon->insertStart(15, map);
                }
            }

            for (int i = 0; i < static_cast<int>(script.m_recoveryObjects.size()); ++i)
            {
                int x = script.m_recoveryObjects[i].first;
                int y = script.m_recoveryObjects[i].second;
                dungeon->InsertMap(x, y, map);
                m_mapList->insert_map(x, y, map, pvpMaps);
                if (script.m_dungeonStartArea.m_w > 0)  /* +0x148 = m_dungeonStartArea.m_w（新 0x1c MapArea 吸收旧独立 int）*/
                {
                    dungeon->insertStart(x, map);
                }
            }
        }
    }
    return 1;
}

// ===================== hasSpecialCharacter =====================
// ORIG 0x08363140：逐字符扫描（宽字符步进 2）。单字节走 m_invalidChar 的
// uchar 区间表(+0x0c)/单字表(+0x00)；双字节拼 big-endian u16 后走 ushort
// 区间表(+0x24)/单字表(+0x18)。命中即 true；扫完返回 false。
bool CDataManager::hasSpecialCharacter(const char* name)
{
    size_t i = 0;
    while (i < strlen(name))
    {
        const unsigned char* p =
            reinterpret_cast<const unsigned char*>(name + i);
        if (_isOneByteCharacter(p[0]))
        {
            for (std::vector<InvalidCharData::stRangeElement<unsigned char> >::
                     iterator range = m_invalidChar.m_charRange.begin();
                 range != m_invalidChar.m_charRange.end(); ++range)
            {
                if (range->m_min <= p[0] && p[0] <= range->m_max)
                {
                    return true;
                }
            }
            for (std::vector<unsigned char>::iterator ch =
                     m_invalidChar.m_charList.begin();
                 ch != m_invalidChar.m_charList.end(); ++ch)
            {
                if (*ch == p[0])
                {
                    return true;
                }
            }
            i += 1;
        }
        else
        {
            const unsigned short value =
                static_cast<unsigned short>((p[0] << 8) | p[1]);
            for (std::vector<InvalidCharData::stRangeElement<
                     unsigned short> >::iterator range =
                     m_invalidChar.m_shortRange.begin();
                 range != m_invalidChar.m_shortRange.end(); ++range)
            {
                if (range->m_min <= value && value <= range->m_max)
                {
                    return true;
                }
            }
            for (std::vector<unsigned short>::iterator ch =
                     m_invalidChar.m_shortList.begin();
                 ch != m_invalidChar.m_shortList.end(); ++ch)
            {
                if (*ch == value)
                {
                    return true;
                }
            }
            i += 2;
        }
    }
    return false;
}

// ===================== loadAdvanceAltarTimeLineParameter =====================
// ORIG 0x08365010（静态）：reset → importStageTimeLineParameter(
// Etc/AdvanceAltar/AdvanceAltarSummonData.etc)；非 0 打印 Fail(%d)，
// 否则 debugLog；透传返回值。
int CDataManager::loadAdvanceAltarTimeLineParameter(
    advancealtar::StageTimeLineParameter& param)
{
    param.reset();
    int result = param.importStageTimeLineParameter(
        "Etc/AdvanceAltar/AdvanceAltarSummonData.etc");
    if (result != 0)
    {
        printf("importStageTimeLineParameter Fail(%d)\n", result);
        return result;
    }
    param.debugLog();
    return result;
}

// ===================== _isOneByteCharacter =====================
// ORIG 0x08362c0e（_ZN12CDataManager19_isOneByteCharacterEh）：
// sub $0x4,%esp 后直接 mov $0x1,%eax 返回 —— 恒真（ORIG 为单字节恒定实现，
// 双字节分支由 hasSpecialCharacter 的步进 2 路径承担）。
bool CDataManager::_isOneByteCharacter(unsigned char /*c*/)
{
    return true;
}
