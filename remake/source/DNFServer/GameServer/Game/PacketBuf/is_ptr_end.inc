bool PacketBuf::is_ptr_end(int index, int len)
{
    if (index + len > PKT_PTR_END_LIMIT)
    {
        setLastError(PACKETBUF_ERROR_PTR_END);
        return true;
    }
    return false;
}