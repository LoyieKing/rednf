// ============================================================================
// DB_LoadInventory::DB_LoadInventory @0x0840a604（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1 0x0840a620 设 vptr→基类空 dtor→恒假 delete 分支；
// D0 0x0840a650 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
DB_LoadInventory::DB_LoadInventory()
{
}