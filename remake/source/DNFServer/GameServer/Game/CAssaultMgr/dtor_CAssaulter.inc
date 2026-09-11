// b28d-solo-CAssaultMgr: P210 三分律②——外联定义置于 CAssaultPlace::~CAssaultPlace
// 之前，使 4.4 对其数组析构循环精确继承 ORIG 的 sub $0x4 帧（原头内 inline 为①，
// 保守撑帧 sub $0x14 ⇒ D1 NEAR 2）。
CAssaulter::~CAssaulter()
{
}