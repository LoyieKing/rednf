int getAvatarEmblemTypeIdx(unsigned short type);

// MSG_EPLP_COMMAND：ORIG 无独立导出符号，仅经 _BroadCastPacket 以引用访问。
// 报告反编译只读 +0x0d/+0x0e 两个命令字节（Dispatchers.cpp 定义同布局视图；
// 前 0xd 字节为包头区【推断】，不设 static_assert）。
