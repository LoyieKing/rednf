int CSecu_ProtectionField::CheckItem(CUser* user, SECURITY_PROTCTION type,
                                     const CItem* item)
{
    if (item == 0 || user == 0)
        return 1;
    unsigned long itemIndex = item->get_index();
    return CheckItem(user, type, itemIndex);
