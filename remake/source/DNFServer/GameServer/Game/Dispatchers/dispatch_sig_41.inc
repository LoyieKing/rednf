// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08225394 orig/ours=134/141 ndiff-blocks=14
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: movl   $&_ZZN24Dispatcher_ConnectP2PPvP12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN24Dispatcher_ConnectP2PPvP12di
int Dispatcher_ConnectP2PPvP::dispatch_sig(CUser* user, PacketBuf& packet)
{
	if (user->get_state() < 2)
		return LineFunc(0xe016, __PRETTY_FUNCTION__, 0, 0);
	char count = -1;                                 // -0x11（ORIG 0xff 初始化）
	if (!packet.get_byte(count))
		return LineFunc(0xe01a, __PRETTY_FUNCTION__, 0, 0);
	if (count > 8 || count < 0)
		return LineFunc(0xe01e, __PRETTY_FUNCTION__, 0, 0);
	// ORIG 0x08205xxx：`movl $0x0,-0x19(%ebp); movl $0x0,-0x15(%ebp)` ⇒ conn 只有
	// **8 字节**（-0x19..-0x12），按两个 dword 清零；**没有** `conn[8]=count` 那次
	// 存储（旧重建多写一字节把数组撑到 9B ⇒ 基址/槽号整体错位 1）。count 只经
	// 第 5 实参传出。清零形实测：① `memset(conn,0,8)` → 4.4 发 **call**（133/136）；
	// ② `union {char[8]; int[2];}` + 逐 dword 写 → 两条 movl 但 union 对齐 4 把 conn
	// 顶到 -0x1c（8 处槽号差 3，133/133 仍 DIFF）；③ int 视图下标写 → 产 lea+mov
	// 两段（133/136）；④ 取 `char conn[8] = { 0 }`（4.4 对 8B 聚合零初始化内联为
	// 两条 movl，且 char 数组对齐 1 ⇒ 槽基与 ORIG 同）。
	char conn[8] = { 0 };                            // -0x19..-0x12
	char seat = -1;                                  // -0x1a（ORIG 0xff 初始化，循环外一次）
	for (int i = 0; i < count; ++i)                  // -0xc
	{
		if (!packet.get_byte(seat))
			return LineFunc(0xe025, __PRETTY_FUNCTION__, 0, 0);
		if (seat > 7 || seat < 0)
			return LineFunc(0xe028, __PRETTY_FUNCTION__, 0, 0);
		if (!packet.get_byte(conn[(int)seat]))
			return LineFunc(0xe02b, __PRETTY_FUNCTION__, 0, 0);
	}
	PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
	if (room == 0)
		return 0;
	if (room->m_matching == 0)                       // +0x6e4 matching 子对象指针
		return 0;
	// ORIG：对 room->m_matching（+0x6e4）虚表 +0x5c 槽 thiscall：
	//   (match, user, room, conn, count)。与 DisPatcherCore.cpp:10214 / PvP_Room.cpp:456 同款直调口径。
	(*(int (**)(void*, void*, void*, char*, char))(*(char**)room->m_matching + 0x5c))(
		(void*)room->m_matching, user, room, conn, count);
	return 0;
}