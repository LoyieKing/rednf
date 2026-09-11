// b28d-solo-InitScriptProducers_1: AE（修：循环变量 size_type→int，复现 ORIG
// `cmp -0xc(%ebp),%eax; setg` 有符号边界；size_type 生 seta）。
// ORIG T 0x08917db3：剔除 '[' 与 ']'（ORIG 以 tag="[]" 逐字符比对）；仅当确有
// 剔除时回写（ORIG local_11 门禁）。
void deleteTagBracketString(std::string& text)
{
    std::string tag = "[]";
    std::string filtered;
    bool replaced = false;

    // ORIG 0x08917ecd：cmp -0xc(%ebp),%eax; setg ⇒ 有符号 int 边界比较（size_type
    // 无符号会产生 seta）。
    for (int i = 0; i < (int)text.size(); ++i)
    {
        // b28d-lane25：P167 差集实证 ORIG 无 char 缓存——text[i] 在条件两臂与
        // push_back 处各读一次（operator[] 共 3 次）。
        if (text[i] == tag[0] || text[i] == tag[1])
        {
            replaced = true;
        }
        else
        {
            filtered.push_back(text[i]);
        }
    }

    if (replaced)
        text = filtered;
}
