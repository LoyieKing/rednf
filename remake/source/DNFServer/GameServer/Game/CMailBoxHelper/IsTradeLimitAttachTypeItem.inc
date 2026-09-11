// ORIG 0x081201dc：find_item(item.m_addInfo)；为空或非 stackable → false；
// 否则 GetAttachType()==7。
bool IsTradeLimitAttachTypeItem(Inven_Item const& item)
{
    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found == 0)
        return false;
    if (!found->is_stackable())
        return false;
    CItem* it = found;
    return it->GetAttachType() == 7;
}