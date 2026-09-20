void CTitleBook::_sendCategoryData(CUser* user, unsigned short extra,
                                   ENUM_TITLE_BOOK_CATEGORY category,
                                   ENUM_TITLE_BOOK_INFO_TYPE type)
{
    ENUM_TITLE_BOOK_CATEGORY ownerCategory = (ENUM_TITLE_BOOK_CATEGORY)4;
    int ownerSlot = 0;
    int count = 0;
    Inven_Item ownerItem;
    ownerItem.reset();
    char hasOwner = 0;

    PacketGuard packet;
    packet.put_header(0, 0x165);
    packet.put_byte((signed char)type);
    packet.put_short((int)extra);
    packet.put_int((int)category);
    count = _getCategoryItemCount(category);
    if (extra != 0)
    {
        if (_getOwnerTitleInfo(ownerCategory, ownerSlot, &ownerItem))
        {
            if (category == ownerCategory)
            {
                ++count;
                hasOwner = 1;
            }
        }
    }
    packet.put_int(count);
#if !remake_verify
    cMyTrace(__PRETTY_FUNCTION__, 0, 5)("[DNF-DIAG TITLE] cat=%d count=%d type=%d extra=%u",
        (int)category, count, (int)type, (unsigned)extra);
#endif
    for (int i = 0; i < TITLE_BOOK_MAX[(int)category]; ++i)
    {
        Inven_Item* item = m_category[(int)category] + i;
        if (item->m_addInfo != 0)
        {
            _putItemData(packet, (short)i, *item);
        }
    }
    if (hasOwner)
    {
        _putItemData(packet, (short)ownerSlot, ownerItem);
    }
    packet.finalize(true);
    user->Send(packet);
}