// [b28d-solo-EquipmentParameterInfo] AE：补 ORIG 两个 dead local + P223 goto 尾标签。
bool scanThen(std::string line, EquipmentParameterInfo* info, int idx)
{
    // ORIG 0x89a123c：循环 ScanType(token,true)；"[/then]" 返回 true；
    // readAction 失败或 EOF 返回 false；values 非空时收集到局部
    // vector<vector<float>>（ORIG 局部，未外传）。
    std::vector<std::vector<float> > collected;
    // [b28d-solo-EquipmentParameterInfo] P171/P232 dead local 照抄：ORIG 0x089a124f
    // /0x089a125a 在函数头构造 -0x20（vector<vector<float>>）与 -0x24（string），
    // 全程只被析构（0x089a1398/0x089a1376），无读取 ⇒ 原码声明未使用的局部。
    std::vector<std::vector<float> > collectedUnused;
    std::string tokenUnused;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            return false;
        }
        if (token == "[/then]")
        {
            // [b28d-solo-EquipmentParameterInfo] ORIG 0x089a12a9 `jne 089a1348`
            // （nop + mov $1,%ebx 就地块）⇒ P223 尾标签法。
            goto L_scanThen_ret1;
        }
        std::vector<float> values;
        if (!readAction(line, info, token, values, idx))
        {
            return false;
        }
        if (!values.empty())
        {
            collected.push_back(values);
        }
    }
L_scanThen_ret1:
    return true;
}