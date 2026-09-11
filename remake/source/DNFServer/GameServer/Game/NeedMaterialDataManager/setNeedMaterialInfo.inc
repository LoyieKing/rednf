// ORIG _ZN23NeedMaterialDataManager18setNeedMaterialInfoEiRKSt4pairIiiE @ 0x081b5b46
// m_map[idx]（operator[] 默认构造）后整体赋值 pair。
void NeedMaterialDataManager::setNeedMaterialInfo(int idx,
                                                  const std::pair<int, int>& info)
{
    m_map[idx] = info;
}