CItem* NeedMaterialDataManager::getNeedMaterial(int index) const
{
    return getNeedMaterial(G_CDataManager()->find_item(index));
}
