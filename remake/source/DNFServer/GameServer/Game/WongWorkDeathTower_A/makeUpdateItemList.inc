// ORIG T 0x0846050e：header(0,0xe)；byte(space)；short(1)；
// inv.MakeItemPacket(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot, *this)；finalize(true)。
void CDeathTower::CPacketHandler::makeUpdateItemList(CUser* user, ENUM_ITEMSPACE space,
                                                     int slot)
{
    // GetInvenTypeFromItemSpace：ORIG 全局 _Z26GetInvenTypeFromItemSpace
    // ENUM_ITEMSPACE；权威声明见 CInventory.h（勿在 WongWork ns 内局部 extern，
    // ORIG 求值序：put_header→put_byte→put_short→GetInvenType→getCurCharacInvenR
    put_header(0, 0xe);
    put_byte((int)space);
    put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(
        (INVEN_TYPE)::GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot, *this);
    finalize(true);
}