// [本波 2026-09-09 修，DIFF→NEAR] ①isUnlimit 全链 bool 化（P292）后
// `if (f()) continue;` 正极形与 ORIG 0x08552486 `call; test %al,%al; jne
// <循环增量>` 全等（旧 uchar 时代 `if(!f())`/continue 两形均不可复现）；
// ②本件 106=106（raw 108=108），残余仅深区槽整体 -4 平移（ORIG it/-0x38、
// minPeriod/-0x34、endIt/-0x30、period/-0x2c；我方浅 4），N82/P112 封顶。
// [2026-09-11 本轮复核修正] 上述旧槽记录有误：ORIG 实测 it@-0x38、endIt@-0x3c、
// minPeriod@-0x34、end() 临时@-0x30、period@-0x2c。本轮按 P328⑤ 把 `long period`
// 改函数头裸声明（占槽不占指令）后 it/endIt/minPeriod 三槽与 ORIG 完全同址，
// 残余仅 period(-0x30) 与 end() 临时(-0x2c) **两槽互换**：4.4 恒把匿名 sret 临时
// 放最浅空闲槽 -0x2c，ORIG 4.1 把块内具名 period 放该处。已试 5 形（period 三种
// 声明位、endIt 显式转换/直构/具名 mailEnd）均不翻 ⇒ N82/P112（保留 3/5 槽对齐形）。
// ③InsertTimer 段 ORIG 用 %edi/%esi/%ebx 三 callee-saved 装载
// （GetLoginTick/GetUID/GetCurCharacNo），4.4 角色互换（N7 族，本 TU 路由
// 4.4.6 仍不翻）。
void CMailBoxHelper::SetPeriodCheckTimer(CUser* user)
{
    // ORIG 0x085523e2
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long pad24 = 0x151800; // P31：ORIG 首槽死写（movl $0x151800,-0x24）
    long deadline = 0x151800;
    long nowSec = GlobalData::s_systemTime_.getCurSec();
    deadline = deadline + nowSec;
    // [2026-09-11 本轮] period 必须函数头裸声明（P328⑤ 占槽不占指令），
    // 否则 it/endIt 整体浅 4（-0x34/-0x38 vs ORIG -0x38/-0x3c）。见函数尾判词。
    long period;
    long minPeriod = deadline;
    MailMap::iterator it = mail_map(box).begin();
    MailMap::const_iterator endIt = MailMap::const_iterator(mail_map(box).end());
    for (; it != endIt; ++it)
    {
        // [本波 2026-09-09，isUnlimit 全链 bool 化后翻正] ORIG 0x08552486 条件为
        // `call; test %al,%al; jne <循环增量>`（2 条）= 正极 `if (f()) continue;`
        // （bool callee；旧 uchar 时代 continue 形曾 +2 条，bool 化后与 ORIG 同形）。
        if (it->second->isUnlimit())
            continue;
        period = it->second->GetPeriod();
        minPeriod = std::min(minPeriod, period);
    }
    if (minPeriod != deadline)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)8, (int)user->GetLoginTick(),
                                    (int)user->getCurCharacNo(), 0);
    }
}