int Dispatcher_GrowthCreatureChangeInfinityCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e8768 orig/ours=49/58 ndiff-blocks=8
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%eax | mov    &_ZN10GlobalData15s_event_managerE,%eax
