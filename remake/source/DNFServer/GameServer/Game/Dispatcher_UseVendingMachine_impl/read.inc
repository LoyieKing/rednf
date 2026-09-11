// ORIG 0x08233db2 W _ZN28Dispatcher_UseVendingMachine4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x08233db2..0x08233dbc）；ORIG 的包读取在
//   dispatch_sig 内完成（get_int/get_int/get_short 直读 PacketBuf）。
int Dispatcher_UseVendingMachine::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;
}