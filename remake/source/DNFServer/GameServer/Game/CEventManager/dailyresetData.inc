// b28d-solo-CEventManager: [N7 封顶] ORIG 0x0811630e——虚调用目标 %ecx + 索引
// %edx/%eax vs 4.4.7 目标恒 %eax + 索引 %ecx/%edx（42=42 助记符序列全等，仅寄存器
// 角色轮转）；P282 复核 +0x34=IsEventing、+0x24=dailyresetData 槽与头声明序一致。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
void CEventManager::dailyresetData()
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
        {
            if (m_events[i]->IsEventing(0))
                m_events[i]->dailyresetData();
        }
    }
}