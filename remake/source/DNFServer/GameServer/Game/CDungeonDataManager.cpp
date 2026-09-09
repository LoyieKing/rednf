// [b28d-38 判词] 本 TU 4 件最终状态：
//  InsertDungeonKey 0x084856ac NEAR（N93 块内对象槽向 node/it 反；p167 余 make_pair 右值形）
//  UpdateSkill 0x08485.. DIFF / UpdateMonster / UpdateCharacter：N107 成员常量偏移取址
//   4.4 恒 add $K,%reg vs ORIG lea K(%eax),%ecx（N107 已列本三函数，六形探针封顶）。
// CDungeonDataManager 批次实现（仅 ORIG 符号，无桥）。
// 证据：docs/class_func_reports/CDungeonDataManager/ 全部 7 份报告
// （InsertDungeonKey/InsertMonsterKey/InsertSkillKey/UpdateCharacter/
//  UpdateMonster/UpdateSkill/UpdateDungeonDataStatistic）。
// 单例 GetInstanceDungeonDataManager T 0x084849b4：P20 函数局部 static。
#include "CDungeonDataManager.h"

#include "SigTypes_B.h"  // SIG_DUNGEON_DATA_STATISTIC 完整定义（键 DungeonKey @ +0x04）

bool CDungeonDataManager::InsertDungeonKey(DungeonKey& key)
{
    std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.find(key);
    if (it != m_dungeonMap.end())
        return false;
    {
        CDungeonNode node;
        m_dungeonMap.insert(std::make_pair(key, node));
    }
    return true;
}

int CDungeonDataManager::InsertMonsterKey(int monsterIndex)
{
    for (std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.begin();
         it != m_dungeonMap.end(); it++)
    {
        CDungeonNode* node = &it->second;
        node->InsertMonsterKey(monsterIndex);
    }
    return 1;
}

CDungeonDataManager& GetInstanceDungeonDataManager()
{
    static CDungeonDataManager instance;
    return instance;
}
int CDungeonDataManager::InsertSkillKey(int skillIndex, int job)
{
    for (std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.begin();
         it != m_dungeonMap.end(); it++)
    {
        CDungeonNode* node = &it->second;
        const DungeonKey& key = it->first;
        node->InsertSkillKey(skillIndex, job,
                             key.m_dungeonIndex, key.m_dungeonDiff);
    }
    return 1;
}

bool CDungeonDataManager::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<DungeonKey, CDungeonNode>::iterator it =
        m_dungeonMap.find((DungeonKey&)stat->m_dungeonIndex);
    if (it == m_dungeonMap.end())
        return false;
    CDungeonNode* node = &it->second;
    node->UpdateCharacter(stat);
    return true;
}

bool CDungeonDataManager::UpdateMonster(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<DungeonKey, CDungeonNode>::iterator it =
        m_dungeonMap.find((DungeonKey&)stat->m_dungeonIndex);
    if (it == m_dungeonMap.end())
        return false;
    CDungeonNode* node = &it->second;
    node->UpdateMonster(stat);
    return true;
}

bool CDungeonDataManager::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<DungeonKey, CDungeonNode>::iterator it =
        m_dungeonMap.find((DungeonKey&)stat->m_dungeonIndex);
    if (it == m_dungeonMap.end())
        return false;
    CDungeonNode* node = &it->second;
    node->UpdateSkill(stat);
    return true;
}

int CDungeonDataManager::UpdateDungeonDataStatistic()
{
    for (std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.begin();
         it != m_dungeonMap.end(); it++)
    {
        CDungeonNode* node = &it->second;
        const DungeonKey& key = it->first;
        node->InsertDB(key);
    }
    return 1;
}

