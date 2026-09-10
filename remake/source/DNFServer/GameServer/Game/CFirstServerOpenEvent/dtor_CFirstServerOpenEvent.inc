CFirstServerOpenEvent::~CFirstServerOpenEvent() {}

// ORIG 0x0811cbd8：显式调基类 IsEventing（call 0x080c84fa；实参 0）。
// 返回形态对齐 ORIG xor $1/test/je/mov0/mov1（P22/P59：bool 取反 guard）。
