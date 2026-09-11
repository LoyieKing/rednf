// ---- ORIG 真实符号（自由函数 / 跨类方法，普通 C++ 声明，无 extern "C" 桥） ----
// _checkTimeoutItem（ORIG 0x867cf8d T）定义于 CDataManagerScripts.cpp。
// b28d-solo-CTitleBook（P248 档①）：ORIG 调用点 0x086412ba 为 `call; test %al,%al;
// jne`（bool callee 裸形）；原 int 声明使 4.4 生 `test %eax,%eax`。
bool _checkTimeoutItem(const Inven_Item& item);
