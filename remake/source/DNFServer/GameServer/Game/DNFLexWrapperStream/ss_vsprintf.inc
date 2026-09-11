// ORIG 变参格式化助手：弱模板 _Z10ss_sprintfILj1024EEiRAT__cPKcz @0x8acf0e0
// （紧接本 TU switchChannelInputStream 之后，自然宿主即本文件）。ORIG 体为
// ss_vsprintf(buffer, N, format, ap)；ss_vsprintf @0x8ad457c 的语义：
// n=vsnprintf(buf,size,fmt,ap)；size>n 返回 n，否则清 buf[0] 返回 0（截断
// 报错形）。声明不定义，宿主属字符串工具批次。
int ss_vsprintf(char* buffer, unsigned int size, const char* format,
