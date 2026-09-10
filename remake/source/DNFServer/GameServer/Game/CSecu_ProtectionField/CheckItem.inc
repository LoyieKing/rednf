int CSecu_ProtectionField::CheckItem(CUser* user, SECURITY_PROTCTION type,
                                     unsigned long itemIndex)
{
    if (itemIndex == 0x1f70 || itemIndex == 0x1f71 ||
        itemIndex == 0x989a13 || itemIndex == 0x292090e1)
    {
        return Check(user, type);
    }
    return 0;
