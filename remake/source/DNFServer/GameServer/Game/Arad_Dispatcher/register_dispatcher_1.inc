bool Arad_Dispatcher_Manager::register_dispatcher(Arad_Dispatcher* d)
{
    // ORIG 0x0818fe98：dispathc_code() 两次调用（ORIG 无 code 栈局部，两次重取）；
    // inter[code]=d；db[code]=d；返回 0。
    // [b23判词·AE假阳性] 20/20 归一化逐条相等（含 `lea 0x354(%edx),%ecx` 第二组寻址形——
    // 两组指针数组连续声明使 db 基址折为 this+0xD50）；残余 2 处 call 目标显示为
    // .o 重定位占位（§6.3 假阳性），relink 后转 IDENTICAL_AE。
    m_interDispatchers[d->dispathc_code()] = d;
    m_dbDispatchers[d->dispathc_code()] = d;
    return 0;
}