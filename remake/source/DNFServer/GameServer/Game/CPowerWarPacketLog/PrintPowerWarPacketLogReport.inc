// ORIG PrintPowerWarPacketLogReport 0x08480ac0：
//   cMyTrace 三段头 → 收集 vector<STPacketLog*> → sort(Compare) →
//   逐条 cMyTrace 打印 → 尾分隔线。
void CPowerWarPacketLog::PrintPowerWarPacketLogReport()
{
    // b28d-60：P221 全表达式临时链——ORIG 槽深序 trace1 -0x7c → trace3 -0x5c
    //（先声明更深），具名声明在 4.4 恒反向（-0x2c → -0x4c）⇒ 改「构造即调用」。
    cMyTrace(__PRETTY_FUNCTION__, 0x3a1, 0)("-------------------------------------------------------------------------------------------------------------");
    cMyTrace(__PRETTY_FUNCTION__, 0x3a2, 0)("|                                   POWAR WAR PACKET RESULT REPORT                                          |");
    cMyTrace(__PRETTY_FUNCTION__, 0x3a3, 0)("-------------------------------------------------------------------------------------------------------------");

    std::vector<STPacketLog*> vec;
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        vec.push_back(it->second);
    }
    std::sort(vec.begin(), vec.end(), STPacketLog::Compare);

    // [N6S6ZeroB 2026-09-11] ORIG 0x08480ac0 槽序实证：收集循环迭代器在
    // -0x90（`lea -0x90(%ebp),%eax` @+0xbf，for-init 形态 ⇒ 4.4 延迟到最后分配，
    // 落更深槽），打印循环迭代器在 -0x8c（@+0x23d）。两循环原为互换（我们
    // -0x8c / -0x90）⇒ 打印循环改「普通块变量 + 空 init 的 for」，两槽归位，
    // ndiff 8→0，verdict NEAR→ae。
    std::vector<STPacketLog*>::iterator vit = vec.begin();
    for (; vit != vec.end(); ++vit)
    {
        STPacketLog* log = *vit;
        cMyTrace(__PRETTY_FUNCTION__, 0x3b1, 0)(
            "- PACKET CLASS:%d NO:%5d, COUNT:%8d, NAME:%s",
            log->m_field4, log->m_field8, log->m_count, log->m_name.c_str());
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x3b6, 0)("-------------------------------------------------------------------------------------------------------------");
}