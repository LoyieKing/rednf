// ---- ORIG @0x086b1d86 _Z7in_timeP2tmS0_ ------------------------------------
// t1 = mktime(begin); t2 = mktime(end); now = time(NULL);
// return now >= t1 && now < t2;
bool in_time(struct tm* begin, struct tm* end)
