Dispatcher_SeriaRoom_Ani_Deco_Event::~Dispatcher_SeriaRoom_Ani_Deco_Event() {}
// b28c-L20: 正极性 bool 分支（P168a①：ORIG 0x081e8862 无 xor 取反，`test %al; je <mov $0 岛>`
//   fall-through return 1）；GetRepeatEvent 结果右值内联（P0，无槽往返）。
