// ORIG 0x081649cc / 0x081649fc：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
EventGiveMeBox::~EventGiveMeBox()
{
}