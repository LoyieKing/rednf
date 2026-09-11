// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081ce8bc orig/ours=51/54 ndiff-blocks=6
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
int Dispatcher_SaveCharacterOption::read(PacketBuf& packet, MSG_BASE& msg)
