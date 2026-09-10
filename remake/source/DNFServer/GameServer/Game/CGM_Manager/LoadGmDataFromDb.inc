bool CGM_Manager::LoadGmDataFromDb(unsigned int accId)
{
    bool ok = false;                       // ORIG 槽 -0xd
    int rows = 0;                          // ORIG 槽 -0xc
    m_db->set_query("seLect level from gm_manifest where m_id = %s",
                    NumberToString(accId, 0));
    ok = m_db->exec(true);
    if (!ok)
    {
        cMyTrace t1(__PRETTY_FUNCTION__, 0x15a2, 0);   // ORIG 槽 -0x30（块内局部）
        t1("Failed to get GM list from DB.");
        return false;
    }
    rows = m_db->get_n_rows();
    if (rows == 0)
    {
        return false;
    }
    // b28d-solo-CGM_Manager [P227+P59]：ORIG 0x08299815 guard 槽 -0x34（最深），失败臂
    // 在析构**前**装 `mov $0,%ebx`、成功路径在析构**后**才 `movzbl -0xd,%ebx` ⇒ 源码
    // 是「块内 guard + 块内 return false + 块外 return ok」。t2 的 vararg 在 ctor 前
    // 求值并跨 ctor 保活 %ebx（P227 全表达式临时链；具名两语句形为反序）。
    {
        writeGuard<rwLock> guard(&m_lock);     // ORIG 槽 -0x34（函数级）
        m_db->fetch();
        ok = m_db->get_int(0, m_gmMap[accId].m_level);
        if (m_gmMap[accId].m_level == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x15b4, 0)(
                "GM: %s exists in GM manifest but doen't have proper level. check DB",
                NumberToString(accId, 0));
            return false;
        }
    }
    return ok;
