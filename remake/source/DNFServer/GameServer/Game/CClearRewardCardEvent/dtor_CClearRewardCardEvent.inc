// ORIG 0x0810a73c / 0x0810a76c：D1 重置 vptr 后调 CEventBase::~CEventBase（末尾
// __in_chrg 常量折叠不 delete）；D0 调 D1 后 operator delete。空体 + 虚析构由
// 编译器生成同形。
CClearRewardCardEvent::~CClearRewardCardEvent()
{
}