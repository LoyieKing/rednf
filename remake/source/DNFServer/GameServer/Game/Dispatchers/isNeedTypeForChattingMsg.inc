// ORIG W 0x0822c1ca（_ZN 非 const）：(unsigned)type <= 2。
bool Packet_Mannerless_User_Accusation::isNeedTypeForChattingMsg(int type)   // ORIG 静态：无 this
{
    // ORIG 0x0822c1d0：`mov 0x8(%ebp),%eax; add $0x0,%eax; cmp $0x2,%eax; ja` ——
    // 该 `add $0` 是 switch 的 min-case 偏移量，4.4 仅在 switch 形下保留
    // （`(unsigned)type <= 2` 写法被折掉 add，probe pd w1/w2 实证）。
    switch (type)
    {
    case 0:
    case 1:
    case 2:
        return true;
    default:
        return false;
    }
}