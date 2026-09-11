void Dispatcher_Compound_ExtreamItem::_reset()
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
        reinterpret_cast<ExtreamItemSlotView*>(this)->m_slots[i] = 0;
        reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(this) + 0xc + i * 0x3d)->reset();
        // [G3 负结论 2026-09-11] 父泳道线索（结合序 `(this + i*0x3d) + 0xc`）已试：
        //   源码改左结合形后 fresh 复编读数**逐值不变**（仍 37/38、仍 `mov 0x8(%ebp),%eax;
        //   mov -0xc(%ebp),%edx; imul $0x3d,%edx,%edx`）⇒ GCC 4.4 对两种结合序折叠一致，
        //   本件残余 = ORIG 0x0822579a `imul $0x3d,%eax,%eax; add 0x8(%ebp),%eax`（eax 目标 +
        //   内存直加） vs 我方 edx 目标 + 额外 this 装载，属 N-寄存器目标死区，已回滚。
        reinterpret_cast<ExtreamItemWordView*>(this)->m_words[i + 0x20] = 0;
    }
    reinterpret_cast<ExtreamItemSlotView*>(this)->m_newSlot = 0;
    m_resultItem.reset();                                       // +0x90
    reinterpret_cast<ExtreamItemSlotView*>(this)->m_f4 = 0;
    reinterpret_cast<ExtreamItemSlotView*>(this)->m_cd = 2;
    reinterpret_cast<ExtreamItemSlotView*>(this)->m_d4 = 2;
    // [K1] ORIG 0x08225804 `leave; ret` 无 eax 设置 ⇒ void（删 `return 0;`）。
}