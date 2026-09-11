// getString(int) @ 0x8acf1f0：函数内静态 DEFAULT_STRING（guard 0x9501a00
// 惰性初始化，ORIG 经 _ZNSsC1EPKcRKSaIcE 自 "" 构造，对象 0x9501a08）。
// index<0 或 index>=条数 → &DEFAULT_STRING（8acf221），否则返回
// &strings_[index]（8acf226）。ORIG 返回非常量 string*（DWARF 原型），
// const 成员取成员地址为 const，此处按 const std::string* 返回。
// b28d-solo-ScriptStringManager 判词：**NEAR**（45 vs 45 条，p167 CLEAR）。
// 残差仅 .o 未解析重定位的 call/guard 标签显示差。关键：具名 `result` 单出口形
// 消掉 4.4 的双 epilogue（嵌套 if / 三目恒 53 条），与 ORIG 共享尾块同构。
const std::string* ScriptStringManager::getString(int index) const
{
    // [b28c-21] ORIG 8acf1f0：正极性嵌套 if（js 负索引 → 尾部 DEFAULT），
    // `||` 合并形 4.4 生 setne 物化 + 3 callee-saved（81 vs 45 insn）。
    static const std::string DEFAULT_STRING("");  // ORIG DEFAULT_STRING
    const std::string* result = &DEFAULT_STRING;
    if (index >= 0 && index < static_cast<int>(strings_.size()))
        result = &strings_[index];
    return result;
}