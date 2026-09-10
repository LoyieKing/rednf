void getScriptValue(ScriptValue& out, std::vector<float>& vec, int idx)
{
    if (static_cast<int>(vec.size()) <= idx + 4)
        return;
    out.type = static_cast<int>(vec.at(idx));
    for (int i = 0; i <= 3; ++i)
        out.value[i] = vec.at(idx + 1 + i);
