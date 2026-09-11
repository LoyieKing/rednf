// ----------------------------------------------------------------------------
// 4. ORIG T 0x086689ee sz 0x32a。
// Quest 触发物品邮寄：m_mailItems(+0x114) 按 (itemNo,count) 成对遍历；
// itemNo==0 且 count>0 走金额路径（count 入第三参）；否则 find_item 经
// make_item 组道具（count 写 m_addInfo2，第三参置 0）。空列表时寄一封默认
// 道具邮件。标题/正文取 m_mailTitle(+0x10c)/m_mailContents(+0x110)。
void CUser::_postQuestTriggerItem(const Quest* quest)
{
    // b28d-solo-CUserDepsB: ①P47/P222 极性反转——ORIG 0x086689ee 先
    // `call empty; test; je <非空分支>`，空列表分支在前（原 `if(!empty)` 反向）；
    // ②P23/P44 实参内联右到左求值（ORIG edi=GetServerGroup、-0xb8=size、
    // esi=body.c_str、ebx=getCurCharacNo、title.c_str 即时），去 5 个具名槽；
    // ③去 items 引用局部（ORIG 每处重读 quest->m_mailItems）。
    // [S426 本轮翻正 ⇒ AE（203=203 逐条同构）] ④第二对元素读完后才 `++it`
    // （循环尾公共块，ORIG 0x08668cce）；⑤else 内层 `if (script != 0) { body }`
    // 改写 P223 负条件 goto 形：ORIG 0x08668bfd `cmpl $0,-0x1c(%ebp);
    // je <nop 岛 +0x2df>` + body 尾 0x08668ccb `jmp <join +0x2e0>` + nop 是
    // 4.1 的 if-join 岛，4.4 对正向 if 恒 jump-thread（少 jmp+nop）；改
    // `if (script == 0) goto post_quest_item_next; body; post_quest_item_next:;`
    // 后 4.4 精确产 `je <nop>; body; jmp <join>; nop; join:`（探针
    // /tmp/b28d_s426/probe_goto.cpp f1 复证）。旧「N112/N121 封顶」判词作废。
    // p167 CLEAR（30/30）。ORIG 0x086689ee。
    if (quest->m_mailItems.empty())
    {
        Inven_Item mailItem;                       // 空列表单封默认道具邮件
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            quest->m_mailTitle.c_str(), mailItem, 0, getCurCharacNo(),
            quest->m_mailContents.c_str(),
            static_cast<int>(quest->m_mailContents.size()), 0,
            GetServerGroup(), 0, 0);
    }
    else
    {
        // 迭代器成对读取（读 A→++→读 B→处理→++），与 ORIG 循环形态一致。
        for (std::vector<int>::const_iterator it = quest->m_mailItems.begin();
             it != quest->m_mailItems.end(); )
        {
            int itemNo = *it;
            ++it;
            int count = *it;
            Inven_Item mailItem;                   // local_aa 每对新建
            if (itemNo == 0 && count > 0)
            {
                mailItem.m_addInfo = itemNo;       // -0xa4
                mailItem.m_addInfo2 = count;       // -0x9f
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    quest->m_mailTitle.c_str(), mailItem, count,
                    getCurCharacNo(), quest->m_mailContents.c_str(),
                    static_cast<int>(quest->m_mailContents.size()), 0,
                    GetServerGroup(), 0, 0);
            }
            else
            {
                CItem* script = G_CDataManager()->find_item(itemNo);
                // P223（本轮翻正）：ORIG 0x08668bfd `cmpl $0,-0x1c(%ebp);
                // je <nop 岛 +0x2df>` + body 尾 0x08668ccb `jmp <join +0x2e0>`
                // + nop ⇒ 4.4 唯一复现形 = 负条件 goto 到 else 块尾空标签
                // （`if (script != 0) { body }` 被 jump-thread 成无 jmp/nop）。
                if (script == 0)
                    goto post_quest_item_next;
                script->make_item(mailItem);   // vtable+8
                mailItem.m_addInfo = itemNo;
                mailItem.m_addInfo2 = count;
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    quest->m_mailTitle.c_str(), mailItem, 0,
                    getCurCharacNo(), quest->m_mailContents.c_str(),
                    static_cast<int>(quest->m_mailContents.size()), 0,
                    GetServerGroup(), 0, 0);
            post_quest_item_next:;
            }
            // ORIG 0x08668cce 段：第二对元素处理完后才 `++it`（循环尾公共块），
            // 非紧跟 `int count = *it;`（P28 增量侧：位置决定 ppEv 落点）。
            ++it;
        }
    }
}