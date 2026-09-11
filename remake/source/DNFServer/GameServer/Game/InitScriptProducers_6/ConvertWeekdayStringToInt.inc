// ----------------------------------------------------------------------------
// ConvertWeekdayStringToInt —— ORIG T 0x088c284a。SUN..SAT→0..6、ALL→7；
// 未命中返回 false 且不触碰 out（ORIG 仅在命中路径写 (%edx)）。
// ----------------------------------------------------------------------------
// b28d-solo-InitScriptProducers_6: P63 共享尾（else-if 链 + 尾部统一 return true）。
// ORIG 用 `s.compare("X") == 0`（int compare → `test %eax,%eax; sete %al; test; je`），
// 非 operator==(bool)。ORIG 0x088c284a。
bool ConvertWeekdayStringToInt(std::string s, unsigned int& out)
{
    if (s.compare("SUN") == 0) out = 0;
    else if (s.compare("MON") == 0) out = 1;
    else if (s.compare("TUE") == 0) out = 2;
    else if (s.compare("WED") == 0) out = 3;
    else if (s.compare("THU") == 0) out = 4;
    else if (s.compare("FRI") == 0) out = 5;
    else if (s.compare("SAT") == 0) out = 6;
    else if (s.compare("ALL") == 0) out = 7;
    else return false;
    return true;
}