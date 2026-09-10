int Dispatcher_GenCeraTicket::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// ORIG W 0x0821e474：int cnt = 0; return cnt;
// ---- Dispatcher_GiveGiftToNPC 生命周期与分发表（ORIG W 实证）----
// ctor W 0x08596a9c：基类 + vptr；dtor W 0x08241332/0x08241362：vptr + 基类析构。
