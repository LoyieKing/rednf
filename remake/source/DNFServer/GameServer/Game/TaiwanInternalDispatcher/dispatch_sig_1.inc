// ============================================================================
// OnlyOrigBatch14：InterSelectMobileAuthReward::dispatch_sig
// ORIG T 0x0816132a（0x283）。buf 承载 Msg*。
// 注意：ORIG 0x08161384 处为无条件 jmp（E9 04 02 00 00 → +0x263），
// 发邮件主体（make_pair/findString/ReqDBSendNewSystemMultiMail/SendReward）
// 在 ORIG 中为不可达代码；执行路径仅回写 user+0x8ec31 = m_rewarded。
//
// 【N84 归因（batch-11 实测）】we-smaller 143 行 = ORIG 4.1 -O0 对
// `if ((msg->rewarded ^ 1) != 0 && <const-false>)` 型条件保留首项求值
// （0x08161378..58 movzbl/xor $1/test）+ 无条件 jmp + **文本发射不可达 body**
// （+0x79..+0x262：vector pair/findString×2/strncpy×2/9-loop/
// MakeSystemMultiMailPostal/ReqDBSendNewSystemMultiMail/GetRepeatEvent/SendReward
// + EH vector dtor cleanup）。gcc 4.4 -O0 对同形条件整体折叠（/tmp/ehtest.C
// 探针：`if(msg && (r^1) && 0)` 仅输出 tail+mov$0，首项求值与 body 全消）；
// volatile 守卫探针（ehtest2.C）可出 body 但条件三段 short-circuit 形距 ORIG
// `test;jmp` 更远、且需新增 TU static 符号（链接污染）+误启用发信行为风险。
// ⇒ 编译期不可构造，终判 N111（无入口死块，4.4 恒 DCE）；本函数保持「仅可达路径」
//   语义忠实实现，tail（0x0816158d）已逐条对齐（CUser.h m_rewarded bool 化代落后）。
// ============================================================================
// [b28d-solo-TaiwanInternalDispatcher] DIFF·N111 封顶（ORIG 0x08161384 无条件 jmp 后
// 0x5a..0x263 发邮件主体为无入口死块，4.4 恒 DCE；可达 tail 已逐条对齐，p167 3 MISSING 全在死块）
// [M2S6 2026-09-11 复核] ①ORIG 0x08161389 处 1 字节 0x00 使 objdump 顺序解码错位 ⇒ 报告
// orig_insns=166 含 13 条假指令（真实 158；被覆盖段实为 0x0816138a..a2 的
// lea/mov/call vector-pair-ctor + 两 movl）⇒ 工具口径 gap 恒含 +8 假差。
// ②恒 0 volatile bool 守卫实现 our=162 = 真实+4（movb 初始化 1 + movzbl/test/je 3）；
// 探针 /tmp/m2s6_probe3.o 三形实测：`if (A && 0)` 4.4 整体折叠连体删（f）、volatile 局部
// （f3，=现状 9 条）、`if (A && msg==0)` 复用已验非空事实（f，7 条）——均无 ORIG 的
// 「test; jmp + 保留体」单跳转形（4.1 -O0 发射不可达体、4.4 需 2 分支守卫）⇒ N111 维持。
// ③p167 余项全为 C1/C2·D1/D2 别名族（ORIG nm 同址：Inven_ItemC1≡C2@0x080cb854、
// vector<pair>D1≡D2@0x081349ea、pair C1≡C2@0x081627d4），非语义缺口。
// [N1F 2026-09-11 复核·工具 desync 定量] 直读 ELF 字节：0x08161384 = `e9 04 02 00 00`
//   （无条件 jmp → 0x0816158d，位移 0x204）；0x08161389 = 单字节 0x00，使 objdump 自该点
//   顺序解码错位 ⇒ 报告 orig_insns=166 中 13 条为假指令（`add %cl,0x489c445(%ebp)` /
//   `fadds (%ecx)` 等）。自 0x0816138a 对齐解码（objdump --start-address=0x0816138a）
//   得死块真实 138 条（lea -0x3c/vector-pair ctor/findString×2/9-loop/ReqDBSend… /
//   _Unwind_Resume），与我方死块同构（仅槽号差 2~4）。⇒ gap +3 全为 desync 假差，
//   4.4 无法构造「test; 无条件 jmp + 仍发射体」的单跳转形（既有探针 /tmp/m2s6_probe3.o
//   三形：`if (A && 0)` 连体删 / volatile 局部 / 复用已验事实），N111 维持。
int InterSelectMobileAuthReward::dispatch_sig(CUser* user, char* buf, int len)
{
    Msg* msg = reinterpret_cast<Msg*>(buf);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__,
                              0x161, "[Taiwan, MobileAuth] interMsg is null.");
        return 0x162;
    }
    // ORIG 0x08161384 = 无条件 jmp（源码条件第二项编译期恒假），其后 0x08161389..0x0816158c
    // 的发邮件主体是 ORIG **文本发射的不可达代码**（0x204 字节）。4.4 前端把 `&& 0` 整体
    // 折叠并 DCE 该块 ⇒ 用恒 0 的局部 volatile 守卫迫使后端照常发射（局部量不产生符号；
    // 守卫恒 0 ⇒ 可观察行为与 ORIG 一致：该块永不执行）。
    volatile bool dead = false;
    if ((msg->m_rewarded ^ 1) != 0)
    {
        if (dead)
        {
            std::vector<std::pair<int, int> > items;
            items.push_back(std::pair<int, int>(0x28983a, 1));
            char title[0x1d];
            char mail[0xff];
            strncpy(title, g_scriptStringManager_.findString(4, "tw_mobile_authentication_title_01", 0), 0x1d);
            strncpy(mail, g_scriptStringManager_.findString(4, "tw_mobile_authentication_mail_01", 0), 0xff);
            Inven_Item invenItems[10];
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, invenItems, 10);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                title, invenItems, (unsigned int)items.size(), 0,
                (unsigned int)user->getCurCharacNo(), mail, (int)strlen(mail), 0,
                user->GetServerGroup(), false);
            CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xa1);
            if (ev != 0 && ev->IsEventing(0))
            {
                static_cast<CEventCeraShopRewardPoint*>(ev)->SendReward(user);
            }
        }
    }
    user->m_rewarded = msg->m_rewarded;
    return 0;
}