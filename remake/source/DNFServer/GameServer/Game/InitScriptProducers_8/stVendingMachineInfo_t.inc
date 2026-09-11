// ORIG C1 0x0891b36a：三个 vector 成员默认构造后调 clear（尾部 call 0x891b3fe）。
stVendingMachineInfo_t::stVendingMachineInfo_t()
{
    clear();
}