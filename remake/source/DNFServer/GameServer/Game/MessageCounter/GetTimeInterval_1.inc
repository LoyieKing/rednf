// ORIG @0x08570254：(now-last)_sec*1000 + (now-last)_usec/1000.0，
// FP 取整后转 long long 返回。
long long CUnixTimeInterval::GetTimeInterval()
