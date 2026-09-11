// ORIG 0x822c9ee（弱符号 W，14 字节）：mov 0x2a0(%eax),%eax —— 第 2 材料项数量
int CStackableItem::get_2nd_material_count()
{
    return m_limitCube.m_2ndMaterialCount;
}