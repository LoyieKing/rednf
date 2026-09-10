void pushScriptValue(std::vector<float>& values, ScriptValue& v)
{
    values.push_back(static_cast<float>(v.type));
    for (int i = 0; i < 4; ++i)
        values.push_back(v.value[i]);
