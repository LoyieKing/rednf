// ===== Dispatcher_PurifyItem::processPurifyType —— ORIG 0x082194b6 =====
// ORIG：type==0 → 直接 return identify()（bool 值原样穿过 %eax，无 test/movzbl 物化）；
// type==1 → reset 后 return 1；其它 → 0。caller process @0x8219166 `xor $0x1,%eax`
// 全字反证 %eax∈{0,1}：bool 返回 ABI 下成立。
bool Dispatcher_PurifyItem::processPurifyType(int purifyType, Inven_Item& item)
{
    if (purifyType == 0)
    {
        return item.m_amp.identify();   // ORIG 0x082194cb：call 后落 %eax 直返
    }
    if (purifyType == 1)
    {
        item.m_amp.reset();
        return true;
    }
    return false;
}