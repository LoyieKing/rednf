CharacManageScript* CharacManageScript::getInsance()
{
    // ORIG 0x088e0674：懒加载单例（new 0x88 → ctor → 存 instance_）。
    if (instance_ == 0)
    {
        instance_ = new CharacManageScript;
    }
    return instance_;
