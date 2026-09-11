// ============================================================================
// InitMapEquipmentAniScript —— ORIG 0x089c0586（init.md:97 +0x0176，返回值
// 不检查继续）。new std::map<string,STEquipmentAniScript>(0x18) → 存入
// g_mapEquipmentAniScript(0x94f7bf8) → ImportEquipmentAniScript(map)。
// ============================================================================
void InitMapEquipmentAniScript(void)
{
    g_mapEquipmentAniScript = new std::map<std::string, STEquipmentAniScript>();
    ImportEquipmentAniScript(g_mapEquipmentAniScript);
}