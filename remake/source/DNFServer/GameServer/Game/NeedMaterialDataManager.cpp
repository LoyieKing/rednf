// 迁移自 GameStubs.cpp NeedMaterialDataManager 桩（batch-6）。
// getInstance / getNeedMaterial(CItem*) 语义按 ORIG 反汇编转写
// （见 NeedMaterialDataManager.h 布局注释）。

#include "NeedMaterialDataManager.h"
#include "CItem.h"

// [b28c-31] N76：ORIG 0x081b5a4f 先 `lea 0x18(%eax),%edx` 再物化实参（LHS 地址
// 先载），4.4 恒 RHS 先；显式列全成员初始化列表无效 ⇒ 封顶。
NeedMaterialDataManager::NeedMaterialDataManager()
    // [b28c-31] ORIG 0x081b5a4f：map 构造后先 `lea 0x18(%eax),%edx` 再物化实参
    // （P211 LHS 地址先载形）⇒ 显式列全成员试翻装载序。
    : m_map(), m_default(-1, 0)
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
        return (CItem*)&it->second;   // ORIG 用 operator->（pt）
    }
    return (CItem*)&m_default;
}
// ORIG _ZN23NeedMaterialDataManager18setNeedMaterialInfoEiRKSt4pairIiiE @ 0x081b5b46
// m_map[idx]（operator[] 默认构造）后整体赋值 pair。
void NeedMaterialDataManager::setNeedMaterialInfo(int idx,
                                                  const std::pair<int, int>& info)
{
    m_map[idx] = info;
}
