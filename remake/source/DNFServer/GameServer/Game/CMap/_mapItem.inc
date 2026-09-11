// _mapItem ctor —— ORIG 0x0815088e：仅清零 +0x04/+0x08/+0x0c，不初始化 m_objectIdx。
_mapItem::_mapItem()
{
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 0;
}