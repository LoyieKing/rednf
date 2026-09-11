// ----------------------------------------------------------------------------
// StringDictionary::StringDictionary()（0x8ab2240）
//   vector<unsigned char*> ctor；m_curPtr=0；m_curRemain=0；
//   vector<StringEntity> ctor；0x100 个桶迭代器默认构造。
//   注意：ORIG 不构造 buckets_[0x100]（末端迭代器由 freeze 写入），但为
//   与额外默认构造一致无 ABI 影响；这里显式将 0x101 个迭代器 value-init。
// ----------------------------------------------------------------------------
StringDictionary::StringDictionary()
    : blocks_(),
      m_curPtr(0),
      m_curRemain(0),
      entities_()
{
    // [b28c-11] AE：buckets_[0x101] 改为隐式数组构造（删显式 for）⇒ 4.4 复现
    // ORIG 倒计数构造循环（P151：mov $0x100,%esi; sub; cmp $-1; setne/test/jne），
    // 43/43 条全等（.o 重定位显示差）。
}