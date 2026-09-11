// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cf0a6 orig/ours=67/69 ndiff-blocks=9
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | movl   $&_ZZN39Dispatcher_SaveGameOption_QuickChatting4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTIO
int Dispatcher_SaveGameOption_QuickChatting::read(PacketBuf& packet, MSG_BASE& msg)
