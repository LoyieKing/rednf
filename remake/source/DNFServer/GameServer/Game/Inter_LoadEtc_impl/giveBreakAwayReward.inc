void Inter_LoadEtc::giveBreakAwayReward(CUser* user)
{
    // [Lane17 I 判词] ORIG 0x084d8e68：判据⑪ 直方图**逐值全等**
    // {−0x1c…−0x182 共 11 值两侧一致} ⇒ 无槽/帧问题；归一化逐行实测**唯一残差**
    // = 实参中转寄存器角色 1 处：ORIG `mov -0x30(%ebp),%ebx; mov %ebx,0x4(%esp)`
    // vs 我方 `mov -0x30(%ebp),%edx; mov %edx,0x4(%esp)` ⇒ **N7 封顶**
    // （跨版本 callee-saved 选择，源形不可择）。旧判词：帧差 0x1ac/0x1b0（已随槽位归位消失）。
    // = N82附/P190 帧分配，语义等价。
    char title[21];
    char body[256];

    if (user->getUserBreakAwayRewardLuckPoint() > 0)
    {
    }
    else
        user->setBreakAwayRewardData(false);   // ORIG test;setg;test;jne

    // [round5-InterLoadEtc 实测回退] P231 register 形：实测 ORIG 0x084d90c4
    // `movl $0x0,-0x30(%ebp)` / 0x084d90e1 `mov %eax,-0x30(%ebp)` **确有 itemId 槽**
    // ⇒ register 提示会消槽（帧/指令数 251→250 劣化），已回退。残余实参装载序
    // （ORIG 先物化 %ebx 再 call receiver）判 N75/N7 跨版本不可翻。
    int itemId = 0;
    int itemCnt = 0;
    itemId = user->getBreakAwayRewardItemId();
    itemCnt = user->getBreakAwayRewardAItemCnt();
    if (itemId == 0 && itemCnt == 0)
        return;

    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_135", 0),
            0x14);

    // ORIG(4.1) 为 if-else 链且每判点重调 getter（无跳转表、无 order 局部槽）
    if (user->getBreakAwayRewardItemOrder() == 0)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_136", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 2)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_142", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 4)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_160", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 1)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_158", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 3)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_159", 0), 0xff);
    else
        return;

    Inven_Item mailItem;
    int coinCount = 0;
    if (itemId == 0)
    {
        coinCount = itemCnt;
        GetInstanceCirculationStatistic()->AddCirculationStatistic(
            CIRCULATION_STATISTIC_FIELD(0x10), user, (unsigned int)coinCount);
    }
    else
    {
        // [K2 2026-09-11] P52/P231 实参升格：ORIG 0x084d9109 `mov -0x30(%ebp),%ebx`
        // 在 receiver `call G_CDataManager` **之前**载入 callee-saved 并跨该 call 保活
        // （旧判 N7/N75 封顶系误判）。探针 /tmp/k2p/recv.cpp r1(裸形：call 后
        // `mov -0x30,%edx`) vs r2(`register` 复本：`mov -0x30(%ebp),%ebx` 前置) ⇒ 逐条命中。
        // 注意：register **不能**加在 itemId 本身（会消掉 ORIG 的 -0x30 槽，旧记录），
        // 只对该调用点的实参加复本；ebx 在本函数序言已 push，帧不变。
        register int itemKey = itemId;
        CItem* found = G_CDataManager()->find_item(itemKey);
        if (found == 0)
            return;
        // [SweepInterLoadEtcImpl] ORIG 0x084d912d `mov %eax,-0x80(%ebp)` 与
        // 0x084d9142 `mov %al,-0x81(%ebp)` 的槽址 = mailItem(-0x82) + 2 / +1
        // ⇒ mailItem.m_addInfo = itemId、mailItem.m_itemType = GetItemType()
        // （旧译文 `(void)found->GetItemType();` 丢两次字段写入 = 真语义 bug）。
        // P282：ORIG 0x84d9135 `add $0xc` ⇒ 虚槽 word[5] = GetItemType()。
        mailItem.m_addInfo = itemId;
        mailItem.m_itemType = (unsigned char)found->GetItemType();
        found->make_item(mailItem);                  // ORIG 虚槽 +0x8
        mailItem.set_add_info(itemCnt);
    }

    unsigned int accId = user->get_acc_id();
    // [SweepInterLoadEtcImpl] ORIG 0x084d9187 `movl $0x0,-0x20(%ebp)`：sent 预清
    // 在 accId 赋值之后、GetServerGroup 之前（P198① 两句式，合写会丢预清行）。
    // GetServerGroup 无栈槽（ORIG 0x084d9199 `mov %eax,%ebx` 直落 callee-saved，
    // P0/P183）⇒ 内联第 8 实参；右→左求值序使它在 strlen 之前，与 ORIG 一致。
    int sent = 0;
    sent = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, mailItem, coinCount, user->getCurCharacNo(), body, (int)strlen(body),
        7u, (ENUM_SERVER_GROUP)(int)user->GetServerGroup(), false, false);

    // [SweepInterLoadEtcImpl] ORIG 0x084d9207 `jne 084d9280`(nop 岛) 直落 epilogue：
    // 4.4 对 `if (sent==0){...}` 恒把该出口线程化到 add 指令（少 1 岛）⇒ P223 尾空标签形。
    if (sent != 0)
        goto give_break_away_reward_end;
    {
        // [SweepInterLoadEtcImpl] ORIG 0x084d920f 结果直落 %edi 跨后续 3 个 getter
        // 保活（无栈槽往返，P0/P183）⇒ 内联第 4 实参；右→左求值序 = ORIG 的
        // getBreakAwayRewards 先于 GetUID/get_acc_id/getCurCharacNo。
        // ORIG 0x84d921c/0x84d9229：失败分支重调 getCurCharacNo/get_acc_id。
        DB_LoadBreakAwayRewardData::makeRequest((int)user->GetUID(),
                                                user->get_acc_id(),
                                                user->getCurCharacNo(),
                                                *user->getBreakAwayRewards());
        user->setBreakAwayRewardItemId(0);
        user->setBreakAwayRewardAItemCnt(0);
    }
give_break_away_reward_end:;
}