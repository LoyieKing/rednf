// ORIG 0x8541afe：空闲号池空 → 0x16 可锁；非空 → 0（CTitleBook.h:290 注释一致；
// 参数 lock 未使用）。
int CItemLock::IsAbleItemLock(unsigned char /*lock*/) const
{
    return m_lockStack.empty() ? 0x16 : 0;
}