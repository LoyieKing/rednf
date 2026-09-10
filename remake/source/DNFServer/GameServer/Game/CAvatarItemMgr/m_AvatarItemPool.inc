DynamicPool<Avatar_Item> CAvatarItemMgr::m_AvatarItemPool(15000, 1000);

// ORIG 布局（nm/报告实证）：m_registeredItems(+0x00)/m_waitAssignItems(+0x18)
// /m_tempKeyCounter(+0x30)/m_emblemInfo(+0x34)/m_emblemEnduranceMap(+0x54)
// /m_enduranceUpdateMap(+0x6c)，总 0x84（CInventory.h:783 权威注释）。
static_assert(sizeof(CAvatarItemMgr) == 0x84, "CAvatarItemMgr ABI size changed");

// ORIG D1/D2 0x0822d2e0：调 Reset() 后成员 map 自动析构（D1 汇编
// Reset → D1(+0x6c) → D1(+0x54) → D1(+0x18) → D1(+0x00)）。
