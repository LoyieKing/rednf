void TownPath::clear()                      // ORIG 0x089e8258
{
    MapArea::clear();
    m_field1c = 0;                       // +0x1c movl $0
    m_field20 = 0;                       // +0x20 movl $0
    m_conds.clear();                     // vector<int>@0x24 clear
