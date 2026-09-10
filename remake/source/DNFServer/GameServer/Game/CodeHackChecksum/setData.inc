void CodeHackChecksumEx::setData(const char* serialized)
{
    // ORIG 直接推进形参槽（addl $0x4,0xc(%ebp)）——下文直接以 serialized 使用。
    int nameLen = static_cast<signed char>(*serialized);  // movzbl+movsbl
    serialized += 4;                                      // addl $0x4
    std::strncpy(m_fileName, toTString(serialized), nameLen);
    serialized += nameLen;                                // 名称缓冲推进
    std::memcpy(&m_range, serialized, sizeof(m_range));   // memcpy(this+0x110,__src,8)
    serialized += 8;                                      // addl $0x8
    int count = static_cast<signed char>(*serialized);    // local_14
    serialized += 4;                                      // addl $0x4
    for (int i = 0; i < count; ++i)
    {
        // ORIG 循环体仅 memcpy 到栈缓冲 + Add（无 entry 构造——ORIG 的
        // CodeHackChecksum ctor 为弱符号从未被调用）；reinterpret 避免 C1/D1。
        unsigned char buf[0x20];
        std::memcpy(buf, serialized, sizeof(buf));        // memcpy(local_34,p,0x20)
        serialized += 0x20;                               // addl $0x20
        AddCodeHackChecksum(*reinterpret_cast<CodeHackChecksum*>(buf));  // push_back
        // [b28d-solo-CodeHackChecksum N-P75 封顶] ORIG 0x0808a89e `mov 0x8(%ebp),%eax;
        // lea -0x34(%ebp),%edx`（this 先求值，实参后）；4.4 对成员调用恒先求实参、
        // 后求 this（求值序右到左）。本波八形实测均不翻：裸调用/显式 this->/
        // Data:: 限定/reinterpret_cast 向上/static_cast 向上/(*this)/&buf[0]/
        // 派生类调基类方法/逗号表达式（探针 /tmp/probe_chc.cpp、probe_chc2.cpp
        // sA-sE、probe_chc3.cpp tA-tF、probe_chc4.cpp uA-uD）⇒ 跨版本实参/this
        // 求值序差异（P75），语义等价，封顶。
    }
