// ORIG 0x0868f6bc (T)：find_item(itemIdx)→verifyStackableItemProcess
bool CUser::verifyStackableItemActionPlace(StackableAction::Type type, unsigned int itemIdx)   // [b28d-Main·P292] int→bool
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    return verifyStackableItemProcess(type, item);
}