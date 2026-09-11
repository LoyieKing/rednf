// ORIG T 0x086b1a7e get_str_time：sep==0 → "%02d%02d%02d"（时/分/秒）；
// 否则 "%02d%c%02d%c%02d"（实参顺序 tm_hour(+0x8)/tm_min(+0x4)/tm_sec(+0x0)）。
void get_str_time(char* out, int c, char sep)
