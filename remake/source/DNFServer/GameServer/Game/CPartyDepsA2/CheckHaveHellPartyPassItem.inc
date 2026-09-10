// [b28d-round5] 返回类型 unsigned char→bool 与 CParty.h:493 同步（父泳道 R2 落地）：
// ORIG 0x085a7d4b `movzbl 0x1865(%eax),%ebx` 单条直落 ⇒ bool 成员+bool 返回无转换
// 临时；探针 /tmp/probe_party/s.cpp h4（bool 返回，单条）vs h1/h5（uchar 返回，两条）。
bool CParty::CheckHaveHellPartyPassItem(CDungeon const* dungeon,
