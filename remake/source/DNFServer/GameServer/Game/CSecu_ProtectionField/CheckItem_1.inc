// ORIG 0x08288786（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTECTIONPK5CItem）：
// item/user 任一为空则 1，否则取 item->get_index() 走上面的重载。
int CSecu_ProtectionField::CheckItem(CUser* user, SECURITY_PROTCTION type,
                                     const CItem* item)
{
    if (item == 0 || user == 0)
        return 1;
    unsigned long itemIndex = item->get_index();
    return CheckItem(user, type, itemIndex);
}