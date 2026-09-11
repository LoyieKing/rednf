// ORIG _Z18get_seed_from_datev T 0x086b1d25：year*365 + mon*30 + mday
// （ORIG 以 mon*32-mon 形态实现 *30；反汇编 0x086b1d53 读 tm_mon(+0x10)）。
int get_seed_from_date()
