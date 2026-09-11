// ORIG @0x08ade844（sret）：*sret = Tick(nowAsInternalValue())。
Tick Tick::now()
{
    return Tick(nowAsInternalValue());
}