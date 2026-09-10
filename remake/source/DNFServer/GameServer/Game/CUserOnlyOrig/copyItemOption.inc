void CUser::copyItemOption(Inven_Item& dst, Inven_Item& src)
{
    dst.SetUpgrade(src.GetUpgrade());
    dst.set_add_info(src.get_add_info());
    // [b28d-solo-CUserOnlyOrig P308] ORIG 0x08671ef0 段 `mov src; movzbl; mov %eax,%edx;
    // and $0x1f; …and $0x1f,%ecx; and $0xffffffe0; or` = 位域赋值（重复掩码为 4.1 位域
    // 装填产物）；手写 `(dst&0xe0)|(src&0x1f)` 反产 dst-first 装载序。
    dst.m_upgradeSep.m_bits.m_lo = src.m_upgradeSep.m_bits.m_lo;
    // ORIG 仅拷 m_amp 前 4 字节（mov 0x11(%edx)→0x11(%eax)；余 0x15..0x1e 不拷）
    // [b28d-22 修法 P235/P183] ORIG 0x08671f12 `mov 0x11(%edx),%edx; mov %edx,0x11(%eax)`
    // 单条 disp ⇒ 用 CItemAmplifier.h 具名 union 视角 m_amp0Int（取址 reinterpret 形恒多 add）；
    // 删具名 amp 引用（ORIG 0x08671f2a 起每处重算 `add $0x11,%eax`）。
    // [b28d-solo-CUserOnlyOrig P247] ORIG 0x08671f12 `mov dst,%eax; mov src,%edx;
    // mov 0x11(%edx),%edx; mov %edx,0x11(%eax)` = 结构整体赋值形（P247）；标量 int
    // 视角 4.4 产 RHS-first（`mov src,%eax; mov 0x11(%eax),%edx; mov dst,%eax; …`）。
    dst.m_amp.m_amp0Head = src.m_amp.m_amp0Head;
    dst.m_fieldd = src.m_fieldd;                  // +0xd int
    LogHistory("Compound+,%u,%d,%d,%d,%d,%d,%d,%d,",
               *(unsigned int*)&dst.m_addInfo,               // +2 uint
               (unsigned)dst.GetUpgrade() & 0xff,
               dst.get_add_info(),
               // [b28d-solo-CUserOnlyOrig N7 封顶] 变参右起求值：ORIG m_fieldd→槽 -0x1c /
               // 位域→%edi；4.4 二者互换（m_fieldd→%edi、位域→槽）且位域多一条 movzbl
               // （uchar cast 形同产）。寄存器分配族，语义等价。
               (unsigned)(dst.m_upgradeSep.m_field & 0x1f),
               dst.m_fieldd,
               (unsigned)dst.m_amp.getAbilityType() & 0xff,
               (unsigned)dst.m_amp.getAbilityValue() & 0xffff,
               (unsigned)dst.m_amp.GetLock() & 0xff);
