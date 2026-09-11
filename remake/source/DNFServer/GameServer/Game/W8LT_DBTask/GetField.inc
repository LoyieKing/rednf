// ============================================================================
// DB_UpdateCreatureItem::GetField —— ORIG 0x084200a8（报告 GetField.md 全文）
// ============================================================================
// 生成 creature_items 的 SET 子句：name='%s'（escape_string 经 25B 中转，
// 源 sig+0x34）、stomach(+0x44)/creature_type(+0x50 符号 char)/exp(+0x4c)/
// charac_no(+0x48)/slot(+0x2c)/it_id(+0x08)/no_charge(+0x52 符号 char)/
// stat(+0x53 符号 char) 逐项按 m_hdr 位掩码经 AddFieldString 过滤；
// expire_date 三态：m_days==0 且 m_expireTick==0 → '9999-12-31 23:59:59'；
// m_days==0 且 m_expireTick!=0 → from_unixtime(m_expireTick)；否则
// from_unixtime(GetDateTimeTick() + m_days*0x15180)。
void DB_UpdateCreatureItem::GetField(MySQL* db, char* out, SIG_CREATURE_ITEM* sig)
{
    bool isFirst = true;
    char nameBuf[25] = { '\0' };
    db->escape_string(nameBuf, sig->m_name);
    AddFieldString(isFirst, out, sig->m_hdr, 0x01, "name='%s'", nameBuf);
    AddFieldString(isFirst, out, sig->m_hdr, 0x02, "stomach=%d",
                   sig->m_stomachValue);
    AddFieldString(isFirst, out, sig->m_hdr, 0x04, "creature_type=%d",
                   (signed char)sig->m_flag50);
    AddFieldString(isFirst, out, sig->m_hdr, 0x08, "exp=%d",
                   sig->m_exp);
    AddFieldString(isFirst, out, sig->m_hdr, 0x10, "charac_no=%d",
                   sig->m_characNo);
    AddFieldString(isFirst, out, sig->m_hdr, 0x20, "slot=%d", sig->m_slot);
    AddFieldString(isFirst, out, sig->m_hdr, 0x40, "it_id=%d",
                   ((DB_CreatureItemInserView*)sig)->m_itId);
    AddFieldString(isFirst, out, sig->m_hdr, 0x80, "no_charge=%d",
                   (signed char)sig->m_keepRight);
    AddFieldString(isFirst, out, sig->m_hdr, 0x100, "stat=%d",
                   (signed char)sig->m_field53);
    if (sig->m_days == 0 && sig->m_expireTick == 0)
    {
        AddFieldString(isFirst, out, sig->m_hdr, 0x200,
                       "expire_date='9999-12-31 23:59:59'");
    }
    else if (sig->m_days == 0 && sig->m_expireTick != 0)
    {
        AddFieldString(isFirst, out, sig->m_hdr, 0x200,
                       "expire_date=from_unixtime('%d')", sig->m_expireTick);
    }
    else
    {
        // ORIG 0x08420…：`call; mov %eax,-0xc; …; imul; add %eax,-0xc` ——
        // 先赋槽再内存累加（+= RMW），非单表达式相加。
        int t = OS_API::GetDateTimeTick();
        t += sig->m_days * 0x15180;
        AddFieldString(isFirst, out, sig->m_hdr, 0x200,
                       "expire_date=from_unixtime('%d')", t);
    }
}