bool DB_InsertCreatureItem::insertCreatureItem(MySQL* db,
                                               SIG_CREATURE_ITEM const* sig)
{
    if (sig->m_mapSize == 0)   // +0x68
    {
        db->set_query(
            "upDate inventory set creature_flag=%d where charac_no=%d",
            1, sig->m_characNo);
        if (db->exec(true) != 1)
        {
            // ORIG 0x0841fa7c：`mov 0xc(%ebp),%eax; mov 0x48(%eax),%ebx` 前置
            // 缓存 charac_no，跨 cMyTrace ctor 后 `mov %ebx,0x8(%esp)`（P87）。
            register int cn = sig->m_characNo;
            cMyTrace trace(__PRETTY_FUNCTION__, 0x55dd, 5);
            trace("DB_InsertCreatureItem::dispatch, exec() ERROR charac_no=%u",
                  cn);
            return 0;
        }
    }
    // ORIG：strncpy 0x16 + 两字节 0（24 字节 movl 序列）+ 40 字节 rep stos。
    // gcc4.4 O0 不内联 strncpy/memcpy，用数组初始化触发同形常量 store。
    char expireDate[0x40] = "'9999-12-31 23:59:59'";
    int tick = OS_API::GetDateTimeTick();   // -0x24
    int days = sig->m_days;                 // -0x20（+0x54）
    if (days != 0)
    {
        memset(expireDate, 0, 0x40);
        sprintf(expireDate, "from_unixtime('%d')", days * 0x15180 + tick);
    }
    char deleteDate[0x40] = "'9999-12-31 23:59:59'";
    int expireTick = sig->m_expireTick;   // -0x1c（+0x58）
    if (expireTick != 0)
    {
        memset(deleteDate, 0, 0x40);
        sprintf(deleteDate, "from_unixtime('%d')", expireTick);
    }
    if (days == 0 && expireTick != 0)
    {
        memset(expireDate, 0, 0x40);
        sprintf(expireDate, "from_unixtime('%d')", expireTick);
    }
    db->set_query(
        "inSert into creature_items(charac_no,slot,it_id,expire_date,delete_date,reg_date,name,stomach,exp,endurance,no_charge,stat,creature_type)values(%d,%d,%d,%s,%s,NOW(),'%s',%d,%d,%d,0,%d,%d)",
        sig->m_characNo, sig->m_slot,
        ((DB_CreatureItemInserView*)sig)->m_itId,
        expireDate, deleteDate,
        sig->m_name,
        sig->m_stomachValue,
        ((DB_CreatureItemInserView*)sig)->m_exp,
        (unsigned int)((DB_CreatureItemInserView*)sig)->m_endurance,
        (int)(signed char)sig->m_field53, (int)(signed char)sig->m_flag50);
    return db->exec(true);
