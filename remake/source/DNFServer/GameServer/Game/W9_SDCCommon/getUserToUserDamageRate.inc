float SDCActiveStatusSyncCommon::getUserToUserDamageRate(bool deathTower)
{
    if (deathTower)
        return ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER;
    return ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE;
