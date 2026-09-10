void CParty::increase_ancient_dungeon_ticket()
{
    // [b28d-solo-CPartyDepsC2 判词] 封 N-lowering（ORIG 0x0859bc9a）：p167 19=19
    // CLEAR；槽位（-0x1c/-0x18/-0x14/-0x10/-0xc/-0x6e/-0x31/-0x16e）、find_item
    // ebx 装载时刻（`*&itemId`）、尾岛数（2 个）已逐条对齐；残差仅 3 条比较
    // lowering（探针 /tmp/b28d_c2_probe/p7.cpp 穷举）：
    //   ① m_89c（组合位 uchar，全仓 10+ 消费点用 !=0，不可 bool 化）`!=1`：
    //      ORIG `xor$1;test%al` vs 4.4 恒 `cmp$1,%al`（g0..g4 五形全 cmp）；
    //   ② slot<0：ORIG `mov;shr$0x1f;test%al;je` vs 4.4 恒 `cmpl$0;jns`
    //      （f0..f5 六形全 cmpl）。
    // 尾部 nop 对齐填充豁免（N112）。语义等价。
    if (m_battleField.m_pDungeon == 0)
        return;
    // movzbl 0x89c：要求副本 +0x89c 字节 == 1 才继续。ORIG 0x0859bcc6 为
    // `xor $0x1,%eax; test %al,%al; jne`（4.1 对 `x != 1` 的取反物化；
    // m_89c 为组合位 uchar 不可 bool 化）⇒ 显式 xor 形（语义等价）。
    if ((m_battleField.m_pDungeon->m_89c ^ 1) != 0)
        return;

    // dungeon+0x7f4 = RequiredItem.m_0（道具 idx）；+0x7f8 = RequiredItem.m_4
    const int itemId = m_battleField.m_pDungeon->m_entryRequiredItem.m_itemIdx;
    const int createParam = m_battleField.m_pDungeon->m_entryRequiredItem.m_itemCount;

    // find_item 结果存槽 local，比较用 cmpl $0（ORIG -0x14）；G_CDataManager()
    // 每次内联重取（P0：ORIG 无 dataMgr 局部，寄存器直传）。
    // ORIG 0x0859bcf5 `mov -0x1c(%ebp),%ebx` 先装载实参再 call G_CDataManager
    // ⇒ `*&` 形（P226 姊妹，探针 p8 p4 逐条全等）令 4.4 提前升格 callee-saved。
    CItem* found = G_CDataManager()->find_item(*&itemId);  // 0x0835fa32
    // ORIG find_item null 分支 `je 0x0859bfd8` 直落主出口（无 nop 中继岛）
    // ⇒ 正向包裹（P285）。
    if (found != 0)
    {
        Inven_Item item;                    // Inven_Item::Inven_Item W 0x080cb854
        item.m_addInfo = itemId;            // uStack_70/uStack_6e 两 short 写 = +0x02 int
        G_CDataManager()->m_itemList->create_item(  // CItemList::create_item T 0x08511fb8
            (ENUM_ITEM_CREATE_TYPE)0, item, createParam);

        // create 后有效性判定：m_addInfo(+0x02) 与 get_add_info()(+0x07) 任一非零
        // ORIG：bVar1 在寄存器物化（mov$1/jmp/mov$0 后 test%al），条件为
        // (m_addInfo != 0) || (get_add_info() != 0)
        if (item.m_addInfo != 0 || item.get_add_info() != 0)
        {
            for (int i = 0; i < 4; ++i)     // cmpl $3/setle：i<=3 共 4 槽
            {
                if (!checkValidUser(i))     // checkValidUser(int) W 0x08145868
                    continue;
                int slot = -1;              // movl $0xffffffff,-0xc
                // insertItemIntoInventory(Inven_Item, eItemAddReason, bool, bool)
                // 实参 (item, 0x38, 1, 1)；返回槽号符号扩展判负
                slot = m_member[i].m_pUser->getCurCharacInvenW()
                           ->insertItemIntoInventory(
                               item, (eItemAddReason)0x38, true, true);
                // ORIG 0x0859be5a `mov -0xc(%ebp),%eax; shr $0x1f,%eax; test
                // %al,%al; je 成功`（4.4 无此 lowering，探针 p7 六形全败）。
                if (((unsigned int)slot >> 31) != 0)
                {
                    // 入包失败 → 系统邮件补发门票后重置栈上物品
                    char title[0x15];       // local_35[21]
                    strncpy(title,
                            g_scriptStringManager_.findString(
                                4, "game_server_msg_53", 0),
                            0x14);
                    char body[0x100];       // local_172[256]
                    strncpy(body,
                            g_scriptStringManager_.findString(
                                4, "game_server_msg_154", 0),
                            0xff);
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        title, item, 0,
                        (unsigned int)m_member[i].m_pUser->getCurCharacNo(),
                        body, (int)strlen(body), 0,
                        m_member[i].m_pUser->GetServerGroup(), false, false);
                    item.reset();           // Inven_Item::reset W 0x080cb7d8
                }
                else
                {
                    m_member[i].m_pUser->SendUpdateItem((CUser::eSendTarget)1,
                                                        (ENUM_ITEMSPACE)0, slot);
                }
            }
        }
    }
