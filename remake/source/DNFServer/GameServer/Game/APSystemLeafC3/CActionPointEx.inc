CActionPointEx::CActionPointEx()
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0889566e 构造序（基类 call → +0x24 →
    // +0x14 容器 → +0x34 → 尾清零）与 APSystemTypes.h 平铺布局妥协互斥
    // （P242 族）；语义等价。
    memset(&m_field00, 0, 0xb);   // 基类 _ActionPoint 子对象 [+0x00,+0x0b)
    *reinterpret_cast<unsigned short*>(&m_bytes02[0x0a]) = 0;   // +0x0c word
    m_bytes02[0x0c] = 0;          // +0x0e
    m_bytes02[0x0d] = 0;          // +0x0f
    m_bytes02[0x0e] = 0;          // +0x10
    m_weekday = 7;                // +0x20（weekday 缺省 7）
