// ----------------------------------------------------------------------------
// ORIG W 0x08539de8 _ZNK12CAICharacter13IsHellMonsterEv（0xc）：
//   movzbl 0x70(this) 直返。mirror 类 +0x30..+0x6b 为 m_dropAvatarInfo，
//   +0x70 rarity 标志未具名建模，按字节视图读取。
// 返回类型 bool：ORIG 体单条 `movzx eax,byte`（无 0/1 规范化 ⇒ 视图成员本身
// bool），调用点 0x08535b08 `call;test %al,%al;je` 无 setne（P292）。
bool CAICharacter::IsHellMonster() const
