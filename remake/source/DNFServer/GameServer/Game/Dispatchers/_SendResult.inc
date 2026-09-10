void Dispatcher_ModItemAttr::_SendResult(CUser* user, unsigned short resultType,
                                         unsigned short matSlot, unsigned short equipSlot)
{
    Inven_Item item;                                        // ORIG 先 C1 构造（-0x51），再成员拷贝
    item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)matSlot);

    PacketGuard packet;
    packet.put_header(1, 0x54);
    packet.put_byte(1);
    packet.put_short((int)matSlot);
    packet.put_int(item.get_add_info());
    packet.put_short((int)resultType);
    packet.finalize(true);
    user->Send(packet);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)equipSlot);
