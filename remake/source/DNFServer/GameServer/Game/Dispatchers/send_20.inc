void Dispatcher_LimitNpcBuyItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e1100：p4>0 → SendCmdErrorPacket(0x1d1,p4&0xff)（PacketGuard 生命周期忠实保留）
    LimitNpcBuyItem& p = static_cast<LimitNpcBuyItem&>(param);
    {
        PacketGuard pkt;
        if (p.m_4 > 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1d1,
                                     (unsigned char)(p.m_4 & 0xff));
        }
    }
