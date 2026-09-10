CItem* CItem::GetNeedMaterial() const
{
    CItem* p = NeedMaterialDataManager::getInstance()->getNeedMaterial(this);
    if (*(int*)p != -1)
    {
        return p;
    }
    return (CItem*)&m_pair24;
