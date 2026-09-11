// ----------------------------------------------------------------------------
// _findIPCount —— ORIG T 0x080f9322 (0x148)：equal_range((u16)a) 遍历，
// 元素 m_b==b 时 Divide_C_ClassIP_EndIP 取 class-C 前缀，
// 前缀非空且 strncmp(prefix, entry.m_ip, 0xc)==0 → 返回 m_cnt；否则 0。
// ----------------------------------------------------------------------------
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N102/N117）→ 试=指针复用形 `char* pfx = prefix;`（A 变体实测：帧 0x58→0x68、insn 92→95、新增 -0xc 槽 ⇒ 回滚）；ORIG 0x080f940e `lea -0x48(%ebp),%eax; movzbl (%eax),%eax; test %al,%al` 为地址物化（4.1 pre-combine 不折），4.4 对 `prefix[0]` 恒折 disp ⇒ 维持。
int CAutoPunishRuleHackTypeMgr::_findIPCount(unsigned int a, unsigned int b,
