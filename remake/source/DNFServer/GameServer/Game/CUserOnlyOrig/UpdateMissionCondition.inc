// ---------------------------------------------------------------------------
// 33. CUser::UpdateMissionCondition @0x086625c8
// ---------------------------------------------------------------------------
void CUser::UpdateMissionCondition(std::vector<short>& kinds, std::vector<int>& values)
{
    CMissionList_Charac* missions =
        reinterpret_cast<CMissionList_Charac*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
    missions->Update_RecvPacket_event(*this, kinds, values);
}