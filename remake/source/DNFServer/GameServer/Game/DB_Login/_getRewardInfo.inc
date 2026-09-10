// ============================================================================
// _getRewardInfo（ORIG T 0x084115f8）—— member_security_grade 安全等级汇总
// ============================================================================

// [SweepDBLogin 本轮] verdict=AE ndiff=0 gap=0（330=330 逐条归一全等；p167 CLEAR 18/18）。
// 历史 [S216_DB_Login2] verdict=DIFF ndiff=382 gap=44 —— 本轮修法（四类，均为真缺口）：
// ①ORIG `if (get_n_rows() != 0) { …主体… }` + 函数尾共享 `return 0`（early-return else
//   形 4.4 恒就地物化 `mov $0,%eax; jmp`）⇒ 主体包进 if 块、尾 return 0 共享；
// ②time 段复用 now 槽：`now = now - dayOff - 0x2a30; if (dayOff>0x1274f) now += 0x15180;`
//   （删 base 具名局部；ORIG `sub $0x2a30` 立即数形 + else `add $0x15180`）；
// ③validIp 补 `= {0}`（ORIG 0x08411877..0x08411893 四条 movl 预清零，旧注释误判为无）；
// ④flags/latest 局部删除，改 `sig->m_secuType/m_rewardType |= N` 字段读改写（ORIG 11 处
//   `movzbl 字段; or $imm,%edx; mov %dl`），col[9] 数组改 9 个独立具名标量（数组下标取址
//   4.4 恒 lea+add，独立标量折单条 lea；ORIG 0x08411732 九条 movl 清零 + 九个单条 lea）。
// 余：0（唯一槽号差 -0x1c..-0x3c 由独立标量声明序精确复现）。
int DB_Login::_getRewardInfo(SIG_LOGIN_DATA* sig)
{
    sig->m_secuType = 0;          // +0x3f0d 注册位图
    sig->m_secuReward = false;    // +0x3f0e 重新校验标记
    sig->m_rewardType = 0;        // +0x3f0f 最新注册类型位
    sig->m_passPadEnable = 0;
    sig->m_passPadCertified = 0;
    sig->m_secCardActivate = false;     // +0x39e0
    sig->m_secCardCertified = false;    // +0x39e1

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect unix_timestamp(last_check_time), unix_timestamp(m_opt_reg), unix_timestamp(pc_opt_reg), unix_timestamp(security_card_reg), unix_timestamp(goblin_pass_mod), unix_timestamp(member_pc_reg), unix_timestamp(gatekeeper_otp_reg), goblin_validity_time>unix_timestamp(now()), security_card_validity_time>unix_timestamp(now()), validity_ip from member_security_grade where m_id = %s ",
        NumberToString(sig->m_accId, 0));
    // [b28d-solo-DB_Login] N-P133：ORIG 两守卫为**寄存器直判**
    // （`call exec; xor $1,%eax; test %al,%al; je` + `call get_n_rows; setne %al;
    //  test %al,%al; je`，无槽）；4.4 对 bool callee 与具名 bool 局部恒插
    // `mov %al,-N; movzbl` 两段（拆独立守卫形实测 244→254 反劣，已回退）
    // ⇒ 保持合并短路 + execOk 具名槽（ORIG 槽深差 -0x14 vs -0x24 同属 N82）。
    if (!db->exec(true))
    {
        return 0;
    }
    // [SweepDBLogin 本轮] ORIG 0x084116c4..0x084116cb `setne %al; test %al,%al; je <尾>`
    // ⇒ 原码是 `if (get_n_rows() != 0) { …主体… }` + 函数尾共享 `return 0`（early-return
    // else 形 4.4 恒就地物化 `mov $0,%eax; jmp`，与 ORIG 的 je 直跳尾块不符）。
    if (db->get_n_rows() != 0)
    {

    // 以 KST 日界（-0x2a30/+0x12750=86400 边界）推 last_check_time 基准
    // [SweepDBLogin 本轮] ORIG 复用 now 槽（无 base 具名局部）：`mov -0x18,%eax;
    // sub -0x10,%eax; sub $0x2a30,%eax; mov %eax,-0x18`，else 分支为
    // `mov -0x18,%eax; add $0x15180,%eax; mov %eax,-0x18`（= +0x12750 等价形，P0）。
    time_t now;
    time(&now);                       // ORIG `lea -0x18(%ebp); mov (%esp); call` 传址形
    int dayOff = (int)(now % 0x15180);
    now = now - dayOff - 0x2a30;
    if (dayOff > 0x1274f)
    {
        now += 0x15180;
    }

    // [SweepDBLogin 本轮] ORIG 0x08411732..0x0841176a = 9 条独立 `movl $0x0,-0x1c..-0x3c`
    // 且每个 get_uint 的取址实参为单条 `lea -0xNN(%ebp),%eax`（无 add）⇒ 原码是 9 个
    // **独立具名标量**（数组下标/结构体成员取址在 4.4 恒生 lea+add，P30/N4；旧 for 循环
    // 形另占 i 槽）。槽序照 ORIG 浅→深：-0x1c lastCheck … -0x3c secCardValid。
    unsigned int lastCheck = 0;      // -0x1c
    unsigned int mOpt = 0;           // -0x20
    unsigned int pcOpt = 0;          // -0x24
    unsigned int secCard = 0;        // -0x28
    unsigned int goblin = 0;         // -0x2c
    unsigned int memberPc = 0;       // -0x30
    unsigned int otp = 0;            // -0x34
    unsigned int goblinValid = 0;    // -0x38
    unsigned int secCardValid = 0;   // -0x3c
    if (!db->fetch())
    {
        return 0;
    }
    db->get_uint(0, lastCheck);                     // last_check_time
    db->get_uint(1, mOpt);                          // m_opt_reg
    db->get_uint(2, pcOpt);                         // pc_opt_reg
    db->get_uint(3, secCard);                       // security_card_reg
    db->get_uint(4, goblin);                        // goblin_pass_mod
    db->get_uint(5, memberPc);                      // member_pc_reg
    db->get_uint(6, otp);                           // gatekeeper_otp_reg
    db->get_uint(7, goblinValid);                   // goblin_validity>now()
    db->get_uint(8, secCardValid);                  // sec_card_validity>now()

    // [SweepDBLogin 本轮] ORIG 0x08411877..0x08411893 实证有 4 条 `movl $0x0,
    // -0x4c..-0x40` 预清零（旧注释「无预清零」系误判）⇒ `= {0}`。
    char validIp[16] = {0};
    char ipMatch = 1;
    db->get_str(9, validIp, 0x10);    // ORIG：get_str 直接覆盖，无预清零
    if (strncmp(validIp, SGBC(sig, OFF_LOGIN_IP), 0x10) != 0)
    {
        ipMatch = 0;
    }

    if (goblin != 0)
    {
        sig->m_passPadEnable = 1;     // goblin_pass_mod → mousepass 区有效位
    }
    if ((goblinValid != 0) && (ipMatch != '\0'))
    {
        sig->m_passPadCertified = 1;       // +0x390e mousepass 换算已注册
        sig->m_passPadVersion = 2;                  // +0x390c（ORIG 单条 disp store）
    }
    if (secCard != 0)
    {
        sig->m_secCardActivate = true;              // +0x39e0 security_card_reg 行存在
    }
    if ((secCardValid != 0) && (ipMatch != '\0'))
    {
        sig->m_secCardCertified = true;             // +0x39e1 有效期内
    }

    // [SweepDBLogin 本轮] ORIG 每处均为**字段读改写**（`movzbl 0x3f0d; mov %eax,%edx;
    // or $imm,%edx; mov %dl,0x3f0d`，共 11 处 movzbl）⇒ 无 flags/latest 具名局部（P0）。
    sig->m_secuType = 0;                             // ORIG 0x0841193a `movb $0x0,0x3f0d`
    if (mOpt != 0) sig->m_secuType |= 1;             // m_opt
    if (pcOpt != 0) sig->m_secuType |= 2;            // pc_opt
    if (secCard != 0) sig->m_secuType |= 8;          // security_card
    if (goblin != 0) sig->m_secuType |= 0x10;        // goblin_pass
    if (memberPc != 0) sig->m_secuType |= 4;         // member_pc
    if (otp != 0) sig->m_secuType |= 0x20;           // gatekeeper_otp

    if (mOpt != 0) sig->m_rewardType |= 1;
    else if (pcOpt != 0) sig->m_rewardType |= 2;
    else if (memberPc != 0) sig->m_rewardType |= 4;
    else if (otp != 0) sig->m_rewardType |= 0x20;
    if (secCard != 0) sig->m_rewardType |= 8;
    else if (goblin != 0) sig->m_rewardType |= 0x10;

    if (sig->m_secuType != 0 && (int)lastCheck < (int)now)
    {
        sig->m_secuReward = true;                    // 需要重新校验标记
    }
    }                                                // if (db->get_n_rows() != 0)
    return 0;                                        // ORIG 恒返 0（布尔由 sig 字段承载）
}
