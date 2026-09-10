int GetEquipmentTypeStringToEnum(std::string& type);

// ============================================================================
// 本 TU 内 ORIG helper（地址均在 EquipmentParameterInfo.cpp 域）：
//   getTargetTag(0x89a7413)/getStatusType(0x89a74a2)/getSign(0x89a752a) 为 static
//   （ORIG 小写 t）；GetStat/GetStatValue/GetStatByCondition/GetApplyStatusInRange/
//   GetSkillLevelUp/getBuffToEnumValue/getSkillDataExpansionToEnumValue/
//   getEquipmentTypeToEnumValue 为全局 T。scanScriptValue/importSkillDataUp 属
//   0x088bxxxx 域，但为 readAction 直接解析依赖，并入本 TU 统一提供。
// ============================================================================
