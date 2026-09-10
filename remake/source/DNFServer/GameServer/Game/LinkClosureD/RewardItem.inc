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
