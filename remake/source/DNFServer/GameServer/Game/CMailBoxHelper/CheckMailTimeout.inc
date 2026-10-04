// [本波 2026-09-09 修] ①isUnlimit 全链 bool 化（P292：生产端 ORIG 0x08557590
// movzbl 直返 + 消费端 0x08552486 `xor $0x1; test %al; je`）⇒ 条件形态翻正、
// gap 1→0（189=189）；②kc 段删裸 register 改内联表达式（P248 第四条件「跨
// call 保活」不满足）⇒ 4.4 `mov -0x18,%edx; mov 0x80(%edx),%edx; add $1,%edx`
// vs ORIG `mov -0x18,%eax; mov 0x80(%eax),%eax; lea 0x1(%eax),%edx`（寄存器
// 角色 eax↔edx + lea↔add，N7/N18a 封顶）；③removed[21] 数组槽 + 迭代器槽序
// + 帧平移（N82/P112）。p167 CLEAR 26/26。
unsigned int CMailBoxHelper::CheckMailTimeout(CUser* user, PacketGuard* packet)
{
    // ORIG 0x08554e90
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return 0;
    unsigned int removed[21] = { 0 };
    long nowSec = GlobalData::s_systemTime_.getCurSec();

    MailMap::iterator it = mail_map(box).begin();
    MailMap::const_iterator e = mail_map(box).end();
    for (; it != e; )
    {
        CMailBox::CMail* mail = it->second;
        if (!mail->isUnlimit() && mail->IsTimeOver(nowSec))
        {
            packet->put_int((int)it->first);
            removed[removed[0] + 1] = it->first;
            mail_map(box).erase(it++);
            removed[0] = removed[0] + 1;
            if (mail->GetLetterId() != 0)
            {
                bool found = false;
                for (it = mail_map(box).begin(); it != mail_map(box).end(); ++it)
                    if (it->second->GetLetterId() == mail->GetLetterId())
                        found = true;
                if (!found)
                {
                    // P248 裸 register 第四条件不满足（无跨 call 保活）⇒ 删；
                    // P0：ORIG 0x08555098 值无槽往返（eax 直承）⇒ 内联表达式。
                    // [2026-09-09 Lane20] ORIG 0x08555098 形 `mov 0x80(%eax),%eax;
                    // lea 0x1(%eax),%edx; mov -0x18(%ebp),%eax;
                    // mov %edx,0x80(%eax)` 需**完整类型成员**访问（CMailBoxHelper
                    // 已是 CMailBox 的 friend）⇒ 用权威成员 m_loadedLetterCount
                    // （+0x80）；本地 CMailBoxView cast 形使 4.4 产 base-first +
                    // `add $1`（探针 /tmp/probe_cmh/p6.cpp q1 vs q3/q9）。
                    box->m_loadedLetterCount += 1;
                }
            }
            mail->ResetPackage();
            box->m_mailPool.Free(mail); // +0x10
        }
        else
        {
            ++it;
        }
    }
    ReqDBRemoveMail(user, removed, true);
    return removed[0];
}