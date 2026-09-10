void CAICharacter::generateIndependentItem(
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out) const
{
    ::generateIndependentItem(m_independentDrops, ref, out);
