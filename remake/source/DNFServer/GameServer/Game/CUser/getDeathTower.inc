WongWork::CDeathTower* CUser::getDeathTower()
{
    // ORIG 0x086552a4：idx<0 => NULL；否则 G_CGameManager()->getDeathTower(idx)
    // ORIG 直读 m_deathTowerIndex（movzwl+test %ax+js 正路径顺序，无局部栈槽）。
    if ((short)(unsigned short)m_deathTowerIndex >= 0)
        return G_CGameManager()->getDeathTower(
            (int)(short)(unsigned short)m_deathTowerIndex);
    return 0;
}