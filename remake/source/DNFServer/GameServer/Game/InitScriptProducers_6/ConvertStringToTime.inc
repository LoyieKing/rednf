// ----------------------------------------------------------------------------
// ConvertStringToTime —— ORIG T 0x088c2341。"YYYY-MM-DD HH:MM:SS"（19 字符）
// → mktime。substr(0,4)/(5,2)/(8,2)/(11,2)/(14,2)/(17,2) 各 atoi 后直接写入
// 栈上 struct tm 对应槽位（tm_sec..tm_year；wday/yday/isdst 不初始化，ORIG 如此，
// mktime 会归一化）。长度非 19 返回 false；成功写 out 并返回 true。
// ----------------------------------------------------------------------------
bool ConvertStringToTime(std::string s, long& out)
