// ncl_make_date(char*) —— 当日 "YYYYMMDD"（8 位数字）。
// ORIG：time → localtime_r → sprintf("%04d%02d%02d", year+1900, mon+1, mday)
// （rodata 0x8ca6a1a="%04d%02d%02d"；tm 字段经返回指针访问，P43）。
void ncl_make_date(char* out)
