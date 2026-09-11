// ORIG 单例为全局指针 0x940bd60（new 构造 / destroyInstance delete）。
// ORIG instance 0x0817dfec（0xa 字节）：直接返回全局指针（构造由 createInstance 完成）。
CLog* CLog::instance()
{
    return this_;
}