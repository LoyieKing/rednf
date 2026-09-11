// ============================================================================
// _InsertPostal —— ORIG 0x0844372e：std::string 拼 "(...)" 20 列（循环 count
// 个附件，i!=0 时 gold 清 0；每项 getValues/GetUpgradeSeparate/blob_to_str 等）
// → set_query(postal insert ... values %s) → exec 失败 cMyTrace → 1。
// [b28d-34R 真 bug 修复] ORIG sprintf 变参 2/3 位序 = sig+0x122→0xc(%esp)、
//   sig+0x11e→0x10(%esp)（0x08443a32/0x08443a2e），即 postal.send_charac_no=
//   发件人 0x122、receive_charac_no=收件人 0x11e。旧视图字段名 send/recv 互换，
//   旧代码把 0x11e 填进 send 列、0x122 填进 receive 列（列值互换的真语义 bug）；
//   已按 0x11e=recv / 0x122=send 改名（三处消费点同步）⇒ 该装载段与 ORIG 逐条对齐。
//   另两处形态修复：`char buf[0x400]` 声明下移至 getValues 之后（ORIG rep stos
//   在 getValues 之后）、ampType/ampValue 改循环体内声明（4.4 槽 -0x31/-0x34 对齐）。
// [残余 N] fresh 复验 269/276（clean 口径）：元素域寻址 4.4 恒 base-first
//   （`mov 0x10(%ebp),%eax; … lea (%edx,%eax,1)`）vs ORIG index-first
//   （`mov -0x1c(%ebp),%eax; … add 0x10(%ebp),%eax`），6 处同形；已试 union→
//   纯数组视图、`(m_item+i)->` 指针算术两形均不可翻（后者反多 1 条 lea）
//   ⇒ P30/N102/N4 族封顶。调用点差集仅 P93 别名（string/cMyTrace C1↔C2、D1↔D2 同址）。
// [M5E round6 复核 · 269/276，定位到 11 处同根因（.h 轴）]
//  · 语义复验（逐条对 ORIG 实参槽）：sprintf 的 0x8/0xc/0x10/0x14(%esp) =
//    occTime(+0x3a1)/+0x122/+0x11e/item+0x126，12 个局部 -0x470…-0x43c 逐个同序；
//    尾部 `xor $1; test; je` + cMyTrace(…,5,0xa5b5) + `mov $0x0/%ebx` 与
//    `mov $0x1,%ebx` ⇒ 无字段错位、无极性错。
//  · 计数差 7 条 = 4 处「ORIG 4 条 `mov i,%eax; imul $0x3d,%eax,%eax;
//    add $0x130,%eax; add 0x10(%ebp),%eax` vs 我方 5 条 base-first」（+4）+
//    3 处我方多 1 条 `mov 0x10(%ebp),%edx`（+3，ORIG 直接用实参槽作 add 的内存
//    操作数）+ 4 处 1:1 换位（0）。同 _InsertLetter 同根因：ORIG 的形参类型自带
//    成员（基址=实参槽内存操作数），我方逐句 cast 使基址先进寄存器。
//  · 探针 /tmp/m5e_loop_probe.cpp：形 B（具名视图局部）与形 C（形参即视图）同为
//    index-first 装载序，但**同为 5 条**（具名局部不省条数，反而 +2 条存/载）
//    ⇒ 本 TU 内无法闭合；.h（SigTypes_D.h 具名成员）落地后本件并同 _InsertLetter
//    一起归零。P30/N102/N4 判词维持（新增 .h 轴归因）。
// ============================================================================
bool DB_MailBox_Req_System_Multi_Mail::_InsertPostal(
    MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sigRaw)
{
    std::string str;
    unsigned int i;

    str.clear();

    for (i = 0; i < sigRaw->m_count; ++i)
    {
        if (i != 0)
        {
            sigRaw->m_gold = 0;
        }

        unsigned char ampType = 0;
        unsigned short ampValue = 0;
        sigRaw->m_item[i].m_ag.m_amp.getValues(
            ampType, ampValue);

        char buf[0x400] = { 0 };

        sprintf(buf,
            "(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s',%d,%d)",
            sigRaw->m_occTime,
            sigRaw->m_sendCharacNo,
            sigRaw->m_recvCharacNo,
            sigRaw->m_item[i].m_sealFlag,
            sigRaw->m_item[i].m_itemId,
            sigRaw->m_item[i].m_addInfo,
            sigRaw->m_item[i].m_endurance,
            ((Inven_Item*)&sigRaw->m_item[i])->GetItemAttr(),
            ampType, ampValue,
            sigRaw->m_gold, (const char*)sigRaw,
            ((Inven_Item*)&sigRaw->m_item[i])->IsCreatureItemType(),
            ((Inven_Item*)&sigRaw->m_item[i])->isAvatarItemType(),
            sigRaw->m_letterId,
            sigRaw->m_item[i].m_extendInfo,
            db->blob_to_str(0,
                (void*)&sigRaw->m_item[i].m_ag.m_g.m_guidBlob[0], 10),
            db->blob_to_str(1,
                (void*)&sigRaw->m_item[i].m_randomBlob[0], 0xe),
            ((UpgradeSeparateInfo*)&sigRaw->m_item[i].m_seperateUpgrade)->GetUpgradeSeparate(),
            sigRaw->m_unlimitFlag);
        str.append(buf);

        if (i < sigRaw->m_count - 1)
        {
            str.append(",");
        }
    }

    db->set_query(
        "inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade ,amplify_option,amplify_value,gold,send_charac_name,creature_flag,avata_flag,letter_id,extend_info,item_guid,random_option,seperate_upgrade,unlimit_flag) values %s",
        str.c_str());

    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa5b5, 5)(
            "Insert Postal Fail - inQuery(%s)", str.c_str());
        return 0;
    }
    return 1;
}