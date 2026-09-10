StringDictionary::StringDictionary()
    : blocks_(),
      m_curPtr(0),
      m_curRemain(0),
      entities_()
{
    // [b28c-11] AE：buckets_[0x101] 改为隐式数组构造（删显式 for）⇒ 4.4 复现
    // ORIG 倒计数构造循环（P151：mov $0x100,%esi; sub; cmp $-1; setne/test/jne），
    // 43/43 条全等（.o 重定位显示差）。
