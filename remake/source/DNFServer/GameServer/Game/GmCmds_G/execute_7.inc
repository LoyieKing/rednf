// 取 0x32 号分发器，手组掉落测试包（finalize(false)+set_index(0xf)）后
// 经其虚表 [0] 入口 (this, user, PacketBuf*) 投递。
void CDropItem::execute()
