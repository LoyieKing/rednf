// b28d-solo-CHackAnalyzer: N 封顶（P75 跨版本装载序，本轮七形复测）——ORIG 0x08278b48。
//   唯一残差 = 循环体前两条 load：ORIG 0x08278c69 `mov -0xc(%ebp),%edx; mov 0x10(%ebp),%ecx`
//   （i 先、msg 形参后）；我方 f1 恒 msg 先（`mov 0x10(%ebp),%ecx; mov -0xc(%ebp),%edx`），
//   其余 8 条逐条全等（含 `lea (%ecx,%eax,1); add $0x30; mov 0xb(%eax)` 常量拆分）。
//   本轮实测七形（探针 /tmp/hc_probe2.cpp）：f1 现状/f2 (unsigned)i/f3 解引用/f6 i*1/f11 双 cast
//   恒 msg 先；f4 具名视图指针、f5 具名元素指针可得 i 先但多发函数头 store（ORIG 无）；
//   f8/f14 裸指针加法得 i 先但改用 add-mem 且少 lea/或改常量拆法 ⇒ 形不符。
//   ⇒ 4.4 对「形参基址 + 下标」恒 base-first 调度，属跨版本装载序，N 封顶（指令数全等）。
//   [round5 复核] fresh 108/108（compare_tu NEAR）；唯一残差仍为该两条 load 序
//   （ORIG 0x08278c69 i 先 / 我方 msg 先），八形穷举后维持 N75/N18a 封顶。
void Secu_AccountHacking::sendMail(char const* name, MSG_CHECK_MAIL_RECEIVER* msg)
{
    // §4.3 裸偏移清零 —— 载荷具名视图（本 TU 私有，不动冻结头）：
    // MSG_CHECK_MAIL_RECEIVER（CGameManager.h:103）的 m_pad 是 set<TYPE>() 于
    // +4 memcpy 写入的字节数据区，对应消息体（MSG_MAILBOX_SEND 等）在本仓仅有
    // 前置声明、权威定义属冻结 PacketTypes.h ⇒ 按 ORIG T 0x08278b48
    // （docs/class_func_reports/Secu_AccountHacking/sendMail.md 汇编段）实证
    // 偏移建 pack(1) 视图：
    //   set<0> 分支：@+0x049 `mov 0x32(%eax),%eax`（金额）/
    //                @+0x081 `mov 0x3d(%eax),%eax`（avatar 物品号）
    //   set<2> 分支：@+0x0e3 `mov 0x33(%eax),%eax`（金额）/
    //                @+0x16f `movzbl 0x37(%eax),%eax`（条目数）/
    //                条目物品号 = base + i*0xb + 0x3b（ Ghidra C 段同形）
#pragma pack(push, 1)
    struct CheckMailReceiverSendView
    {
        int m_checkType;              // +0x00（set<> 写入的 TYPE）
        char m_msgHead[0x32 - 4];     // +0x04..+0x31（本函数不读的消息前缀）
        unsigned int m_money;         // +0x32
        char m_pad36[0x3d - 0x36];    // +0x36..+0x3c
        unsigned int m_avatarItemIdx; // +0x3d..+0x40
    };
    struct CheckMailReceiverListEntry
    {
        unsigned int m_itemIdx;       // 条目 +0x00（ORIG 步长 0xb）
        char m_rest[0xb - 4];         // +0x04..+0x0a（本函数不读）
    };
    struct CheckMailReceiverListView
    {
        int m_checkType;              // +0x00
        char m_msgHead[0x33 - 4];     // +0x04..+0x32（本函数不读的消息前缀）
        unsigned int m_money;         // +0x33
        unsigned char m_entryCount;   // +0x37
        char m_pad38[0x3b - 0x38];    // +0x38..+0x3a
        CheckMailReceiverListEntry m_entries[1];   // +0x3b
    };
#pragma pack(pop)
    static_assert(sizeof(CheckMailReceiverSendView) == 0x41,
                  "CheckMailReceiverSendView ABI drift");
    static_assert(offsetof(CheckMailReceiverListView, m_money) == 0x33 &&
                  offsetof(CheckMailReceiverListView, m_entryCount) == 0x37 &&
                  offsetof(CheckMailReceiverListView, m_entries) == 0x3b,
                  "CheckMailReceiverListView ABI drift");
    static_assert(sizeof(CheckMailReceiverListEntry) == 0xb,
                  "CheckMailReceiverListEntry stride drift");

    // ORIG T 0x08278b48（sendMail.md）：检查类型 0（金币+单 avatar）/2
    // （金币+avatar 列表）时取出金额与 avatar 物品号逐一 sendGold/sendAvatar。
    // MSG_CHECK_MAIL_RECEIVER 各字段偏移（0x33/0x37/0x3b/0x3d/0x32）来自报告
    // 汇编；该类型为 set<T> 写入的字节数据区，具名化见上方视图。
    if (((CUser*)m_userP)->getCurCharacInvenR() == 0)
        return;

    if (*(int*)msg == 0)
    {
        // P0：ORIG getCurCharacInvenR 结果不经栈槽往返（`call; mov %eax,(%esp);
        // call get_money`）⇒ 内联实参，具名指针局部多 2 条 spill。
        sendGold(name, 0, ((CheckMailReceiverSendView*)msg)->m_money,
                 ((CUser*)m_userP)->getCurCharacInvenR()->get_money(),
                 2, (char*)0);
        if (isAvatar(((CheckMailReceiverSendView*)msg)->m_avatarItemIdx))
            sendAvatar(name, 0);
    }
    else if (*(int*)msg == 2)
    {
        sendGold(name, 0, ((CheckMailReceiverListView*)msg)->m_money,
                 ((CUser*)m_userP)->getCurCharacInvenR()->get_money(),
                 2, (char*)0);
        for (int i = 0;
             i < (int)((CheckMailReceiverListView*)msg)->m_entryCount; ++i)
        {
            // b28d-solo-CHackAnalyzer: NEAR 封顶（P75/N76 跨版本装载序）。ORIG
            // 0x08278c69 循环体 `mov -0xc(%ebp),%edx; mov 0x10(%ebp),%ecx`（索引先、
            // msg 后）；本工具链对 m_entries[i] 恒 base-first。本轮实测新增两形
            // （交换下标 i[arr]、索引 (unsigned int)i 中转）仍 base-first，六形
            // 均不可翻；两侧指令数/助记符序列全等，仅两 load 序差，语义等价。
            // [L6S4 实测负结论] 新试形：实参位 `(unsigned int)` 冗余 cast（P267 转换
            // 节点改装载序）——实测仍 `mov 0x10(%ebp),%ecx` 先于 `mov -0xc(%ebp),%edx`
            // （base-first 未翻），回滚。累计 9 形均不可翻 ⇒ N75/N76 封顶。
            if (isAvatar(((CheckMailReceiverListView*)msg)->m_entries[i].m_itemIdx))
                sendAvatar(name, 0);
        }
    }
}