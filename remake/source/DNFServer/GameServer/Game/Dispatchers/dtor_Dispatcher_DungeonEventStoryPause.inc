Dispatcher_DungeonEventStoryPause::~Dispatcher_DungeonEventStoryPause() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08216dcc orig/ours=163/167 ndiff-blocks=29
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | lea    -0x19(%ebp),%eax | movl   $&_ZZN33Dispatcher_DungeonEventStoryPause12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,
