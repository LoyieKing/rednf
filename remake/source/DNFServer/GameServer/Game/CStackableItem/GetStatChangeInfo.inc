// ORIG 0x08694716 (W 非 const)：无 ctor 调用，直接 8 字节拷贝
//   mov 0x2e0(%eax),%edx; mov 0x2dc(%eax),%eax; mov %eax,(%ecx); mov %edx,0x4(%ecx)
// +0x2dc 为 StatChangeInfo 子对象（m_stat2dc：int + 低两字节 flag）。
// 调用方(_processEffectItem)按 ORIG 只测 flag 的低 16 位。
StatChangeInfo CStackableItem::GetStatChangeInfo()
{
    // 直接 reinterpret 拷贝：不调用 StatChangeInfo ctor，生成 ORIG 的 8 字节整拷。
    return *reinterpret_cast<StatChangeInfo*>(&m_stat2dc);
}