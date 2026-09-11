// ============================================================================
// _InsertLetter —— ORIG 0x084435d4：escape(+0x1e 名)→0x1ff 缓冲、escape(+0x00
// 正文)→0x3d 缓冲 → set_query(letter insert 6 列) → exec → GetIdentity 回填
// +0x39d → 1。
// b28c-34（已翻正）：+0x3a5 stat 实参的 ORIG 两段装载
//   `mov 0x3a5(%eax),%eax; mov %eax,%ebx`（0x08443693/0x08443699）由 m_stat 视图
//   字段 unsigned int 化 + 保留 (int) 实参异型化复现（P211 异型化；探针
//   /tmp/c28l34_probe3.cpp q 实证同形）。语义结论（推断）：stat 原码类型不可由
//   二进制区分，异型化仅触发 4.4 的转换中转，store 宽度/偏移/mangled 不变。
// b28c-34 判词 N（N76 族：4.1 RHS 值先载 vs 4.4 LHS 地址先载）：尾部 +0x39d
//   回填 ORIG 0x08443711..0x08443719 为
//   `call GetIdentity; mov 0x10(%ebp),%edx; mov %eax,0x39d(%edx)`；
//   4.4 对 cast 视图 LHS 恒先求地址并缓存 callee-saved：
//   `mov 0x10(%ebp),%ebx; …; call GetIdentity; mov %eax,0x39d(%ebx)`。
//   已试形 20+（探针 /tmp/c28l34_probe{1..10}.cpp）：-> / (*p). / p[0]. /
//   (V&)*p / reinterpret_cast / *(int*)&视图 / 语句表达式（整体与 RHS）/
//   条件式 / 逗号 / 返回赋值结果 / always_inline setter / throw()/nothrow 声明
//   —— 全部 LHS-first；唯一复现 ORIG 形的写法是「形参具名成员」直访问
//   （探针 s_uint：`call; mov 0x8(%ebp),%edx; mov %eax,0x4(%edx)`），而共享头
//   SigTypes_D.h 的 SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 仅有 m_data[0x3a9] pad，
//   翻正需改共享头（本任务约束 5 禁止）⇒ 保持语义正确的 cast 视图访问。
// [b28d-N76] fresh 复验 105=105：残差 = 上述 LHS-first 装载序（唯一差异行），
// 已试形 20+ 均不可择（P211 边界：已异型件直接封顶）⇒ 维持 N76。
// [b28d-34R] 字段命名修正：letter 列 charac_no/send_charac_no 的 ORIG 实参序
//   （`mov 0x11e(%eax),%eax; … mov 0x122(%eax),%edx`，0x084436b0/0x084436a7）
//   确证 0x11e=收件人、0x122=发件人（与 _InsertPostal 的 postal 列互证）；
//   视图字段已按此改名，值序不变 ⇒ 本件汇编零变化。
// [M5E round6 复核 · 语义轴 + 装载序，105/105，唯余 2 条 .h 轴]
//  · 语义复验（逐条对 ORIG 实参槽）：charac_no←+0x11e、send_charac_no←+0x122、
//    send_charac_name←escape(+0x1e)、letter_text←escape(+0x00)、reg_date←+0x3a1、
//    stat←+0x3a5；`xor $1,%eax; test %al,%al; je` @0x08443702 = ORIG 的
//    `if (!db->exec(true)) return 0;`，尾 `+0x39d ← GetIdentity(db)` + `mov $1,%eax`
//    ⇒ 无字段错位、无极性错。
//  · 残余唯一 2 条：我方 +0x137 `mov 0x10(%ebp),%ebx`（在 call **前**）vs ORIG
//    +0x13a `mov 0x10(%ebp),%edx`（在 call **后**）。根因 = LHS 基址 tree 形：
//    逐句 `((SigSystemMultiMailView*)sigRaw)->m_letterId = ...` 的 NOP_EXPR 被 4.4
//    在 RHS 之前物化（探针 /tmp/m5e_lhs_probe.cpp：形 A 逐句 cast ⇒ call 前
//    `mov 0x8(%ebp),%ebx`；形 B 具名视图局部 ⇒ call 后 `mov -0xc(%ebp),%eax`，
//    即 ORIG 形）。但具名局部自身 +2 条（存+载）⇒ 净劣 106/105，不采纳。
//    真解 = .h（SigTypes_D.h 的 SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 仅
//    m_data[0x3a9] pad、无具名成员；ORIG 该形参类型自带 +0x39d 成员 ⇒ 基址为
//    普通 decl，4.4 直接 RHS-first）。.h 落地后本件 105/105 逐条相同。
// ============================================================================
bool DB_MailBox_Req_System_Multi_Mail::_InsertLetter(
    MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sigRaw)
{
    char titleBuf[0x1ff] = { 0 };
    db->escape_string(titleBuf,
                      sigRaw->m_sendCharacName);   // [M5] SigTypes_D 具名化后直访

    char nameBuf[0x3d] = { 0 };
    db->escape_string(nameBuf, sigRaw->m_letterText);

    db->set_query(
        "inSert into letter(charac_no,send_charac_no,send_charac_name,letter_text,reg_date,stat) values(%d,%d,'%s','%s',from_unixtime(%d),%d)",
        sigRaw->m_recvCharacNo,
        sigRaw->m_sendCharacNo, nameBuf, titleBuf,
        sigRaw->m_occTime,
        (int)sigRaw->m_stat);

    if (!db->exec(true))
    {
        return 0;
    }
    sigRaw->m_letterId = WongWork::DBCommon::GetIdentity(db);
    return 1;
}