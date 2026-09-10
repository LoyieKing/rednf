Dispatcher_ValidateScriptHash::~Dispatcher_ValidateScriptHash() {}
// ORIG 0x08264364（报告 Dispatcher_ValidateScriptHash/get_hack_type.md）[随行依赖]：
// switch(1..5) → 0x344..0x348；0 与越界分支 ORIG 未初始化 local_8（忠实保留）。
