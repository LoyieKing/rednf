void StringResourceDelayedItem::makeCacheStr()
{
    CharString const* out;
    // ORIG 0x08ad538a：-0xc 槽**无 C1**、format 直调、aSER、尾显式 D1（两分支汇合后
    // 统一析构）⇒ ORIG 源形=union 包裹抑制默认构造（4.1.2 扩展允许非平凡 union 成员，
    // 4.4.7 硬拒——本仓实测）。忠实复刻=raw-buffer 引用形：零新符号、发射形逐条同 ORIG
    // （format 内部 placement C1 完成初始化，显式 D1 释放；无未初始化读）。
    unsigned long msgRaw[(sizeof(CharString) + sizeof(unsigned long) - 1) / sizeof(unsigned long)];
    CharString& msg = *reinterpret_cast<CharString*>(msgRaw);
    if (get(&out)) {
        m_cacheStr = *out;
    } else {
        msg = CharString::format("getStr(%d) == none", m_id);
        m_cacheStr = msg;
        msg.~CharString();   // ORIG 显式 D1 仅在 else 路径（0x8ad5400←0x3f4；hit 路径 0x3bd 直落 c_str）
    }
    m_pCache = m_cacheStr.c_str();
