// ORIG T 0x081b5bf2 _ZNK23NeedMaterialDataManager15getNeedMaterialEi：
// getNeedMaterial(G_CDataManager()->find_item(i))。
CItem* NeedMaterialDataManager::getNeedMaterial(int index) const
{
    return getNeedMaterial(G_CDataManager()->find_item(index));
}
