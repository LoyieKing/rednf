bool CheckTradeLimitItemCountToMove(const Inven_Item& item, bool& ok)
{
    ok = true;
    CItem* proto = G_CDataManager()->find_item(item.m_addInfo);
    if (proto == 0)
        return ok;
    if (proto->is_stackable())
        return ok;
    if (proto->GetAttachType() != 7)
        return ok;
    if (item.GetTradeLimitCount() != 0)
        ok = false;
    return ok;
