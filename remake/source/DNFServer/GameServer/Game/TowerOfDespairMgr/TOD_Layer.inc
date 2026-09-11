TOD_Layer::TOD_Layer(unsigned short layer)
{
    m_layer = layer;
    if (m_layer > 100)
    {
        // [b28c-35] ORIG 0x085fe7d8：layer 在 cMyTrace::C1 调用**前**读入 %ebx
        // （`movzwl -0x1c(%ebp),%ebx` 跨 ctor 保活，push %ebx; sub $0x34）；具名
        // 对象形 4.4 改在 ctor 后重读槽（无 push ebx，帧 0x38）。P227 全表达式
        // 临时链复现。
        cMyTrace(__PRETTY_FUNCTION__, 0x10, 5)(
            "@TOD: invalid layer setting : %d", (unsigned int)layer);
        m_layer = 1;
    }
}