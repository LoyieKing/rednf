void Dispatcher_Mercenary_Info::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ddb7c (0xa7 insn): put_header(1,0x1ba) + put_byte(1) +
    // SetMercenaryInfoPacket + finalize(true) + Send
    PacketGuard guard;
    guard.put_header(1, 0x1ba);
    guard.put_byte(1);
    user->SetMercenaryInfoPacket(guard);
    guard.finalize(true);
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG  // remake-only diag; off unless -DDNF_DIAG=1
    {
        int blen = guard.get_len();
        const unsigned char* bp = reinterpret_cast<const unsigned char*>(guard.get_packet(0));
        char hx[3 * 64 + 1];
        hx[0] = 0;
        int n = blen < 64 ? blen : 64;
        for (int k = 0; k < n; ++k)
        {
            char b[4];
            b[0] = "0123456789abcdef"[(bp[k] >> 4) & 0xf];
            b[1] = "0123456789abcdef"[bp[k] & 0xf];
            b[2] = ' ';
            b[3] = 0;
            for (int j = 0; j < 3; ++j) hx[k * 3 + j] = b[j];
        }
        hx[n * 3] = 0;
        cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
            "[DNF-DIAG MERC] send 442 len=%d bytes=%s", blen, hx);
    }
#endif
    user->Send(guard);
}