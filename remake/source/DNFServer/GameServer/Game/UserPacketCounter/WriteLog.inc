// ORIG T 0x0856fff0：
//   CMyFileLog(m_logPath, 0x9d)(m_logPath, "user_packet_counter total_count(%d)", total)
//   遍历 map：idStr = NumberToString(key, 0)；
//     CMyFileLog(m_logPath, 0xa0)(m_logPath, "(%s,%12d)", idStr, value)
//   收尾：total=0、map.clear()。
// （ORIG 每条记录都新建一个 CMyFileLog，忠实保留。）
void CUserPacketCounter::WriteLog()
{
    // [A24TailBatch2] 判据③/P139：ORIG 0x08570010 `movl $&_ZZ...E19__PRETTY_FUNCTION__,0x4(%esp)`
    // ⇒ ctor 第一实参是 __PRETTY_FUNCTION__（局部静态符号），非 m_logPath。
    // P227 全表达式临时链：ORIG 0x0856fffc/0x08570005 在 ctor call 之前把
    // op() 实参求值到 callee-saved（total→%ebx、&m_logPath→%esi），4.4 具名对象形
    // 恒把实参压到 ctor 之后 ⇒ 用临时链复现（push ebx/esi + 帧 0x4c）。
    CMyFileLog(__PRETTY_FUNCTION__, 0x9d)(
        m_logPath, "user_packet_counter total_count(%d)", m_totalCount);

    for (std::map<unsigned int, int>::iterator it = m_counter.begin();
         it != m_counter.end(); ++it)
    {
        // [A24TailBatch2] P329/P52：ORIG 全部 no-slot（0x08570060 second→%esi、
        // 0x086070082 first→%eax→NumberToString→%ebx、idStr 留 %ebx 跨 ctor）⇒
        // 原码无 value/accId/idStr 具名局部（旧三槽使帧 0x4c→0x58 = 3×4B）。
        // 实参右→左求值：末实参 it->second 先算（→%esi），再 NumberToString(it->first,0)。
        CMyFileLog(__PRETTY_FUNCTION__, 0xa0)(
            m_logPath, "(%s,%12d)", NumberToString(it->first, 0), it->second);
    }

    m_totalCount = 0;
    m_counter.clear();
}