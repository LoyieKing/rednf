char* CodeHackCheckRangeEx::setData(const char* serialized)   // T @0x0808a9cc
{
    // [b28c-L29] ORIG 0x0808a9cc：形参 serialized 自身自增（addl $0x4,0xc(%ebp)/
    // add %eax,0xc(%ebp)），每处重读形参槽——源码无命名局部 p（P0）。
    int nameLen = static_cast<signed char>(*serialized);  // movzbl/movsbl
    serialized += 4;                                      // addl $0x4
    std::strncpy(m_fileName, toTString(serialized), nameLen);
    serialized += nameLen;                                // add %eax,0xc(%ebp)
    std::memcpy(&m_start104, serialized, 8);              // memcpy(this+0x104,...,8)
    serialized += 8;                                      // addl $0x8
    return const_cast<char*>(serialized);
