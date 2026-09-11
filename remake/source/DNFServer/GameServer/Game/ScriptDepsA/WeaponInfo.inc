// ORIG WeaponInfo C1 W 0x08a3ce08：vector 构造后 clear()。D1 W 0x083dc13a 平凡
//（成员逆序析构由编译器生成）；copy 为逐成员语义（ORIG C1ERKS_ W 0x08a3dfca）。
WeaponInfo::WeaponInfo() { clear(); }
