// ORIG 0x080f4ee2。按 m_materialKind 取失败概率写入 outProb：kind==1 → _getFailProb_Fortune，
// kind==2 → _getFailProb_NewNPC。kind==0 或其它不改 outProb。
void Event_Upgrade_Jar::GetProbability(int materialIdx, unsigned int& outProb) const
{
    // [b28d-solo-EventUpgradeJar] ORIG 0x080f4ee2：kind==0 的 je 落到 leave 前的
    // 空 then nop 岛（0x080f4f5e），两分支各自 `jmp leave`。4.4 的复现形为
    // 「负极性早退 goto + 尾标签」（P223 正例），外层 if(kind!=0) 包裹会被
    // jump-thread 掉 nop 岛（探针 /tmp/eu_probe1.cpp fB/fD 实证）。
    if (m_materialKind == 0)
        goto END;

    if (m_materialKind == 1)
    {
        outProb = _getFailProb_Fortune(materialIdx, (unsigned int)m_materialIdx);
        goto END;
    }
    if (m_materialKind == 2)
    {
        outProb = _getFailProb_NewNPC((unsigned int)m_materialIdx);
        goto END;
    }
END:;
}