// ORIG _ZN13user_creature4CEggC1Ev T 0x08338f4c（报告 CEgg.md 全汇编转写）：
// ① 基类 ctor（写 CCreatureItem vtable 0x8c2f4d8）
// ② vptr ← &_ZTVN13user_creature4CEggE+0x8（nm V 0x08c2f4a0 → +8 = 0x08c2f4a8）
// ③ Reset()
// 镜像以 int m_vptr 槽模拟多态头，无虚表生成。
// ORIG 只调一次 CCreatureItemC1（编译器隐式基类构造）；显式
// `CCreatureItem();` 会额外构造一次临时对象（NEW 反汇编 2 次 C1），删除。
CEgg::CEgg()
{
    // [b28d-Main·检查点14] vptr 由编译器自动写为 &_ZTVN13user_creature4CEggE+0x8
    //   （原手工常量已删）。
    Reset();
}