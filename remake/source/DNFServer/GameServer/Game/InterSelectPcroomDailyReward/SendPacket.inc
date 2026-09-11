// ---- SendPacket ----
// ORIG 0x81618a6：若 user==0 直接返回；否则 PacketGuard ctor;
//   put_header(0,0x22e); put_byte((char)user[0x8ec32]); finalize(true); CUser::Send。
//   0x8ec32 = CUser::m_field8ec32（char）。
void InterSelectPcroomDailyReward::SendPacket(CUser* user)
{
    // b28c-33：ORIG 尾部为 `je <nop 岛>`（+0xa3）+ `jmp +0xa4`，即 P223 空 then 占位岛；
    // 正向 `if(user!=0){}` 被 4.4 折成直落 ⇒ 改负条件 goto 到共享尾空标签。
    if (user == 0)
        goto END;
    {
        PacketGuard guard;
        guard.put_header(0, 0x22e);
        guard.put_byte((int)(char)user->m_field8ec32);
        guard.finalize(true);
        user->Send(guard);
    }
END: ;
}