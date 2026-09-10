Dispatcher_DeleteToBlackList::~Dispatcher_DeleteToBlackList() {}
// [b28d-solo-Dispatchers] P211 callee 侧：返回类型 int→unsigned int 使 process 调用点
//   store 翻为 ORIG 的 RHS-first+edx 中转（头声明同步，mangled 不变）。
