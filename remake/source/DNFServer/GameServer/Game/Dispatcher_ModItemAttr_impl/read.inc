// ORIG 0x082335d0 W _ZN22Dispatcher_ModItemAttr4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x082335d0..0x082335da）；ORIG 的包读取在
//   dispatch_sig 内完成（get_short/get_int 直读 PacketBuf），read 不承担职责。
int Dispatcher_ModItemAttr::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;
}