int Dispatcher_Compound_ExtreamItem::_reset()
{
    // ORIG 0x0822577e T：i=0..1 → ushort(this+8+i*2)=0、Inven_Item::reset(this+0xc+i*0x3d)、
    // uint(this+0x88+i*4)=0；随后 ushort(this+6)=0、Inven_Item::reset(this+0x90)、
    // ushort(this+4)=0、byte(this+0xcd)=2、byte(this+0xd4)=2。
    char* p = reinterpret_cast<char*>(this);
    for (int i = 0; i < 2; ++i)
    {
        *reinterpret_cast<unsigned short*>(p + 8 + i * 2) = 0;
        reinterpret_cast<Inven_Item*>(p + 0xc + i * 0x3d)->reset();
        *reinterpret_cast<unsigned int*>(p + 0x88 + i * 4) = 0;
    }
    *reinterpret_cast<unsigned short*>(p + 6) = 0;
    m_resultItem.reset();                                       // +0x90
    *reinterpret_cast<unsigned short*>(p + 4) = 0;
    p[0xcd] = 2;
    p[0xd4] = 2;
    return 0;
