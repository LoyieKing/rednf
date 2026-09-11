// [b28d-29] N6/N22：ORIG 29 条为 (1<<state)&0x15e8 无槽寄存器形；4.4 折叠成 (mask>>state)&1（N6）或槽化（P0），三形实测 33/34 均不达 29  (ORIG 0x081c619c insn 29->34)
DisPatcher_RepairEquip::~DisPatcher_RepairEquip() {}
