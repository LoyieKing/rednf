bool CEventManager::TriggerEventEnd(int idx)
{
    // b28d-solo-CEventManager: [N7 封顶] ORIG 0x08115d60——同 TriggerEventStart 的
    // 正向包裹 `je <mov $0 尾>` 布局 + filterEvent bool 化后 43=43 全等；唯一残差=
    // 2 处虚调用目标寄存器（ORIG %edx vs 4.4.7 %eax，N7）。
    // b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event != 0)
    {
        event->deliveryEnd(m_delivery);
        BroadcastEventInfo();
        event->EndEvent();
        return 1;
    }
    return 0;
}