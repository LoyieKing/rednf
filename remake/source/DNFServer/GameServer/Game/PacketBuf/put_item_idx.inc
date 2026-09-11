bool PacketBuf::put_item_idx(unsigned long value)
{
    return put_int(m_use_size, (int)value);
}