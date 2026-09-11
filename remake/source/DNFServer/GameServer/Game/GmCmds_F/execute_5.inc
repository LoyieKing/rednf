// ORIG execute 0x084ac6a0：向发起者所在公会代理发送加 fund 请求
// SendAddGuildFund(characNo, guildKey, amount, eFundType_GM)。
// [b28d-solo-GmCmds_F] N7 封顶（callee-saved 角色互换）：ORIG 0x084ac6a0 amount=%edi /
// guildKey=%ebx / characNo=%esi；4.4 按首次跨调用序分配 %edi→%esi→%ebx（guildKey=%esi、
// characNo=%ebx）。实测五形均不翻：①声明序 amount/characNo/guildKey ②初始化式三连
// ③register 子集（全删 / 仅 amount+guildKey）④guildKey unsigned 化 ⑤语句表达式升格。
// [补] ⑥4.4.6 后端（c6-g++-446r）与 4.4.7 逐条同形 ⇒ 非 P230 N7 敏感族，换路由无解；
// ⑦声明/赋值分离（register int a; a=…）声明序两向均不改分配（探针 /tmp/probe_guild2.cpp
// execA/execB/execC：A=B=edi/esi/ebx，无 register 全落栈）。
// [P1 round5] ⑧交换 guildKey/characNo 的**求值序**（characNo 先、guildKey 后）实测
//   **不翻**且更差：寄存器角色仍 characNo=%ebx/guildKey=%esi（4.4 分配序与该序无关），
//   反而把 `movl $<A>,0x4(%esp)`（eFundType_GM）提前 ⇒ NEAR→DIFF，已回滚。
// 全 register 形 40=40，仅两处寄存器名差，语义等价（SendAddGuildFund 实参序/符号一致）。
// ORIG 报告：docs/class_func_reports/game_master__CSetGuildFund/execute.md。
void CSetGuildFund::execute()
{
    CUser* user = GetUser();
    register int amount = m_fund;
    register int guildKey = user->get_charac_guildkey();
    register int characNo = user->get_charac_no(-1);
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendAddGuildFund(characNo, guildKey, amount,
                           Packet_Add_Guild_Fund::eFundType_GM);
}