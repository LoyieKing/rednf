// 迁移自 GameStubs.cpp NeedMaterialDataManager 桩（batch-6）。
// getInstance / getNeedMaterial(CItem*) 语义按 ORIG 反汇编转写
// （见 NeedMaterialDataManager.h 布局注释）。

#include "NeedMaterialDataManager.h"
#include "CItem.h"

NeedMaterialDataManager::NeedMaterialDataManager()
    : m_default(-1, 0)
{
}

NeedMaterialDataManager::~NeedMaterialDataManager()
{
}
NeedMaterialDataManager* NeedMaterialDataManager::getInstance()
{
    static NeedMaterialDataManager inst;
    return &inst;
}

CItem* NeedMaterialDataManager::getNeedMaterial(const CItem* item) const
{
    if (item == 0)
    {
        return (CItem*)&m_default;
    }
    int index = item->get_index();
    std::map<int, std::pair<int, int> >::const_iterator it = m_map.find(index);
    if (it != m_map.end())
    {
        return (CItem*)&(*it).second;
    }
    return (CItem*)&m_default;
}