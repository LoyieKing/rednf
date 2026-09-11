// ORIG 083ce3ee：先析构 m_itemList(+0x10) 再 m_skillList(+0x04)（成员声明逆序，编译器自动生成）
STEventCharacterInfo::~STEventCharacterInfo() {}
