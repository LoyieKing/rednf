CEventManager::~CEventManager()
{
    // b28d-solo-CEventManager: [N7 封顶] P183/P0 修正——ORIG 0x08114d4e 无 event/
    // delivery 具名局部（每次重读 m_events[i]；delete 自带判空），缓存局部使槽
    // -0xc→-0x10 平移（已删）。44=44 助记符序列全等，唯一残差=虚析构调用目标寄存器
    // （ORIG add $0x4 槽 %ecx vs 4.4.7 %eax，P230/N7）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
            delete m_events[i];
    }
    delete m_delivery;
}