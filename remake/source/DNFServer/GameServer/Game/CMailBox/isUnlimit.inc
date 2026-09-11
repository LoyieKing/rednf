// ORIG 0x08557590 (W)：movzbl 直返（bool 字段直返无归一化，P9 勘误）+
// 消费端 0x08552486 `call; xor $0x1,%eax; test %al,%al; je`（P292 bool 形）。
bool CMailBox::CMail::isUnlimit() const { return m_unlimit; }
