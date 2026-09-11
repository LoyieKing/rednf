// ============================================================================
// insertMailData —— ORIG 0x0841f2fa（static bool）：
//   getCurSec → localtime_r(+1h,min/sec=0) → mktime → 按 m_flag/m_nB 折算到期
//   → m_nType!=0 且 !=last_letter_id 时 upDate letter → last_letter_id 缓存
//   → m_titleLen!=0 且 letterId==0 时 InsertLetter（失败 logFormat 0x550c），
//     若 m_addInfo==0 && m_destCharacNo==0 直接返 1
//   → 拷贝 senderName → blob_to_str(0,+0x3a,10)/(1,+0x47,0xe) →
//     set_query(postal insert 20 列) → exec。
// ============================================================================
// b28d-24: 291/291 全等（AE）——旧 b28d-21「N 封顶」三处残差经 P211 档③ + P321 S1
// 类型旋钮翻正（各形实测于 /tmp/p_imd.cpp、/tmp/p2.cpp、/tmp/p7.cpp 探针）：
// ① 减法段 LHS 异型化（(unsigned int)t）⇒ ORIG 0x0841f389..0x841f3a1
//    `mov -0x1c,%edx; imul; mov %edx,%ecx; sub %eax,%ecx; mov %ecx,%eax; mov %eax,-0x1c`
//    （同型 (int) 形 4.4 恒折 `sub %eax,-0x1c(%ebp)`）；
// ② 加法段 `(unsigned int)t` ⇒ ORIG 0x0841f3bd..0x841f3ce
//    `imul $0x15180,%eax,%edx; mov -0x1c(%ebp),%eax; lea (%edx,%eax,1),%eax`
//    （同型形恒折 `add -0x1c(%ebp),%eax`）；
// ③ 视图 m_senderCharacNo int→unsigned long（P321 T1 直证 32 位读）⇒ 同字段同时
//    复现 InsertLetter 实参两段 `mov 0x1e(%eax),%eax; mov %eax,%edx`（0x0841f46c）
//    与 set_query vararg 单段 `mov 0x1e(%eax),%edx`（0x0841f67f）。
// p167 CLEAR、jcc 17/17 逐位同、指令数 291=291。
bool DB_MailBox_Req_Send::insertMailData(MySQL* db, const SIG_MAILBOX_SEND_NEW_MAIL* sigRaw)
