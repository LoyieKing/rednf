// ---------------------------------------------------------------------------
// getAttackConditionToEnumValue — ORIG @ 0x89bcb6d，4 个分支，默认 4。
// ---------------------------------------------------------------------------
int getAttackConditionToEnumValue(std::string& s)
{
    if (s == "critical")     return 0;
    if (s == "counter")      return 1;
    if (s == "back attack")  return 2;
    if (s == "stuck")        return 3;
    return 4;
}