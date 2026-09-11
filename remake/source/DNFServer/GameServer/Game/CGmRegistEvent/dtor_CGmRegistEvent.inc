// ORIG 0x0811cc94 / 0x0811ccc4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CGmRegistEvent::~CGmRegistEvent()
{
}