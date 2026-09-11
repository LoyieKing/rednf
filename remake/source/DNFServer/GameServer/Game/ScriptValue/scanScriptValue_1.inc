// ORIG 0x088bd8e6：默认构造 ScriptValue → scanScriptValue → 成功则 pushScriptValue。
bool scanScriptValue(std::vector<float>& values)
{
    ScriptValue v;
    // ORIG 0x088bd902 `xor $0x1; test; je +0x2a`：negated 判定走成功路（pushScriptValue;
    // mov $1 直落），失败 `mov $0;jmp` 岛紧贴 test 之后。`if(!f(v))return false;`
    // 负向先返形态复现该块序（P191 早返块落点）。
    if (!scanScriptValue(v))
    {
        return false;
    }
    else
    {
        pushScriptValue(values, v);
    }
    return true;
}