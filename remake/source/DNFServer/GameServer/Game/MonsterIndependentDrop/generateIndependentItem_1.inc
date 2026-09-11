// ----------------------------------------------------------------------------
// ORIG 0x0834a13a：lea 0x24(%eax) 取容器后直接转发自由函数。
void CAICharacter::generateIndependentItem(
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out) const
{
    ::generateIndependentItem(m_independentDrops, ref, out);
}