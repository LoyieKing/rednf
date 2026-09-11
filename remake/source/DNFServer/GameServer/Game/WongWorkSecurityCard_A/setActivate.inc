// W 0x084ebd82：mov %al 截断暂存; mov this; movzbl 回读; mov %dl,0x2f
// （P33/P136：bool 字段接收 bool 值直赋，禁止三元归一化/uchar 中转）。
void CSecurityCard::setActivate(bool flag)
