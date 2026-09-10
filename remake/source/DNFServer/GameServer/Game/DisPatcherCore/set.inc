void Packet_Monitor_MegaPhone_Hyper_Link::set(

    unsigned char channel, unsigned short uniqueId, char const* name,
    unsigned char msgLen, char const* msg, unsigned char type,
    unsigned char itemCount, hyperlink_item_info const* items)
{
    // [b28c-34] ORIG：`mov 0x8(%ebp),%eax; movzbl -0x1c(%ebp),%edx; mov %dl,0xb(%eax)`
    reinterpret_cast<MegaPhoneHyperLinkSetView*>(this)->m_channel = channel;
    m_uniqueId = uniqueId;
    m_type = type;
    strncpy(m_name, name, 0x1e);
    strncpy(m_msg, msg, 0x100);
    // [b28c-34] ORIG：`mov 0x8(%ebp),%eax; movzbl -0x24(%ebp),%edx; mov %dl,0x166(%eax)`
    reinterpret_cast<MegaPhoneHyperLinkSetView*>(this)->m_msgLen = msgLen;
    m_itemCount = itemCount;
    // ORIG 0x086d1b9a：src `mov -0xc,%eax; imul $0x68; add 0x28(%ebp),%eax`、
    // dst `mov -0xc,%edx; imul; add $0x20; add 0x8(%ebp); add $0xe`——两侧均为
    // char 数组下标形（各重算一遍 i*0x68、常量不合并）。
    for (int i = 0; i < itemCount; ++i)
    {
        memcpy(&m_items[i * 0x68],
               &reinterpret_cast<char*>(
                   const_cast<hyperlink_item_info*>(items))[i * 0x68], 0x68);
    }
    packetSize = 0x167 + msgLen;
}
