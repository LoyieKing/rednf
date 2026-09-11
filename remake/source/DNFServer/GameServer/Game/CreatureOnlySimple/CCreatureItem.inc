// 【objdump】ORIG _ZN13user_creature13CCreatureItemC1Ev W 0x08340c82（C2 同址，
// size 0x0e）：movl $0x8c2f4d8,(%eax) —— 写 CCreatureItem vtable 指针槽。
// [b28d-Main·检查点14] 真多态化后由编译器自动发射该 vptr 写（探针 /tmp/vt_probe.cpp）。
CCreatureItem::CCreatureItem()
{
}