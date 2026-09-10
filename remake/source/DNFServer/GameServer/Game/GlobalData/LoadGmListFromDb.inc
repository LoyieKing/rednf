bool CGM_Manager::LoadGmListFromDb()
{
    // [b23S3C] ORIG 0x08299530：ok 即返回变量（exec/get_int 逐次覆写），
    // mId/level/rows 函数级预清（P197③ 帧头 movl $0×N），writeGuard 保护填充。
    // ORIG 帧头 `movb $0x0,-0x11(%ebp)` ⇒ bool 预清两句式（P198①）。
    bool ok = false;
    int rows = 0;
    unsigned int mId = 0;
    int level = 0;
    m_db->set_query("seLect m_id, level from gm_manifest");
    ok = m_db->exec(true);
    if (!ok)
    {
        // [b28d-solo-GlobalData] P221 全表达式临时链：ORIG 三处 trace 各占
        // 独立槽（-0x44/-0x34/-0x24），具名对象在 4.4 下复用同槽致帧浅 0x30。
        cMyTrace(__PRETTY_FUNCTION__, 0x1579, 0)(
            "Failed to get GM list from DB.");
        return false;
    }
    writeGuard<rwLock> guard(&m_lock);
    rows = m_db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        m_db->fetch();
        ok = m_db->get_int(0, mId);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1588, 0)(
                "Failed to get GM's m_id from DB.");
            // [b28d-solo-GlobalData] ORIG 0x0829962x 该分支为 `mov $0x0,%ebx;
            // jmp <尾>`（直接物化返回寄存器）⇒ 原码是 `return false;` 而非
            // ok=false;break（后者多写 -0x11 槽且尾部经 movzbl 重载）。
            return false;
        }
        ok = m_db->get_int(1, level);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x158e, 0)(
                "Failed to get GM's lv from DB.");
            return false;
        }
        m_gmMap[mId].m_level = level;
    }
    return ok;
