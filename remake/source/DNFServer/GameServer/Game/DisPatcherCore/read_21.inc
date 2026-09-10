int DisPatcher_SortItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SORT_ITEM& m = *reinterpret_cast<MSG_SORT_ITEM*>(&msg);
    if (!packet.get_byte(m.m_invenType))                  // +0x0d
    {
        return LineFunc(0x1d16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
