bool CEventManager::TriggerEventStart(int idx, Word_Param param)
{
    // b28d-solo-CEventManager: [N7 封顶] ORIG 0x08115cc6——`je <mov $0 尾>` 布局，
    // 正向包裹 `if (event != 0) { body; return 1; } return 0;`（P222/P194）+
    // filterEvent bool 化后 52=52 助记符序列全等；唯一残差=3 处虚调用目标寄存器
    // （ORIG %edx vs 4.4.7 %eax，N7）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event != 0)
    {
        event->StartEvent(param);
        BroadcastEventInfo();
        event->deliveryStart(m_delivery);
        event->StartAction();
        return 1;
    }
    return 0;
}