bool DB_DispatcherTaiwan::dispatch(int userIdx, int slot, Stream* stream)
{
    int code = 0;   // ORIG movl $0,-0x20(%ebp)
    *stream >> code;   // ORIG call Stream::operator>>(int&)

    TaiwanInterfaceDBDispatcher* disp =
        m_disp.getDispatcher((TaiwanInternalPack::T)code);   // ORIG call getDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（code 提前入 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0xc17a, 5)
            ("[TaiwanDispater] Invalid internal packet code. Code:%d, Uid:%d",
             code, slot);
        return false;   // ORIG mov $0,%eax
    }
    return disp->dispatch(userIdx, slot, stream);   // ORIG 虚槽2（vptr+8）直调
