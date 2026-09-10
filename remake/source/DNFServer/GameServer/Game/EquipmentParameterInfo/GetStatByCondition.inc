int GetStatByCondition(std::vector<float>* v)
{
    v->push_back(38.0f);
    std::string s;
    if (!ScanStr(&s)) return 0;
    float st = (float)(int)getChangeStatusType(s);
    if (!ScanStr(&s)) return 0;
    float pct = (s == "%") ? 1.0f : 0.0f;
    bool ok = false;
    float fval = (float)ScanInt(&ok);   // ORIG -0x2c 为转换匿名临时，无具名 int
    if (!ok) return 0;
    v->push_back(st);
    v->push_back(pct);
    v->push_back(fval);
    return 1;
