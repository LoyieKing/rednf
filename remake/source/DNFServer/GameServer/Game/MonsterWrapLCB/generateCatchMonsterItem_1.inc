void CMonster::generateCatchMonsterItem(std::vector<Inven_Item>& out) const
{
    // P134：ORIG call 后结果寄存器直用（lea 0x70(%eax)），无 script 局部槽。
    ::generateCatchMonsterItem(
        static_cast<STMonsterScript*>(getMonsterScript())->m_catchItems, out);
