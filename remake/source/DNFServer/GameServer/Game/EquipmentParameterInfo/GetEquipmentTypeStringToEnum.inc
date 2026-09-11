// ---- 由 CStackableItem.cpp 提供（ORIG 0x08a86d28，'[hat avatar]' 带括号变体） ----
int GetEquipmentTypeStringToEnum(std::string& type);

// ============================================================================
// 本 TU 内 ORIG helper（地址均在 EquipmentParameterInfo.cpp 域）：
//   getTargetTag(0x89a7413)/getStatusType(0x89a74a2)/getSign(0x89a752a) 为 static
//   （ORIG 小写 t）；GetStat/GetStatValue/GetStatByCondition/GetApplyStatusInRange/
//   GetSkillLevelUp/getBuffToEnumValue/getSkillDataExpansionToEnumValue/
//   getEquipmentTypeToEnumValue 为全局 T。scanScriptValue/importSkillDataUp 属
//   0x088bxxxx 域，但为 readAction 直接解析依赖，并入本 TU 统一提供。
// ============================================================================

static bool getTargetTag(int* out)   // [b28d-Main·P292] int→bool（ORIG 调用点 call;xor $0x1;test %al）
{
    // [b28c-lane24] ORIG 0x089a7413：多 return + 共享 ~s 尾块 ⇒ 4.4 用 %ebx 保存
    // 返回值（探针 /tmp/b28c_l24_probe2.cpp::pA 逐条同形）；正极性 ScanStr（无 xor）
    // + `if (v == 10) return 0;` 复现 jne 成功块 / je 失败岛布局。
    if (out == 0) return false;
    std::string s;
    if (ScanStr(&s))
    {
        int v = getTargetToEnumValue(s);
        if (v == 10) return false;
        *out = v;
        return true;
    }
    return false;
}