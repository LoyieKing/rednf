void CUser::add_inventory_item(unsigned int itemId)
{
    	CItem* pItem = G_CDataManager()->find_item(itemId);
	if (!pItem)
		return;
	Inven_Item invenItem;
	pItem->make_item(invenItem);
	invenItem.m_addInfo2 = 1;  // count
	invenItem.m_addInfo = itemId;  // item id
	// ORIG 0x0865cad2/0x0865cae6：slot 先以 -1 初始化（movl $0xffffffff,-0xc），
	// 调用回写后 `== -1` bool 物化比较（cmpl/sete/test/jne）。
	int slot = -1;
	slot = getCurCharacInvenW()->insertItemIntoInventory(
	    invenItem, (eItemAddReason)0x18, true, true);
	// b28d-solo-CUser：ORIG 0x0865cae6 `cmpl; sete %al; test %al,%al; jne`（bool 物化，
	//   P295 while 形复现；`(bool)(x==-1)` 直折 je 无 sete）。
	while (slot == -1)
		return;
	// ORIG 0x0865cb19：MakeItemPacket 成功则 finalize+Send（0xe/byte 0/short 1）
	PacketGuard packet;
	((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
	((InterfacePacketBuf*)&packet)->put_byte(0);
	((InterfacePacketBuf*)&packet)->put_short(1);
	// ORIG 0x0865cb??：`call; xor $1,%eax; test %al,%al; jne 尾` = if(!f()) goto
	// 尾出口反形（4.4 正向 if 生 test/je 无 xor）。
	if (!getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet))
		goto done;
	((InterfacePacketBuf*)&packet)->finalize(true);
	Send(packet);
done:;
