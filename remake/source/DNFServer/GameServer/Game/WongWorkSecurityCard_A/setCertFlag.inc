// W 0x084ebd9a：char 形参经 movzbl 截断直写字节 +0x31
// （字段为 uchar 字节存储语义；bool 字段会让 char→bool 走 cmpb/setne 规范化，
//  与 ORIG movzbl 直写不符，故保持 uchar）。
void CSecurityCard::setCertFlag(char flag)
