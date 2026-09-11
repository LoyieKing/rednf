// ============================================================================
// DB_LoadCharac::DB_LoadCharac @0x844d8c6（C1/C2 同址）
// ORIG：调 DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1/D2 同址 0x844d912 设 vptr→基类空 dtor→恒假 delete
// 分支；D0 0x844d912 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
DB_LoadCharac::DB_LoadCharac()
{
}