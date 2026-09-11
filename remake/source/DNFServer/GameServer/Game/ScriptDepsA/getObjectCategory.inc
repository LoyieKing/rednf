// ORIG getObjectCategory T 0x08a3ca1b（报告全文转写）：按值收参；查不到 0x2c。
// b28c-l23 根因（P96/P50 + P220 极性 + P132 operator*）：ORIG 只发射 `map::find` /
// `map::end` / `_Rb_tree_iterator...neEERKS4_` / `...deEv`（**operator\*，非 operator->**）
// 各一次（30 条），find 结果直接构造进 -0x10（无拷贝 ctor）⇒ ①原码用**非 const
// iterator**；②判据是正极 `if (it != end()) return (*it).second;` 让查表走
// fall-through（负极 `==` 形 4.4 恒产 eq + 块序反）；③取值写 `(*it).second`
// 而非 `it->second`（后者发 ptEv）。
int getObjectCategory(std::string name)
{
    std::map<std::string, ENUM_CATEGORY, IgnoreCaseCompare>::iterator it =
        s_objCategoryString.find(name);
    if (it != s_objCategoryString.end())
        return (*it).second;
    return 0x2c;
}