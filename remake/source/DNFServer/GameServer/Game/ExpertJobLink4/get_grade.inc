char CDisjointMachine::get_grade() const { return m_field20; }     // +0x20 movzbl

// ---------------------------------------------------------------------------
// CDisjointer —— LinkMisc5 收口（vtable 重排后按虚槽落地）
// ---------------------------------------------------------------------------

// send_error（报告 send_error.md，ORIG T 0x085d1e62）：
// header(0,0xcb)/byte(0)/byte(err)/finalize/Send。
