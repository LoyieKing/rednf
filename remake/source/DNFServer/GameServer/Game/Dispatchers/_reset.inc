int Dispatcher_Compound_ExtreamItem::_reset()
{
    // ORIG 0x0822577e T：i=0..1 → ushort(this+8+i*2)=0、Inven_Item::reset(this+0xc+i*0x3d)、
    // uint(this+0x88+i*4)=0；随后 ushort(this+6)=0、Inven_Item::reset(this+0x90)、
    // ushort(this+4)=0、byte(this+0xcd)=2、byte(this+0xd4)=2。
    // [G1Frest 2026-09-10] ORIG 0x0822578c 起 `mov -0xc(%ebp),%edx; mov 0x8(%ebp),%eax;
    //   movw $0x0,0x8(%eax,%edx,2)` —— 每次重载 this 形参 + **scaled index 直存**；
    //   旧码 `char* p` 具名局部(-0x10) + `p+8+i*2` 展开成 add/add/add 全链（帧 +4、+11 条）。
    //   ⇒ 删 p，改用数组下标形（`((T*)((char*)this+8))[i]`）。
    for (int i = 0; i < 2; ++i)
    {
        reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(this) + 8)[i] = 0;
        reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(this) + 0xc + i * 0x3d)->reset();
        reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 8)[i + 0x20] = 0;
    }
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(this) + 6) = 0;
    m_resultItem.reset();                                       // +0x90
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(this) + 4) = 0;
    reinterpret_cast<char*>(this)[0xcd] = 2;
    reinterpret_cast<char*>(this)[0xd4] = 2;
    return 0;
