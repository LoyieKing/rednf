// ORIG 0x0811dce6：向用户发送单项词典标记（接口包 0x181：byte=1 + int item）。
void CItemDictionary::sendItemDictionaryInfo(CUser* user, unsigned int item)
{
    if (!user) return;
    PacketGuard guard;
    guard.put_header(0, 0x181);
    guard.put_byte(1);
    guard.put_int(static_cast<int>(item));
    guard.finalize(true);
    user->Send(guard);
}
