// ORIG 0x82a7572：push ebp/mov esp,ebp/pop ebp/ret —— 空函数（Inven_Item 平凡
// 析构，编译器不发清理代码，与 ORIG 完全一致）。
// [b28d-Main·检查点14 A/B] throw()（同上）。
CDeathTower::CPlayData::~CPlayData() throw()
{
}