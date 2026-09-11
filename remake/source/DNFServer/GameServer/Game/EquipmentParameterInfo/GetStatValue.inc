// [b28d-solo-EquipmentParameterInfo] AE：具名 fval 先物化 + `if(!ok)` 负极性早退；P167 CLEAR。
int GetStatValue(std::vector<float>* v)
{
    std::string s;
    if (!ScanStr(&s)) return 0;
    float st = (float)(int)getChangeStatusType(s);
    if (!ScanStr(&s)) return 0;
    float pct = (s == "%") ? 1.0f : 0.0f;
    // [b28d-solo-EquipmentParameterInfo] P0：ORIG `mov %eax,-0x2c; fildl; fstps -0x1c`
    // 中 -0x2c 是 int→float 转换的匿名临时（非具名 int 局部）⇒ ScanInt 结果直接
    // 作为 (float) 操作数，fval 具名（0x0899154d..0x08991553）。
    bool ok = false;
    float fval = (float)ScanInt(&ok);
    // ORIG 0x0899155a 检查形 `movzbl ok; xor $1; test %al; je <push块>` ⇒
    // 负极性早退（失败块直落），push 成功后 return 1。
    if (!ok) return 0;
    v->push_back(st);
    v->push_back(pct);
    v->push_back(fval);
    return 1;
}