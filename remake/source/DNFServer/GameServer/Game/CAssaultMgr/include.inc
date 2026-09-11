// ORIG T _ZN7MapArea7includeEii 0x82f0298：矩形包含判定。
bool MapArea::include(int x, int y)
{
    // caller 0x82e5ed1 call 后 xor $0x1,%eax（技巧1 bool 专属形）⇒ bool 返回
    // ORIG 0x82f0298：四条件共用尾岛 `mov $0,%eax`（技巧24），true 路径
    // `mov $1; jmp 尾` 在岛前。4 个独立 goto 在 4.4 各产 nop 中继块 ⇒
    // 合并为单 || 短路链，跳边直指共享 false 块。
    if (m_x > x || m_x + m_w < x || m_y > y || m_y + m_h < y)
        goto RET0;
    return true;
RET0:
    return false;
}