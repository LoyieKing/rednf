// ORIG dtor 0x08373d9e：destroy() 后按逆序析构各子对象（成员析构由编译器生成）。
CAICharacterList::~CAICharacterList() { destroy(); }
