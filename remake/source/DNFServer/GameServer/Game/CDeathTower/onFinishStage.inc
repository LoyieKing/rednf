void CDeathTower::CStage::onFinishStage()
{
    m_finishTick = GlobalData::s_systemTime_.getCurTickCount();
}