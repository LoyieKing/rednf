// ---------------------------------------------------------------------------
// CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA* data) T 0x08690226 sz0x1d1
// 报告：docs/class_func_reports/CUser/setTagCharacInven.md
// 前任 W7-BLOCKED 解除：调用目标已逐一 nm 解析（0x084e9990
// CInventory::SetClearAvatar(int,int)、0x084e99b4 SetClearAvatar(ulong*)、
// 0x084e91a4 GetItemIdByAddInfo(Inven_Item*,int,int)、0x08694a0c
// CCargo::SetCapacity(int)）。getTagCharacInvenW 返回 Inven_Item* 即
// CInventory 首址视图。TAG_CHARAC_INVEN_DATA 为 0x4024 字节 blob
//（SigTypes_C.h m_data），字段经 m_data+偏移具名成员索引访问。
// ---------------------------------------------------------------------------
int GetItemIdByAddInfo(Inven_Item* items, int space, int addInfo);   // W 0x084e91a4（owner：Item 域批次）
