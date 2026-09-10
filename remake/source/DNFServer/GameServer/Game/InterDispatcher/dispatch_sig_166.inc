// P30/P248（ORIG 0x084d8??）：
//   ORIG 每处访问发 `mov <pPkt槽>,%ecx` + `add <pPkt>,%eax`（%eax 已是 i*0x14）、
//   `lea 0xe(%eax),%edx` / `add $0x10,%eax; mov 0xe(%eax),%ecx`
//   ⇒ 原码基址用**具名 pPkt**（非裸 data 形参），且条目 +0x10 处是子结构（字段 +0x0e 相对子结构）。
//   我方原写 `data + i * 0x14 + 0x1e`（扁平，`add $0xe; add data` 顺序相反，且多用形参而非 pPkt）。
//   RecWinView 定义不在本件写域，故本件自带同布局子结构视图。
struct Inter_BlackIPMonitorDeleteIPRecLeafView
{
    char m_pad0[0xe];              // 子结构 +0x00..+0x0d
    unsigned int m_val;            // 子结构 +0x0e == 条目 +0x1e
};

int Inter_BlackIPMonitorDeleteIP::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BlackIPMonitorDeleteIPPktView* pPkt = reinterpret_cast<Inter_BlackIPMonitorDeleteIPPktView*>(data);
    unsigned int i = 0;                                // ORIG 0x084d87f6 预清 -0xc + 循环前再清零（P191 双初始化）
    cMyTrace(__PRETTY_FUNCTION__, 0x3f6d, 0)("Delete IP Cnt : %d", pPkt->m_a);

    for (i = 0; i < pPkt->m_a; ++i)
    {
        // 逐处内联（P0：ORIG 除 pPkt 外无条目/取值具名槽往返）。
        if (GlobalData::g_blackIPMonitor->deleteIP(
                reinterpret_cast<char*>(pPkt) + i * 0x14 + 0xe,
                reinterpret_cast<Inter_BlackIPMonitorDeleteIPRecLeafView*>(
                    reinterpret_cast<char*>(pPkt) + i * 0x14 + 0x10)->m_val) != '\0')
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x3f76, 0)(
                "Delete IP: %s, Type: %d",
                reinterpret_cast<char*>(pPkt) + i * 0x14 + 0xe,
                reinterpret_cast<Inter_BlackIPMonitorDeleteIPRecLeafView*>(
                    reinterpret_cast<char*>(pPkt) + i * 0x14 + 0x10)->m_val);
        }
    }
    return 0;
