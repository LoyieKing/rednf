int CParty::_Send_TOD_APCInfo(int todLayerIdx)
{
    if (m_battleField.m_pDungeon->isTowerOfDespairDungeon())           // +0xcac（ORIG 直取）
    {
        TowerOfDespairMgr* todMgr =
            static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);

        TOD_Layer layerZero(0);                         // -0xe
        todMgr->SendAPCInfo(layerZero, getManager());
        if (todLayerIdx != -1)
        {
            TOD_Layer layerParam((unsigned short)todLayerIdx);  // -0x10
            todMgr->SendAPCInfo(layerParam, getManager());
        }
    }
    return 1;
