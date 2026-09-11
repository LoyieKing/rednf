// [SweepDBLogin 本轮] verdict=DIFF ndiff=4 gap=0（110=110；p167 仅 cMyTrace C1/C2 同址
// 别名豁免）。残差 = P196 折痕：ORIG `cmp $0x1,%eax; sete %al; test %al,%al; je`
// vs 我方 `setne/jne`（同一谓词的极性镜像，分支落点体相同，非真反）。
// 修法：`if (db->get_n_rows() == 1) { …body… } else { goto link_end; }` 改负条件
// goto 共享尾空标签（P223 多源汇入连续尾标签形）⇒ gap 1→0（旧 N121「尾 jmp+nop
// 不可复现」被本形推翻；nop 岛按 §6.1 对齐 nop 豁免已剥离）。
// 试过并列读数：正条件 then + else goto（gap=1/ndiff=1）、then 尾 goto（gap=1/ndiff=1）、
// 嵌套正极性（gap=1/ndiff=1）；`!(x==1)` 被 4.4 前端折成 `!=1` 恒产 setne ⇒ sete 侧
// 不可再翻（P196 折痕封顶）。
void DB_Login::getCharacLinkSystemData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // P48 no-CSE：ORIG 三处各自 fresh NumberToString()（两条 set_query + cMyTrace）
    // 与两处 fresh get_n_rows()。
    db->set_query("seLect message_flag from charac_link_message where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    // [b28c-L9] SIGFIX:DB_Login::getCharacLinkSystemData —— ORIG
    // __PRETTY_FUNCTION__ 串为 `void DB_Login::getCharacLinkSystemData(...)`，
    // 且尾 0x084101a0 `xor $1,%eax; test %al,%al; jmp exit`（值无分支消费者）
    // ⇒ 原码非「返回错误码」而是 void + 嵌套 if/else（旧 bool 早返形系误建模；
    // 0x084101a7 的 nop 是 4.1 为 n_rows!=1 空 else 发射的中继岛，P184/P223）。
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x2f54, "LINK_LOG : charac_link_message DB ERROR!!");
    }
    else if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into charac_link_message(m_id,message_flag) values(%s,0)",
                      NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x2f5d, "LINK_LOG : charac_link_message DB INSERT FAIL!!");
        }
    }
    else
    {
        // [Lane26 复核] P223：ORIG 0x08410112 `cmp $0x1; sete; test; je <nop 岛>`
        // + get_byte 段尾 0x084101a5 `jmp exit`（多源汇入连续尾标签）⇒ 负条件 goto
        // 共享尾空标签。判据①（jcc/jmp 目标下标逐位）：本形 bmm=0（全部跳转落点与
        // ORIG 同下标同方向）；唯一残差 = setcc 折痕镜像（ORIG sete/je vs 4.4
        // setne/jne，P196 等价，非真反）。
        // 【Lane26 对照实验】`while (db->get_n_rows() == 1) { body; goto link_end; }`
        // 可复现 ORIG 的 sete/je 物化（推翻旧「sete 侧不可翻」判词，P225 形有效），
        // 但 body 尾 `jmp exit` 被 4.4 jump-threading 恒消（N121；break/goto/return
        // 三形实测同），代价 = 少 1 条指令 + 9 处 jcc delta 位移。
        // [b28d-h9 P226 条件向实验·已回滚] 曾试 `if (!({ (n_rows() == 1); }))` 语句
        // 表达式翻 ORIG 的 `sete/je` 侧，实测读数**完全不变**（110/110/ndiff2）⇒
        // P226 在本点位无效，回滚。残余 2 行 = P196 折痕（`sete/je` vs `setne/jne`
        // 同落点体），按 P168a② 封顶。
        // [DBLogin-V3 非-bug 直证] ORIG 0x0841010a `cmp $0x1,%eax`；0x0841010d
        // `sete %al`；0x08410112 `je 0x084101a7`（+0x1dd nop 岛，落点体=fetch 块）。
        // 我方 .o 同点 `cmp $0x1,%eax`（+0x140）`setne %al`（+0x143）`jne <+0x1dd>`
        // （+0x148）：**同谓词、同落点偏移、同落点体** —— ORIG 在 rows!=1 时经
        // `sete`(al=0)+`je` 跳，我方经 `setne`(al=1)+`jne` 跳，两支条件语义全等
        // ⇒ 比较方向未反演、非真 bug（仅 4.1↔4.4 setcc 物化/前端折叠差，P196）。
        // [DBLogin-V3 实验A·已回滚] 嵌套正极性 if（无 else、无 goto）可复现 ORIG
        // `sete/je` 物化，但函数尾 `jmp exit` 被 4.4 与 return 落点合并消掉 ⇒
        // 109 vs ORIG 110（gap=1/ndiff=1），劣于本形（gap=0/ndiff=4，同判 DIFF）⇒ 回滚。
        if (!(db->get_n_rows() == 1))
        {
            goto link_end;
        }
        // [b28d-solo-DB_Login] P168a/P222：ORIG `call; xor $1; test; je <get_byte 段>`
        // ⇒ 失败(trace)块直落、成功块后置；旧 if(fetch) 正极性产 `test; je <trace>`。
        if (!db->fetch())
        {
            // P227：全表达式临时链 ⇒ NumberToString 结果在 ctor 前求值并跨 ctor
            // 保活 %ebx（ORIG push %ebx / mov %eax,%ebx）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2f65, 5)(
                "LINK_LOG : charac_link_message DB ERROR!! M_ID(%s)",
                NumberToString(sig->m_accId, 0));
        }
        else
        {
            if (!db->get_byte(0, reinterpret_cast<char&>(sig->m_characLinkMessageFlag)))
            {
                return;
            }
        }
    }
link_end:
    ;
}