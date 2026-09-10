void CParty::send_hellParty_value()
{
    PacketGuard packet;
    packet.put_header(0, 0xcf);

    // [b28d-51] P0：ORIG 无具名 float 局部（flds 0xb0c; flds 0x8cbbbcc; fmulp;
    // fildl 0xd0c; fdivrp; fistpl 一气呵成）⇒ 表达式必须内联。
    const int out =
        D2_ROUND((bd(this).m_hellPartyValueTotal * 100.0f) /
                 static_cast<float>(bf(this).m_hellPartyRatio0));
    packet.put_int(out);
    packet.finalize(true);
    send_to_party(packet);
