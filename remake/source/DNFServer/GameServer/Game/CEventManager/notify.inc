// b28d-solo-CEventManager: [N7 封顶] BOOST_FOREACH 复现 + 正向 `if (it != end)` 包裹
// （P220）；ORIG 0x08116682，144=144 助记符序列全等，唯一残差=间接 call 目标寄存器
// （ORIG %ecx vs 4.4.7 恒 %eax，P230 限定 4.4.7；4.4.6 后端可翻但禁改路由表）。
// 逐条归一化比对确认无其它差异；P282 复核 vtable 槽 +0x30=delivery 与头声明序一致。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
// checkAddress 校验 → map.find → 遍历该 type 的 list，对非空 CEventBase 经
// vtable+0x30（= delivery 虚槽 12）调用 delivery(msg)。
void CDelivery::notify(int type, DeliveryMsg& msg)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it != m_eventMap.end())
    {
        BOOST_FOREACH(CEventBase*& e, it->second)
        {
            if (e != 0)
                e->delivery(msg);
        }
    }
}