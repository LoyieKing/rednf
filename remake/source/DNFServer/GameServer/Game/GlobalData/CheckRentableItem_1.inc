// Dimension::CheckRentableItem（ORIG 0x0826eea8）：同构。
// [b28d-18 IDENTICAL_AE] 32=32；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x0826eea8
bool RentalMachineDimension::CheckRentableItem(unsigned int itemIdx)
{
    // ORIG 0x0826ef01：const_iterator::ne 的 this = end() 临时槽(-0xc)、
    // 实参 = it 槽(-0x10)——源码序 `end() != it` 复现。
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return m_impl->m_set.end() != it;
}