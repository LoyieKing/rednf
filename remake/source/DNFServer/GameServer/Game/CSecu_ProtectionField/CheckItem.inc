// ORIG 0x0828873a（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTCTIONm）：
// 物品 index 命中受保护集合（0x1f70/0x1f71/0x989a13/0x292090e1）时委托 Check(user,type)，
// 否则直接 0。
int CSecu_ProtectionField::CheckItem(CUser* user, SECURITY_PROTCTION type,
                                     unsigned long itemIndex)
{
    if (itemIndex == 0x1f70 || itemIndex == 0x1f71 ||
        itemIndex == 0x989a13 || itemIndex == 0x292090e1)
    {
        return Check(user, type);
    }
    return 0;
}