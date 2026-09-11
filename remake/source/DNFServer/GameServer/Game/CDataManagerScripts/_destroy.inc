// ORIG _destroy 0x8581784：遍历 hash_map delete 各 CNPCScript*（调 D1 + operator delete），
// 再 clear()。
// [b28d-solo-CDataManagerScripts] AE：①缓存 end 转换构造 `const_iterator e(m_npcMap.end())`（P13）；②无条件 `delete it->second`（P141 删显式判空，消 -0xc 槽）⇒ 帧/槽与 ORIG 对齐；ORIG 0x08581784。
void CNPCScriptList::_destroy()
