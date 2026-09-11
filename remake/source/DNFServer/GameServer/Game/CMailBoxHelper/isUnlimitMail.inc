// ORIG 0x0855254c：遍历 SIG_MAILBOX_LIST 无限信列表（count @+0x794 单 disp，
// 条目步长 0x61；条目基 +i*0x61：+0x5c dword=信件 id，+0x5b byte=无限标志）。
// 视图（P82/P95，同 DB_MailBox_Req_List.cpp IsContainedLetter 先例）：
// count 单 disp；条目为 m_postals[i].field——4.4 出 imul/lea + add $0x50
// 数组基址 + 元素内 disp（ORIG 0x8552561 `add $0x50; mov 0xc(%eax)` 拆段）。
// 返回类型 unsigned char（ORIG 尾 `movzbl (%eax),%eax` 单段直返，P9）。
// [b28d-L8] N18b/N4 | ORIG 两段 mov 0xc(%eax),%edx + cmp %eax,%edx；4.4 折单段 cmp mem（负例表 N18b）；装载序互换 P211 族（ORIG 0x08557e6x）
// [b28d-solo-CMailBoxHelper] N 封顶（N7+N102+N18b 组合，ORIG 0x0855254c）：
// 已修 = 子对象嵌套视图 + `0x61*i + info` 基址序（i 先求值，lea 形与 ORIG 全等）。
// 残余三族实测不可翻：①装载序两条 mov 互换（ORIG i-first/4.4 info-first，N7/P75）；
// ②ORIG 两级 `add $0x50,%eax; mov 0xc(%eax)` 被 4.4 恒折 `mov 0x5c(%eax)`
//   （成员访问/取子对象地址/指针算术三形实测，N102/N4）；
// ③ORIG 比较两段 `mov mem,%edx; mov arg,%eax; cmp %eax,%edx` 被 4.4 折
//   `cmp mem,%eax` 单段（N18b，交换操作数无效）。31→28 insn。
// [2026-09-09 SweepCMailBoxHelper-2 翻案，本件 identical] 旧判词「N102/N4 add-vs-disp
//   不可翻 + N18b cmp 折单段」全部作废，两处根因均为**访问形式**而非编译器：
//   ①cast 视图形（无论算术基址还是 reinterpret_cast 本地 struct）恒 info-first 且
//     折 disp；②比较写成 `letterId == 字段` 会因同型折 `cmp mem,%eax`。
//   正解 = 权威头 SIG_MAILBOX_LIST 具名嵌套视图（参数类型自身成员，P235）+
//   **字段在左**的比较（m_letterId 为 unsigned int、形参 int 异型 ⇒ P274 两段装载）：
//   `info->m_box.m_postals[i].m_tail.m_occ.m_letterId == letterId`。
//   探针 /tmp/p_mail/Game/CMailBoxHelper.cpp fN（4.4.6 路由）与 ORIG 0x0855254c
//   31 条逐条同形：i-first → imul $0x61 → lea → add $0x50 → mov 0xc(%eax),%edx
//   → mov 0xc(%ebp),%eax → cmp %eax,%edx；命中段 add $0x5b → movzbl (%eax)；
//   循环条件 setg 物化。OccSub.m_unlimit@+0x5b 为同波 SigTypes_D.h 字段化新增。
bool isUnlimitMail(SIG_MAILBOX_LIST const* info, int letterId)
