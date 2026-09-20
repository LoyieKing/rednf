// ---- SIG_LOAD_ETC ctor（ORIG W 0x08491f86）----
// Mixed-region subobjects are constructed on named union fields (not this+imm).
SIG_LOAD_ETC::SIG_LOAD_ETC()
{
    new (m_mixedRegion) SIG_LOAD_SKILL();
    new (m_pvpMissionInfo) SIG_REQUEST_PVP_MISSION_LIST();
    new (m_questShopMain) SIG_QUEST_SHOP_DATA();
    new (m_padD6FC) SIG_ITEM_MAKING_SKILL();
    new (m_todUserState) SIG_REQUEST_TOD_USER_STATE();
    new (m_padD73C) SIG_ITEM_DICTIONARY_FLAG();
    new (m_characOptions) CharacterOption();
    new (m_padECC8) SIG_BOOSTER_GAGE_DATA();
    new (m_padECD0) SIG_BEST_CLEAR_TIME();
    new (m_comboSkill) SIG_COMBO_SKILL();
    new (m_seriaRoomAniDeco) seriaRoom_AniDeco();
}
