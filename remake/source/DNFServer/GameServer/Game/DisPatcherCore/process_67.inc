int DisPatcher_ReportClient4Hack::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233742 W _ZN28DisPatcher_ReportClient4Hack7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x082041a0，见 docs/class_func_reports/DisPatcher_ReportClient4Hack/dispatch_sig.md)

// ---------------------------------------------------------------------------
// ST_Guild_War_Rank_Info / Packet_Notice_Guild_War_Info 生命周期（StubS3C）。
// ST ctor：ORIG W 0x0822ba9a —— uint(+0)=0、uint(+4)=0、ushort(+8)=0、
// memset(+0xa,0,0x17)。Packet_Notice ctor：ORIG W 0x0822badc ——
// PacketHeader(0x414,0x15c) 后对 +0x12 起 9 个元素（stride 0x21）逐个构造。
// ---------------------------------------------------------------------------
