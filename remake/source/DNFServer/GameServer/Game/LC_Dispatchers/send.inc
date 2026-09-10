void Dispatcher_MouseRegister::send(CUser* user, ParamBase& param)
{
    (void)user;
    TempParam& out = static_cast<TempParam&>(param);
    if (out.m_4) return;  // ORIG 尾 load+test（探针 senJ 同形）
