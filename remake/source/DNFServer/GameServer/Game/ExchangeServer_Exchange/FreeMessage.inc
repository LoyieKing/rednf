bool CExchangeServer::FreeMessage(Stream* stream, int flag)
{
    // [b28-L11 判 N·N93] 助记符序列 88=88 全等，残差 8 = 三 switch 臂 cMyTrace
    // 对象槽深方向跨版本相反（4.1 先声明→-0x3c 深，4.4→-0x1c 浅）。块文本序
    // 即 ORIG 汇编布局序（case1,0,2），交换块序可翻槽号但毁块布局（两约束
    // 死锁）；cMyTrace 带参 ctor 不可 P212 头部声明化。N93 主案在册勿再试。
    if (stream == 0)
    {
        return false;                                 // ORIG: mov $0x0,%eax 后走公共尾
    }

    const int len = stream->length();
    switch (stream->m_poolType)
    {
    case 1:                                           // SmallStreamPool
        if (len > 0xfff)
        {
            // [b28d-solo-Exchange 判 N·N93] P221 临时链实测翻槽方向成功
            // （case1 -0x1c→-0x3c 与 ORIG 同）但副作用：4.4 把 len 升格 %ebx
            // 跨 ctor 保活（push %ebx; sub $0x54；ORIG 无 push、ctor 后重读
            // -0xc），88→91 insn 反劣 ⇒ 维持具名形；残差 4 条 lea 槽号
            // （三臂 cMyTrace 槽深方向跨版本相反，N93 主案）。
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "FreeMessage(Stream*, int)", 0x3b5, 0x5)
                ("Small Stream Size over code(%d), size(%u)", flag, len);
        }
        GlobalData::s_small_stream_pool->Free(stream);
        break;
    case 0:                                           // StreamPool
        if (len > 0x3fff)
        {
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "FreeMessage(Stream*, int)", 0x3c0, 0x5)
                ("Default Stream Size over code(%d), size(%u)", flag, len);
        }
        GlobalData::s_stream_pool->Free(stream);
        break;
    case 2:                                           // BigStreamPool
        if (len > 0x3ffff)
        {
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "FreeMessage(Stream*, int)", 0x3cb, 0x5)
                ("Big Stream Size over code(%d), size(%u)", flag, len);
        }
        GlobalData::s_big_stream_pool->Free(stream);
        break;
    default:
        break;                                        // ORIG: 其它类型直接返回
    }
    return true;
