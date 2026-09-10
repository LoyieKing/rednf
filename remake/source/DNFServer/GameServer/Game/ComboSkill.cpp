// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ComboSkill/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN10ComboSkill9resetTreeE20ENUM_SKILL_TREE_KIND`（ORIG 0x80ea3c7）。
#include <iostream>


#include <utility>

#include "SkillSlot.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"



// G_CDataManager() 返回对象 +0x14 处为 char*（组合技列表基址），成员访问形态
// 与 ORIG 一致（mov 0x14(%eax),%eax；而非 add+mov）。
struct CDataManagerComboLayout
{
    char m_pad[0x14];
    char* m_pComboList;  // +0x14
};

#include "ComboSkill/resetTree.inc"
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp", __PRETTY_FUNCTION__,
            0xd, "ComboSkill::resetTree kind(%d) error", kind);
    }
    else
    {
        ComboSkillMap& curMap = getComboSkillMap(kind);
        for (ComboSkillMap::iterator it = curMap.begin(); it != curMap.end();
             ++it)
        {
            for (unsigned int i = 0; i < it->second.size() && i < 6; ++i)
            {
                it->second[i] = 0;
            }
        }
    }
}

#include "ComboSkill/ComboSkill.inc"
{
    std::vector<std::pair<int, int> >* pComboSkillList =
        (std::vector<std::pair<int, int> >*)(
            ((CDataManagerComboLayout*)G_CDataManager())->m_pComboList + 0x48fc);
    std::vector<int> vecSkill;
    vecSkill.resize(6);
    for (int i = 0; i < 6; ++i)
    {
        m_comboSkillMap.insert(
            std::make_pair((*pComboSkillList)[i].first, vecSkill));
        m_comboSkillSfpMap.insert(
            std::make_pair((*pComboSkillList)[i].first, vecSkill));
    }
}

#include "ComboSkill/ComboSkill_1.inc"
    : m_comboSkillMap(other.m_comboSkillMap),
      m_comboSkillSfpMap(other.m_comboSkillSfpMap)
{
}

#include "ComboSkill/dtor_ComboSkill.inc"
{
}

ComboSkill& ComboSkill::operator=(const ComboSkill& other)
{
    m_comboSkillMap = other.m_comboSkillMap;
    m_comboSkillSfpMap = other.m_comboSkillSfpMap;
    return *this;
}

#include "ComboSkill/convertComboSkillMap.inc"
                                      const comoboSkillData* data,
                                      ENUM_SKILL_TREE_KIND kind)
{
    if (data == 0)
    {
        return;
    }
    if (count > 6)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0x25, "ComboSkill::setComboSkill count value(%d) error",
            (int)count);
    }
    else
    {
        if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
        {
            LogManager::logFormat(
                1, "ComboSkill.cpp",
                __PRETTY_FUNCTION__,
                0x2b, "ComboSkill::setComboSkill kind(%d) error", kind);
        }
        else
        {
            ComboSkillMap& curMap = getComboSkillMap(kind);
            ComboSkillMap::iterator findIt = curMap.end();
            std::vector<int> vecSkill;
            for (int i = 0; i < (int)count && i < 6; ++i)
            {
                findIt = curMap.find(data[i].m_comboSkillIndex);
                if (findIt == curMap.end())
                {
                    continue;
                }
                else
                {
                    vecSkill.clear();
                    for (int j = 0; j < 6; ++j)
                    {
                        vecSkill.push_back((int)data[i].m_comboSkillList[j]);
                    }
                    curMap[data[i].m_comboSkillIndex] = vecSkill;
                }
            }
        }
    }
}

#include "ComboSkill/convertComboSkillArray.inc"
                                        comoboSkillData* data,
                                        ENUM_SKILL_TREE_KIND kind) const
{
    if (data == 0)
    {
        return;
    }
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0x4b, "ComboSkill::convertComboSkillArray kind(%d) error", kind);
    }
    else
    {
        const ComboSkillMap& curMap = getComboSkillMap(kind);
        ComboSkillMap::const_iterator it = curMap.begin();
        unsigned int index = 0;
        for (; it != curMap.end(); ++it)
        {
            data[index].m_comboSkillIndex = it->first;
            for (unsigned int j = 0; j < it->second.size() && j < 6; ++j)
            {
                data[index].m_comboSkillList[j] = (short)it->second[j];
            }
            ++index;
        }
        count = (unsigned char)index;
    }
}

#include "ComboSkill/loadData.inc"
{
    if (data == 0)
    {
        return 0;
    }
    if (data->m_tree1Count != 0)
    {
        convertComboSkillMap(data->m_tree1Count, data->m_tree1Data,
                             SKILL_TREE_1);
    }
    if (data->m_tree2Count != 0)
    {
        convertComboSkillMap(data->m_tree2Count, data->m_tree2Data,
                             SKILL_TREE_2);
    }
    return 1;
}

#include "ComboSkill/saveData.inc"
{
    if (data == 0)
    {
        return 0;
    }
    convertComboSkillArray(data->m_tree1Count, data->m_tree1Data,
                           SKILL_TREE_1);
    convertComboSkillArray(data->m_tree2Count, data->m_tree2Data,
                           SKILL_TREE_2);
    return 1;
}

#include "ComboSkill/getComboSkillSendList.inc"
{
    getComboSkillSendForm(packet, SKILL_TREE_1);
    getComboSkillSendForm(packet, SKILL_TREE_2);
}

#include "ComboSkill/getComboSkillSendForm.inc"
                                       ENUM_SKILL_TREE_KIND kind) const
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0x99, "ComboSkill::getComboSkillSendForm kind(%d) error", kind);
    }
    else
    {
        const ComboSkillMap& curMap = getComboSkillMap(kind);
        ComboSkillMap::const_iterator it = curMap.begin();
        packet.put_byte((int)kind);
        packet.put_byte((int)curMap.size());
        for (; it != curMap.end(); ++it)
        {
            packet.put_short((int)it->first);
            packet.put_byte((int)it->second.size());
            for (unsigned int i = 0; i < it->second.size() && i < 6; ++i)
            {
                packet.put_short(it->second[i]);
            }
        }
    }
}

#include "ComboSkill/setComboSkill.inc"
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0xb5, "ComboSkill::setComboSkill kind(%d) error", kind);
    }
    else
    {
        if (map.empty())
        {
            return;
        }
        else
        {
            ComboSkillMap& curMap = getComboSkillMap(kind);
            ComboSkillMap::iterator findIt = curMap.end();
            ComboSkillMap::iterator it = map.begin();
            resetTree(kind);
            while (it != map.end())
            {
                findIt = curMap.find(it->first);
                if (findIt == curMap.end())
                {
                    continue;
                }
                else
                {
                    for (unsigned int i = 0; i < it->second.size() && i < 6; ++i)
                    {
                        findIt->second[i] = it->second[i];
                    }
                }
                ++it;
            }
        }
    }
}

#include "ComboSkill/eraseDuplicationComboSKill.inc"
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0xd4, "ComboSkill::eraseDuplicationComboSKill kind(%d) error", kind);
    }
    else
    {
        ComboSkillMap& curMap = getComboSkillMap(kind);
        for (ComboSkillMap::iterator it = curMap.begin(); it != curMap.end();
             ++it)
        {
            for (std::vector<int>::iterator vit = it->second.begin();
                 vit != it->second.end(); ++vit)
            {
                if (*vit == slot)
                {
                    it->second.erase(vit);
                    it->second.push_back(0);
                }
            }
        }
    }
}
