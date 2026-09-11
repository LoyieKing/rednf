// ORIG D1 0x0811ff7c：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查
// （mov $0x0,%eax; test %al,%al; je —— gcc 虚析构标准形态）。
LevelUpBefore70LvEvent::~LevelUpBefore70LvEvent()
{
}