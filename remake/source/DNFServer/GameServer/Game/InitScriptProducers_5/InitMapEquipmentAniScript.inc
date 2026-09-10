void InitMapEquipmentAniScript(void)
{
    g_mapEquipmentAniScript = new std::map<std::string, STEquipmentAniScript>();
    ImportEquipmentAniScript(g_mapEquipmentAniScript);
