void DisPatcher_EventDungeon_DestoryObject::send(CUser* user, ParamBase& param)

{
    ParamBase* pParam = &param;   // ORIG -0xc(%ebp)（local_10，先于 guard ctor 落栈）
    PacketGuard guard;
    guard.put_header(1, 0x21b);
    guard.put_byte(1);
    guard.put_binary(reinterpret_cast<char*>(pParam), 6);
    guard.finalize(true);
    user->Send(guard);
}
