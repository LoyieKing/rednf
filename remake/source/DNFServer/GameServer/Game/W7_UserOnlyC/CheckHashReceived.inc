// ORIG T 0x0808bf7e PIReceiverManager::CheckHashReceived(uchar*,uint,uint,bool)
// this+0x04 为接收 hash key 字节状态。
// b28d-solo-W7_UserOnlyC 判词：P0/P183 删 pThis 指针缓存（this 表达式直读）+
// P211 cur 同型化（L/S 同型 ⇒ LHS-first）⇒ 72=72，TU 层 AE。
bool PIReceiverManager::CheckHashReceived(unsigned char* data,
                                          unsigned int len, unsigned int key,
                                          bool bNewKey)
{
    // P0/P183：ORIG 无 this 指针缓存槽（每处 `mov 0x8(%ebp),%eax` 表达式直读）。
    // CUser.h 冻结：+0x04 现为 char m_pad[4]（hash key 字节），以 m_pad[0] 具名访问。
    if (bNewKey)
        m_pad[0] = 0x27;

    const unsigned short packed = static_cast<unsigned short>(key >> 8);
    const unsigned char oldKey = m_pad[0];

    // 新 key 低字节重组：位段 12..13→0..1、4..5→2..3、8..9→4..5、整体左移 6 取低 8 位。
    // P211 同型化：字段 char 与 RHS 同型 ⇒ store 装载序 LHS-first（ORIG 形）。
    const char cur = static_cast<char>(
        ((packed & 0x3000) >> 12) | ((packed & 0x30) >> 2) |
        ((packed & 0x300) >> 4) | (packed << 6));
    m_pad[0] = cur;      // ORIG 无条件写回新 key（在 expect 计算之前）

    // 校验字节：位段 6..7→0..1、14..15→2..3、2..3→4..5、10..11→6..7。
    // P198①：ORIG 先 movb $0,-0xb / movb $0,-0xa 预清 ok/hash 两句式。
    bool ok = false;
    unsigned char hash = 0;
    const unsigned char expect = static_cast<unsigned char>(
        ((packed & 0xc0) >> 6) | ((packed & 0xc000) >> 12) |
        ((packed & 0xc) << 2) | ((packed & 0xc00) >> 4));
    if (len == 0)
    {
        ok = true;
    }
    else
    {
        hash = MakeHash(data, len, oldKey);
        ok = (hash == expect);
    }

    return ok;
}