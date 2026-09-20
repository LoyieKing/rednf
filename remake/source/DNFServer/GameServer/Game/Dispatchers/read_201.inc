int Dispatcher_SecretShopOpenClose::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SECRET_SHOP_OPEN_CLOSE& req = *reinterpret_cast<MSG_SECRET_SHOP_OPEN_CLOSE*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_openFlag)))
    {
        return LineFunc(0x2fc4, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
