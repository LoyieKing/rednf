// ----------------------------------------------------------------------------
// ORIG T 0x0834f8e8 _ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0x70(script) 取 m_catchItems 转发。
void CMonster::generateCatchMonsterItem(std::vector<Inven_Item>& out) const
{
    // P134：ORIG call 后结果寄存器直用（lea 0x70(%eax)），无 script 局部槽。
    ::generateCatchMonsterItem(
        static_cast<STMonsterScript*>(getMonsterScript())->m_catchItems, out);
}