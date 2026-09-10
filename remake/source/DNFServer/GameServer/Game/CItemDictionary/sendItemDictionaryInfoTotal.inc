void CItemDictionary::sendItemDictionaryInfoTotal(CUser* user)
{
    if (user == 0)
        return;
    PacketGuard guard;
    guard.put_header(0, 0x18a);
    const int length = getLengthBinary();
    if (length == 0)
        guard.put_int(0);                     // ORIG 0x0811dde6：空词典显式写 0
    else
    {
        guard.put_int(length);
        // ORIG 0x0811de24 → _ZN18InterfacePacketBuf10put_binaryEPci（非 const 重载，
        // W 0x0811df08）；传 const 会选到 PKci（0x0815096a）错目标。
        guard.put_binary(reinterpret_cast<char*>(m_data), length);
    }
    guard.finalize(true);
    user->Send(guard);
