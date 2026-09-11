// ---------------------------------------------------------------------------
// ORIG T 0x084bfb84 sz 0x1a4（报告缺失；objdump 0x084bfb84..0x084bfd28 逐指令
// 转写，下称证据 A）。语义：时间窗检查通过后按 itemIdx 生成物品附 add_info=1，
// RDAR 脚本文案作标题/正文经 ReqDBSendNewSystemMail 系统邮件投递。分支细节：
//   now = GlobalData::s_systemTime_.getCurSec()（证据 A 84bfb8f）
//   end > now → 返回；start3 != 0 且 start3 < now → 返回；begin > end → 返回
//   Inven_Item item;（默认 ctor 0x080cb854）
//   CItem* found = G_CDataManager()->find_item(itemIdx)；空 → 返回
//   itemIdx 存 item+0x02 槽（-0x64，find_item 前预存）；found->make_item(item)
//   （vptr+0x08 槽）→ item.set_add_info(1)
//   title = g_scriptStringManager_.findString(4, titleStr, 0)；strncpy(t,0x14)
//   msg   = g_scriptStringManager_.findString(4, msgStr, 0)；strncpy(m,0xff)
//   ReqDBSendNewSystemMail(title, item, 0, characNo, msg, strlen(msg),
//                          p8, group, false, false)
// 参数表（证据 A 槽序）：0x8 user / 0xc begin / 0x10 end / 0x14 start3 /
//   0x18 itemIdx / 0x1c titleStr / 0x20 msgStr / 0x24 p8(uint)。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] NEAR | 真语义 bug 修正：ORIG 0x084bfbfb
// `mov 0x18(%ebp),%eax; mov %eax,-0x64(%ebp)` = make_item 前写 item.m_addInfo
// （原译漏写）。另修：nAddInfo 具名 int（ORIG 0x084bfbc8）、body[0x100]（ORIG
// 栈距 0x100，Ghidra local_16a[256]）、P227 内联实参（group→%ebx/strlen→%esi/
// characNo→%eax 无槽）。帧 0x190 与全部具名槽对齐；残余 = title/item/body 槽
// 整体浅 1 字节（4.1 在 title 与 now 间留 1B 空洞，N95/P28 族）。
void RewardItem(CUser* user, long begin, long end, long start3,
                unsigned int itemIdx, const char* titleStr,
                const char* msgStr, unsigned int p8)
{
    long now = GlobalData::s_systemTime_.getCurSec();
    if (end > now)
        return;
    if (start3 != 0 && start3 < now)
        return;
    if (begin > end)
        return;

    // ORIG 0x084bfbc8 `movl $0x1,-0x10(%ebp)` 在 item ctor 之前 ⇒ 具名 int 局部
    int nAddInfo = 1;
    Inven_Item item;
    const CItem* found = G_CDataManager()->find_item((int)itemIdx);
    if (found == 0)
        return;

    // ORIG 0x084bfbfb `mov 0x18(%ebp),%eax; mov %eax,-0x64(%ebp)`：make_item 前
    // 把 itemIdx 写入 item+0x02（真语义缺口，原译漏写）。
    item.m_addInfo = (int)itemIdx;
    found->make_item(item);
    item.set_add_info(nAddInfo);

    // ORIG 深区顶部：title 起点 -0x29（= now -0x14 上方 0x15 字节，Ghidra
    // local_2d[21]）⇒ 数组声明 char[0x15]；strncpy 长度仍 0x14（0x084bfc4f）。
    char title[0x15];
    strncpy(title, g_scriptStringManager_.findString(4, titleStr, 0), 0x14);
    // ORIG 栈距 -0x166..-0x66 = 0x100 字节（Ghidra local_16a[256]），
    // strncpy 长度 0xff（0x084bfc89）。
    char body[0x100];
    strncpy(body, g_scriptStringManager_.findString(4, msgStr, 0), 0xff);

    // ORIG 0x084bfca3..cf4：group→%ebx、strlen→%esi、characNo→%eax 全程无槽
    //（P0）⇒ 三值均内联实参。
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, 0, (unsigned int)user->getCurCharacNo(), body,
        (int)strlen(body), p8, user->GetServerGroup(), false, false);
}