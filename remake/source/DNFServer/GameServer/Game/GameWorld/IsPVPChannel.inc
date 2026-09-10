bool GameWorld::IsPVPChannel() const
{
    // ORIG _ZNK9GameWorld12IsPVPChannelEv @0x081424e8：m_channelType 只装载一次
    // （0x081424ec `mov 0x8(%ebp),%eax` + 0x081424ef `mov 0x20(%eax),%eax`），
    // 0x081424f2 cmp 与 0x081424fe `mov %eax,%ecx`（移位量）复用同一 %eax
    // （0x081424f5 ja / 0x081424f7 `mov $0x1,%edx` / 0x081424fc `mov %edx,%ebx`
    //  / 0x08142500 `shl %cl,%ebx` / 0x08142502 `mov %ebx,%eax`）；
    // 4.4 默认在 `one << m_channelType` 处再装载一遍 ⇒ 多 2 条。
    // b28c-lane3：历史 register asm("eax")/asm("edx") 绑定按 P0/N96 清除；
    // 普通 register 局部（4.4 无栈槽）阻断 N6 位折叠并保持单次装载。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        // ORIG 0x081424f7 `mov $0x1,%edx` + 0x081424fc `mov %edx,%ebx` 是
        // 4.1 的 scratch→目标两段形态；one 走 register 局部即复现。
        // b28d-lane11 判词 N96/N6：清绑后残余仅 `mov %eax,%ebx`（cmp 前）与
        // `mov %ebx,%eax`（shl 前）两条中转——4.4 把 register ct 落 %ebx，
        // ORIG ct 留 %eax 不可造；试形单 if 短路（合并两条件）触发 N6 折叠
        // 反而劣化 ndiff 4→12（shr/and $1），故维持嵌套 if + register one。
        register unsigned int one = 1u << ct;
        if (one & 0x1e7f0)
            return true;
    }
    return false;
