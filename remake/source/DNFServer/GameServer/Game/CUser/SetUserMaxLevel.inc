// ============================================================================
// ORIG T 0x0868fec8 _ZN5CUser14SetUserMaxLevelEi：写 m_field8ebd8（0 → 1）。
// ============================================================================
void CUser::SetUserMaxLevel(int level)
{
    // ORIG 0x0868fec8：>85 clamp 到 85；>0 取 level；否则 1。
    if (level > 85)
        m_field8ebd8 = 85;
    // ORIG 0x868fee4 `jg 868fef5(m=level)`：假路 fall=m=1 ⇒ 源码用反向 `<=0`（技巧 17，4.4 对 if(<=0){1}else{v} 产 jg 前进跳）
    else if (level <= 0)
        m_field8ebd8 = 1;
    else
        m_field8ebd8 = level;
}