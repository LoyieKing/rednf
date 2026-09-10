void ProcStage::sendMailRewardItem(CUser* user,
                                          const std::vector<_RewardItem>& items,
                                          const char* senderKey,
                                          const char* titleKey)
{
    if (user == 0 || senderKey == 0 || titleKey == 0)
    {
        return;
    }

    // b28d-solo-ProcStage：P0——ORIG 0x812f4c6/0x812f500 的 strncpy 第 2 参直接是
    //   findString 的 %eax（无 senderText/titleText 具名指针槽；旧形多 2 槽 ⇒
    //   全帧深 3）。循环计数器槽序：-0x14=count（在 itemBase 前声明、体首 ++）、
    //   -0x10=i（在 pairs 后声明、体尾 ++、条件 `cmpl $0x9` 用它）。
    // b28d-solo-ProcStage：P312——ORIG 数组槽基比 4.4 深 5 字节（sender -0x4b
    //   vs -0x46）⇒ 前置 5 字节死局部把数组组整体顶到 ORIG 同址。
    char framePad[1];
    (void)framePad;
    char sender[30];                      // ebp-0x4b 起 0x1e
    std::strncpy(sender, g_scriptStringManager_.findString(4, senderKey, 0),
                 sizeof(sender));
    char title[255];                      // ebp-0x15c 起 0xff
    std::strncpy(title, g_scriptStringManager_.findString(4, titleKey, 0),
                 sizeof(title));

    unsigned int count = 0;               // -0x14
    Inven_Item itemBase[10];              // ebp-0x3be，stride 0x3d 十次构造
    std::vector<std::pair<int, int>> pairs;
    int i = 0;                            // -0x10（上限 10）
    for (std::vector<_RewardItem>::const_iterator it = items.begin();
         it != items.end() && i < 10; it++, ++i)
    {
        ++count;
        // P178 嵌套实参形：ORIG 0x812f5a4 make_pair → pair<int,int> 转换 ctor → push_back。
        pairs.push_back(std::pair<int, int>(
            std::make_pair(it->m_value1, it->m_value2)));
    }

    // b28d-solo-ProcStage：P171——ORIG 0x812f631 `movl $0xf,-0xc(%ebp)` 死写入
    //   （调用点仍压立即数 0xf）⇒ 具名 int 局部照抄，同时把 count/i 槽顶深 4。
    int fieldType = 0xf;
    (void)fieldType;
    WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(pairs, itemBase, count);
    // b28d-solo-ProcStage：P0/P44——ORIG 0x812f65f/0x812f66f/0x812f684 把
    //   group/titleLen/characNo 留在 %ebx/%esi/%edx（无槽）⇒ 全部实参内联；
    //   求值序 = 右→左（group → titleLen → characNo），与 ORIG 装载序一致。
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
        sender, itemBase, count, 0,
        static_cast<unsigned int>(user->get_charac_no(-1)), title,
        static_cast<unsigned int>(std::strlen(title)), 0xf,
        user->GetServerGroup(), false);
