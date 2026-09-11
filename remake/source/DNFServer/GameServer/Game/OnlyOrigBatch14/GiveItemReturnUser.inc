// ===== _Z18GiveItemReturnUserP5CUser —— ORIG 0x084bfd28 =====
// find_item(0x1f15) 失败直接返回；make_item 填局部 Inven_Item、set_add_info(1)。
// 按最后游玩 dungeon 是否存在分两路拼邮件：正文
// findString(4,"return_user_msg_1",0) + snprintf("%s", dungeonName) 或
// findString(4,"return_user_msg_2",0)；发件人 findString(4,"return_user_msg_title",0)；
// ReqDBSendNewSystemMail(title, item, 0, characNo, body, strlen(body), 7,
//                        serverGroup, false, false)。
void GiveItemReturnUser(CUser* user)
{
    // [b14 复核·AE 归零（145=145，gap 0）] 三项修复（旧判词 N82/N115 作废）：
    // ①P329/P0：两处 `findString(4,"return_user_msg_title",0)` 原写成具名
    //   `const char* title` 局部 ⇒ 求值被提前到 GetServerGroup 之前且多一槽；
    //   内联回 ReqDBSendNewSystemMail 首实参位后，4.4 按 cdecl 右→左求值复现
    //   ORIG 0x084bfe31 段顺序（GetServerGroup→strlen→getCurCharacNo→findString）。
    // ②删除 dead `char pad[8]`（title 内联后槽自然重排，item 落 -0x69 与 ORIG
    //   一致，且不再多发 `movb $0x0,-0x34(%ebp)`）；帧 0x19c / push edi 亦自动对齐。
    // ③P223 多岛形：两个 return 改「两条独立 goto + 尾标签串」
    //   （`if(def==0) goto ret1;` / `if(dungeon==0) goto ret2;` + 尾
    //   `ret1:; ret2:;`）⇒ 复现 ORIG 0x084bff7f 的 `nop;jmp` 与 0x084bff82 的
    //   `nop` 双岛（4.4 对裸 return 恒 jump-thread 掉第一个 jmp）。
    // 虚调用核对（P193/P282）：`def->make_item(item)` ⇒ `mov (%eax),%eax;
    // add $0x8,%eax; mov (%eax),%edx; call *%edx` = CItem vptr+8 = vtable 槽 @2；
    // ORIG `_ZTV5CItem`@0x08c96da0 word[4]=0x08724fe0（纯虚）与 CItem.h 声明序
    // （~CItem D1/D0 占 @0/@1，make_item @2）一致，且与 ORIG 逐条同偏移。
    int itemIdx;
    itemIdx = 0x1f15;
    int nAddInfo;
    nAddInfo = 1;
    Inven_Item item;
    CItem* def = G_CDataManager()->find_item(itemIdx);
    if (def == 0)
    {
        goto give_item_ret1;
    }
    item.m_addInfo = itemIdx;
    def->make_item(item);
    item.set_add_info(nAddInfo);

    if (user->getCurCharacLastPlayDungeonIndex() != 0)
    {
        CDungeon* dungeon =
            G_CDataManager()->find_dungeon(user->getCurCharacLastPlayDungeonIndex());
        if (dungeon == 0)
        {
            goto give_item_ret2;
        }
        char buf[0x100];
        OS_API::snprintf(
            buf, 0xff,
            g_scriptStringManager_.findString(4, "return_user_msg_1", 0),
            dungeon->GetDungeonName());
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0),
            item, 0, user->getCurCharacNo(), buf,
            static_cast<unsigned int>(strlen(buf)), 7,
            user->GetServerGroup(), false, false);
    }
    else
    {
        const char* body =
            g_scriptStringManager_.findString(4, "return_user_msg_2", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0),
            item, 0, user->getCurCharacNo(), body,
            static_cast<unsigned int>(strlen(body)), 7,
            user->GetServerGroup(), false, false);
    }
give_item_ret1:;
give_item_ret2:;
}

// ===== _Z38GetInstanceDeathTowerPlaydataStatisticv —— ORIG 0x0860f451 =====
// ORIG：mov $0x94733a8 直返（.bss 静态实例，无 guard）。重建以文件级
// static 对象承载同一地址语义。
CDeathTowerPlayDataStatistic* GetInstanceDeathTowerPlaydataStatistic()
{
    // ORIG 0x094733a8 = _ZZ38GetInstanceDeathTowerPlaydataStatisticvE8instance（b，1B）
    // ⇒ 函数内 static 真名为 `instance`（原 `s_instance` 为发明名，改回 ORIG 名）。
    static CDeathTowerPlayDataStatistic instance;
    return &instance;
}