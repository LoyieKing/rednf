// ORIG T 0x088bd984：vector 长度 <= idx+4 直接返回；否则
// out.type = (int)at(idx)；out.value[0..3] = at(idx+1+i)。
void getScriptValue(ScriptValue& out, std::vector<float>& vec, int idx)
{
    if (static_cast<int>(vec.size()) <= idx + 4)
        return;
    out.type = static_cast<int>(vec.at(idx));
    for (int i = 0; i <= 3; ++i)
        out.value[i] = vec.at(idx + 1 + i);
}