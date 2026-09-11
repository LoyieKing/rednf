// ---------------------------------------------------------------------------
// OnRecvMessageServerInfo                  ORIG 0x84931de
// stream buffer：+0x0a channel_no(int)、+0x0e port(short)、+0x10 ip(uint)
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] NEAR 判 N7：p167 CLEAR(17/17)；残差=callee-saved 角色
// （ORIG edi=channelNo/esi=port vs 4.4 esi/ebx）+ 缺 push %edi。真 bug 已修：
// SetServerType(MASTER) 移出内层 if（ORIG je 直达）。
int CExchangeServer::OnRecvMessageServerInfo(Stream* stream, int a,
