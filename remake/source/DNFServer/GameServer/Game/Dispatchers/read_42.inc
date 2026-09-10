int Dispatcher_Back2Village::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// process 依赖（清单外）：清 param.m_8=0；GetPartyIndex 找不到队伍 → cMyTrace
// "Party == NULL"，返回 8；否则 0。（声明需入 Dispatchers.h，见 NEEDS_DECL）
