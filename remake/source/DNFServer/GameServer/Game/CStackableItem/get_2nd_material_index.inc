// ORIG 0x822c9e0（弱符号 W，14 字节）：mov 0x29c(%eax),%eax —— 第 2 材料项索引
int CStackableItem::get_2nd_material_index()
{
    return m_limitCube.m_2ndMaterialIdx;
}