// PcRoom::CheckRentableItem（ORIG 0x08270752）：
// return m_impl->m_set.find(itemIdx) != m_impl->m_set.end()。
bool RentalMachinePcRoom::CheckRentableItem(unsigned int itemIdx)
{
    // ORIG 0x08270752：auto_ptr 解引用后 m_set.find(itemIdx) != end()。
    // ORIG 0x082707b2：ne 的 this = end() 临时槽(-0xc)、实参 = it 槽
    // (-0x10) ⇒ 源码序 `end() != it`（Dimension 同构已验证）。
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return m_impl->m_set.end() != it;
}