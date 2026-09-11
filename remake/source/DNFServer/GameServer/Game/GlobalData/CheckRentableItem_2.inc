// ORIG _ZN20InstanceRentalSystem17CheckRentableItemEij T 0x082702dc【objdump】：
// machine = getMachine(type)；machine==0 → 返 1（视为可用）；否则尾调虚槽
// vptr+0x10 CheckRentableItem(itemIdx)。（OnlyOrig_CreatureJob.EJEnchanter 批补）
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+伴随寄存对调，主体同形封顶。
bool InstanceRentalSystem::CheckRentableItem(int type, unsigned int itemIdx)
