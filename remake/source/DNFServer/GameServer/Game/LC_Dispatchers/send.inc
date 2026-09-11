// ORIG 0x082632b4：读取 param+0x04（ORIG 尾部 eax 返回值；头声明 void，
// dispatch 模板不消费返回值）。
void Dispatcher_MouseRegister::send(CUser* user, ParamBase& param)
{
    (void)user;
    TempParam& out = static_cast<TempParam&>(param);
    if (out.m_4) return;  // ORIG 尾 load+test（探针 senJ 同形）
}