// ---------------------------------------------------------------------------
// 19. CUser::Update_ComboMission @0x0866280a
// ---------------------------------------------------------------------------
void CUser::Update_ComboMission(int comboIdx, short count)
{
    CMissionList_Charac* missions =
        reinterpret_cast<CMissionList_Charac*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
    missions->Update_ComboMission_Event(*this, comboIdx, count);
    send_MissionList();
}