// ============================================================================
// df_game_r 还原 —— ComboSkill（G1-2 技能域，组合技）
// 参照 docs/class_func_reports/ComboSkill.md + ORIG 反汇编。
// 布局：+0x00 std::map<short, vector<int>> m_comboSkillMap（0x15）
//       +0x15 std::map<short, vector<int>> m_comboSkillSfpMap（0x15）
//       sizeof = 0x2a；SkillSlot 内嵌于 +0x1c。
// 默认构造从 G_CDataManager()+0x14 指向的 vector<pair<int,int>>（+0x48fc）
// 预填 6 组组合技映射；saveData/loadData 按 SIG_COMBO_SKILL 的 pack(1) 布局
// 在 +0x0a/+0x5e 存取两棵树（comoboSkillData 0xe 字节/条）。
// ============================================================================

#include <utility>

#include "SkillSlot.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"

// ---- PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char）----
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// G_CDataManager() 返回对象 +0x14 处为 char*（组合技列表基址），成员访问形态
// 与 ORIG 一致（mov 0x14(%eax),%eax；而非 add+mov）。
struct CDataManagerComboLayout
{
    char m_pad[0x14];
    char* m_pComboList;  // +0x14
};

ComboSkill::ComboSkill()
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

ComboSkill::ComboSkill(const ComboSkill& other)
    : m_comboSkillMap(other.m_comboSkillMap),
      m_comboSkillSfpMap(other.m_comboSkillSfpMap)
{
}

ComboSkill::~ComboSkill()
{
}

ComboSkill& ComboSkill::operator=(const ComboSkill& other)
{
    m_comboSkillMap = other.m_comboSkillMap;
    m_comboSkillSfpMap = other.m_comboSkillSfpMap;
    return *this;
}

void ComboSkill::resetTree(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp", "void ComboSkill::resetTree(ENUM_SKILL_TREE_KIND)",
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

void ComboSkill::convertComboSkillMap(unsigned char count,
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
            "void ComboSkill::convertComboSkillMap(unsigned char, const comoboSkillData*, ENUM_SKILL_TREE_KIND)",
            0x25, "ComboSkill::setComboSkill count value(%d) error",
            (int)count);
    }
    else
    {
        if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
        {
            LogManager::logFormat(
                1, "ComboSkill.cpp",
                "void ComboSkill::convertComboSkillMap(unsigned char, const comoboSkillData*, ENUM_SKILL_TREE_KIND)",
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

void ComboSkill::convertComboSkillArray(unsigned char& count,
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
            "void ComboSkill::convertComboSkillArray(unsigned char&, comoboSkillData*, ENUM_SKILL_TREE_KIND) const",
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

char ComboSkill::loadData(SIG_COMBO_SKILL* data)
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

char ComboSkill::saveData(SIG_COMBO_SKILL* data) const
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

void ComboSkill::getComboSkillSendList(PacketGuard& packet) const
{
    getComboSkillSendForm(packet, SKILL_TREE_1);
    getComboSkillSendForm(packet, SKILL_TREE_2);
}

void ComboSkill::getComboSkillSendForm(PacketGuard& packet,
                                       ENUM_SKILL_TREE_KIND kind) const
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            "void ComboSkill::getComboSkillSendForm(PacketGuard&, ENUM_SKILL_TREE_KIND) const",
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

void ComboSkill::setComboSkill(ENUM_SKILL_TREE_KIND kind, ComboSkillMap& map)
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            "void ComboSkill::setComboSkill(ENUM_SKILL_TREE_KIND, ComboSkillMap&)",
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

void ComboSkill::eraseDuplicationComboSKill(ENUM_SKILL_TREE_KIND kind, int slot)
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            "void ComboSkill::eraseDuplicationComboSKill(ENUM_SKILL_TREE_KIND, int)",
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
