// ORIG 符号: _ZN11CGM_Manager16LoadGmDataFromDbEj @ 0x08299740
// set_query("seLect level from gm_manifest where m_id = %s", NumberToString(accId,0))
// → exec(true) 失败：cMyTrace(__PRETTY_FUNCTION__, 0x15a2, 0) 记
// "Failed to get GM list from DB." 返 false；get_n_rows()==0 返 false；
// writeGuard(m_lock) 内 fetch() 一次，get_int(0, m_gmMap[accId].m_level)，
// 等级为 0：cMyTrace(__PRETTY_FUNCTION__, 0x15b4, 0) 记
// "GM: %s exists in GM manifest but doen't have proper level. check DB" 返 false；
// 否则返回 exec 结果。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 123 = our 123（compare_tu AE 口径）。旧
//   b28d-37「N104/N82 封顶」作废，复现三钥匙：①guard 用显式作用域块——块内
//   `return false;`（失败臂析构前 `mov $0,%ebx; mov $0,%esi`）、块外 `return ok;`
//   （成功路径析构后才 `movzbl -0xd,%ebx`），4.4 即发 ORIG 的 %esi 选择器形；
//   ②t2 vararg 用全表达式临时链 `cMyTrace(...)(fmt, NumberToString(...))`（P227：
//   实参在 ctor 前求值并跨 ctor 保活 %ebx；具名两语句形实参序反）；③两把
//   cMyTrace 槽 -0x30/-0x20 与 guard -0x34 随①②归位。注：game_func_report 在
//   .o 阶段仍报 DIFF（未解析重定位标签显示差），以 compare_tu_game_orig.py 为准。
bool CGM_Manager::LoadGmDataFromDb(memberIdentificationNumber_t accId)
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
}