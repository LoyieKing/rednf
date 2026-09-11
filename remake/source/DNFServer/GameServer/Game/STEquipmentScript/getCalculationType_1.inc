// ---- ORIG T 0x088c227b：[calculation type]（'+'→0、'%'→1、其余→2） ----
int getCalculationType(const char* name)
{
    std::string tmp(name);
    if (tmp == "+")
        return 0;
    if (tmp == "%")
        return 1;
    return 2;
}