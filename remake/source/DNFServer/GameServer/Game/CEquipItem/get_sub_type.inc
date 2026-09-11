// ORIG 0x833eecc：返回 this+0x160（脚本写入的 sub type 选择值，见 set_item 报告）
int CEquipItem::get_sub_type() const
{
    return m_randomTable.m_field18;
}