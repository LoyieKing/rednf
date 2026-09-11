StrikerScript* StrikerScript::getInsance()
{
    // ORIG 0x08a9d836：懒加载单例（new 0xb4 → ctor → 存 instance_）。
    if (instance_ == 0)
    {
        instance_ = new StrikerScript;
    }
    return instance_;
}