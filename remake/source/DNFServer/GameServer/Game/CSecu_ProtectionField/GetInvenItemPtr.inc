// ORIG T 0x082886ae _ZN21CSecu_ProtectionField15GetInvenItemPtrEP5CUsercs：
// space==2 → 仓库槽引用；space∈{0,3,7} → 背包槽引用；其余返回空。
Inven_Item* CSecu_ProtectionField::GetInvenItemPtr(CUser* user, char space,
                                                   short slot)
{
    // ORIG T 0x082886ae（报告 GetInvenItemPtr.md）：switch 树状展开。
    switch (space)
    {
    case 0:
    case 3:
    case 7:
        return (Inven_Item*)user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)space), (int)slot);
    case 2:
        return user->getCurCharacCargoR()->GetCargoRef((int)slot);
    }
    return 0;
}