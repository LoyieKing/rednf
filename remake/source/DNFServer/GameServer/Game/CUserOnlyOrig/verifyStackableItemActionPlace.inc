// ---------------------------------------------------------------------------
// 47. CUser::verifyStackableItemActionPlace @0x0868f69a(纯尾调)
// ---------------------------------------------------------------------------
bool CUser::verifyStackableItemActionPlace(StackableAction::Type type, const CItem* item)   // [b28d-Main·P292] int→bool
{
    return verifyStackableItemProcess(type, item);
}