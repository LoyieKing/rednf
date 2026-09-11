// ---- RandomItemFairy（ORIG C1/C2 W 0x8a3db5e / clear W 0x8a3db72）----
// C1：仅调 clear()（无成员构造，纯标量）；clear：+0 movl $0 直写 →
// 循环 i=0..2（setle）清 +4+i*4。D1/D2 W 0x8a4f436 空体（头内联）。
RandomItemFairy::RandomItemFairy()
{
    clear();
}