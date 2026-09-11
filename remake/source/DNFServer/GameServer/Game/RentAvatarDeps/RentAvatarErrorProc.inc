// ORIG W _Z18RentAvatarErrorProcP5CUseri @ 0x082352c5 (size 0xb2)
// void RentAvatarErrorProc(CUser*, int) — 逐指令转写：
//   PacketGuard 构造 → clear() → put_header(0x1, 0x16)
//   → put_byte(0) → put_byte(errCode) → finalize(true) → user->Send(guard)
// ORIG 无返回值（ebp 恢复前不写 eax），签名 void 与调用点语句级用法一致。
void RentAvatarErrorProc(CUser* user, int errCode)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(0x1, 0x16);
    guard.put_byte(0);
    guard.put_byte(errCode);
    guard.finalize(true);
    user->Send(guard);
}