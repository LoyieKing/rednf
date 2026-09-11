// ORIG 0x081c2792 T _ZN21DisPatcher_DeleteItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_DeleteItem/send.md 汇编+反编译 C 全文转写）：
// m_4!=0x7fffffff 时发包。m_4==0：put_header(1,0x12)/1/m_8/m_9，i<(char)m_9 循环
// put_short(m_slotsD2[i])/put_int(m_counts19c[i])/put_short(m_slotsA[i])，
// finalize(true)+Send；否则 0/errNo/m_8 三字节包。
void DisPatcher_DeleteItem::send(CUser* user, ParamBase& param)

{
    stDeleteItemParam& p = static_cast<stDeleteItemParam&>(param);
    PacketGuard guard;
    if (p.m_4 != 0x7fffffff)
    {
        if (p.m_4 == 0)
        {
            guard.put_header(1, 0x12);
            guard.put_byte(1);
            guard.put_byte((char)p.m_8);
            guard.put_byte((char)p.m_9);
            for (int i = 0; i < (int)(char)p.m_9; ++i)
            {
                guard.put_short((int)p.m_slotsD2[i]);
                guard.put_int(p.m_counts19c[i]);
                guard.put_short((int)p.m_slotsA[i]);
            }
            guard.finalize(true);
            user->Send(guard);
        }
        else
        {
            guard.put_header(1, 0x12);
            guard.put_byte(0);
            guard.put_byte(p.m_4);
            guard.put_byte((char)p.m_8);
            guard.finalize(true);
            user->Send(guard);
        }
    }
}
