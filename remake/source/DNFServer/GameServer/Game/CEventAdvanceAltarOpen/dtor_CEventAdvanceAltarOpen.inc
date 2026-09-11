// ORIG 0x080e95e2（弱 W）：D1 `movl $0x8b314e8,(%eax)` 重置 vptr（vtable+8）后
// call CEventBase::~CEventBase(0x0810ae52)，`mov $0x0,%eax; test %al,%al` 判定
// deleting 标志为假；D0 0x080e9612 调 D1 后 operator delete。空体 + 虚析构
// 由编译器生成同形。
CEventAdvanceAltarOpen::~CEventAdvanceAltarOpen()
{
}