// ---------------------------------------------------------------------------
// ORIG 0x08263224（报告 Dispatcher_MouseRegister/process.md）：
// param.m_4 = check_error(user,msg)；>0 直返；<0 → return LineFunc(0x178,...)；
// ==0 路径物化 msg 死槽（ORIG 尾 mov 0x10(%ebp)=&msg；4.1 死局部发射，
// 探针 /tmp/p17c.cpp procA 同形）。块排布与 ORIG 逐条同形。
// （check_error 定义见 Dispatchers.cpp）
int Dispatcher_MouseRegister::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    TempParam& out = static_cast<TempParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x178,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    MSG_BASE* dead = &msg;
    (void)dead;
    return 0;
}