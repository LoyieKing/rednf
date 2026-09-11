NeedMaterialDataManager* NeedMaterialDataManager::getInstance()
{
    static NeedMaterialDataManager inst;
    return &inst;
}