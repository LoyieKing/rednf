// ===== Dispatcher_GuildCargoPushItem::check_error —— ORIG 0x081d0f64 =====
// ORIG（3 参数符号；字段自 ParamBase 读）：
//   state!=3 或 getCurCharacR()==0 → -1；guildkey==0 → 100；IsExistGuildAgit()!=1 → 0xb5；
//   invenType=GetInvenTypeFromItemSpace(param+0xd)；slot=param+0xe(ushort)；
//   GetInvenSlot(inven, invenType, slot)；Secu Check(user,8) 非 0 → 透传；
//   CheckItemLock(invenType,slot) → 0xd5；param+0x10==0 或 != 槽内 m_addInfo → 0xc5；
//   find_item(m_addInfo)==0 → 0xc5；通过 → 0。
// ===== Dispatcher_ModItemAttr::_SendResult —— ORIG 0x08201938 =====
// ORIG 参数映射：(user, param_2=resultType, param_3=matSlot, param_4=equipSlot)。
// 包：header(1,0x54) | byte(1) | short(matSlot) | int(matSlot 槽 get_add_info()) |
//     short(resultType) → finalize(true) → CUser::Send → SendUpdateItemList(1,0,equipSlot)。
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
}