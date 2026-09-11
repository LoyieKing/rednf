void CDeathTower::_destroy()
{
    setTowerState((ENUM_TOWER_STATE)0);
    G_CGameManager()->returnDeathTower(this);
}