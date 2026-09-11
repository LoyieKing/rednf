// [b28d-solo-EquipmentParameterInfo] AE：单一 bool ok + 具名 f1/f2；P167 CLEAR。
int GetApplyStatusInRange(std::vector<float>* v)
{
    v->push_back(39.0f);
    std::string s;
    if (!ScanStr(&s)) return 0;
    float st = (float)(int)getChangeStatusType(s);
    if (!ScanStr(&s)) return 0;
    float pct = (s == "%") ? 1.0f : 0.0f;
    // ORIG 0x089912c8/0x089912e0：两次 ScanInt 共用同一 bool 槽（-0x19），
    // 各自紧跟 `fildl/fstps` 物化具名 float（-0x20/-0x24）；检查在两次之后
    // （0x089912f4 `movzbl; xor $1; test; je`）⇒ 负极性早退。
    bool ok = false;
    float f1 = (float)ScanInt(&ok);     // ORIG -0x2c 转换临时复用，无具名 int
    float f2 = (float)ScanInt(&ok);
    if (!ok) return 0;
    v->push_back(st);
    v->push_back(pct);
    v->push_back(f1);
    v->push_back(f2);
    return 1;
}