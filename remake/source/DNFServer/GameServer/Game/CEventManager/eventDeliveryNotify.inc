void CEventManager::eventDeliveryNotify(int type, DeliveryMsg& msg)
{
    // ORIG 0x08116774：无局部槽，条件与调用各重读 m_delivery（+0x298）
    // 两次（P0 表达式直读）；body 尾 `jmp +1; nop` 中继需 P223 goto 尾空标签形。
    if (m_delivery == 0)
        goto done;
    m_delivery->notify(type, msg);
done:;
