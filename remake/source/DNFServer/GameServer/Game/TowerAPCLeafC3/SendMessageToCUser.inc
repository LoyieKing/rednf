// ---------------------------------------------------------------------------
// TowerOfDespairMgr::SendMessageToCUser —— ORIG T 0x08644366
//                                        [0x08644366, 0x0864446f]（静态成员，
//                                        asm 无 this 槽，user@+8/msg@+0xc）
// isGMUser 命中才发包：PacketGuard() → put_header(0,0xc) → byte/short/byte 0
// → put_int(strlen(msg)) → put_str(msg, strlen(msg))（ORIG 调用两次 strlen）
// → finalize(true) → Send。
// ---------------------------------------------------------------------------
void TowerOfDespairMgr::SendMessageToCUser(CUser* user, char* msg)
{
    if (user->isGMUser())
    {
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int((int)strlen(msg));
        guard.put_str(msg, (int)strlen(msg));
        guard.finalize(true);
        user->Send(guard);
    }
}