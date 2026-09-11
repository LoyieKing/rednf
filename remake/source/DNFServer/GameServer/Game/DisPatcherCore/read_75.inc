// ORIG 0x0812be82 T _ZN33DisPatcher_EventDungeon_ClearRoom4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/read.md 全文转写）：
// ORIG 汇编含 &msg 的局部落栈（-0x4c(%ebp) spill/reload 对），以具名局部复现。
int DisPatcher_EventDungeon_ClearRoom::read(PacketBuf& packet, MSG_BASE& msg)

{
    char* pBuf = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    packet.get_binary(*(&pBuf), 0x14);            // 同上：reload 先于 len 入栈
    return 0;
}
