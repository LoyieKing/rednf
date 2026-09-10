Dispatcher_BlueMarble::~Dispatcher_BlueMarble() {}
// b28d-solo-Dispatchers verdict=AE(routed) [P226/P0] ORIG 0x081e612c orig/ours=62/62
//   [S162] 已修：尾部具名 bool（fail = !(get_state()==0xb || ==5)）落 -0x9 并使
//   全帧槽号 +4；改条件表达式直用（ORIG 物化于 %al `test %al,%al`，P0 无槽往返）。
