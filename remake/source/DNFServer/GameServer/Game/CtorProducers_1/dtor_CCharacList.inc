CCharacList::~CCharacList()
{
    // ORIG 0x0834945e：destroy() 后逆序析构各元素（成员析构自动生成）。
    destroy();
