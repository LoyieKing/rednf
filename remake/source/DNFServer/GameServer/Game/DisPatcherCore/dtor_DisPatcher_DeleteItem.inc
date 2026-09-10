DisPatcher_DeleteItem::~DisPatcher_DeleteItem() {}
// [b28d-Main·检查点14] 原 TU 局部视图 DeleteItemMsgTailView 已删除：
//   m_checkSum 现为 MSG_DELETE_ITEM 本体字段（+0x4bf，PacketTypes.h）。
// ORIG 0x081c1a6c T _ZN21DisPatcher_DeleteItem11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_DeleteItem/check_error.md 逐块映射）：背包校验和
// getCurCharacInvenCheckSum 与 msg+0x4bf 不一致 → addServerHackCnt(0x25b,1,0,0)；恒返 0。
