// ORIG 0x081c9656 T _ZN18DisPatcher_SetArea4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/send.md 汇编+反编译 C 全文转写）：
// m_errNo!=0x7fffffff 且 !=0 时发包：put_header(1,0x26)/0/errNo；errNo==0x44 →
// put_byte((char)m_9)+GetLeftTimeOfForbiddenMove(getCurCharacName())/60000 put_int；
// 否则 put_byte((char)m_8)；finalize(true)+Send。
void DisPatcher_SetArea::send(CUser* user, ParamBase& param)

{
    stSetAreaParam& p = static_cast<stSetAreaParam&>(param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0x26);
        guard.put_byte(0);
        guard.put_byte(p.m_errNo);
        if (p.m_errNo == 0x44)
        {
            guard.put_byte((char)p.m_9);
            guard.put_int(
                G_GameWorld()->GetLeftTimeOfForbiddenMove(
                    user->getCurCharacName()) /
                60000);
        }
        else
        {
            guard.put_byte((char)p.m_8);
        }
        guard.finalize(true);
        user->Send(guard);
    }
}
