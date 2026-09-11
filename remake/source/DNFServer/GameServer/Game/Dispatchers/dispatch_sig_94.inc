// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x0820b17a orig/ours=108/116 ndiff-blocks=24
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | sub    $0xb20,%esp | ja     0820b1a9 <+0x2f>
int Dispatcher_Antibot_DPCallBack::dispatch_sig(CUser* user, PacketBuf& packet)
