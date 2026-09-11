// [b28d-18 N] ORIG 切片缺失（objdump 直转 082a73cb）；p167 CLEAR（9/9）；残差=表内容占位+codegen | ORIG 0x082a73cb
// [b28d-solo-GlobalData] DIFF 封顶 N：ORIG 人证字符串表（.data 0x936e2a0，64 项）内容缺失，
// 重建以全 0 占位必须保留 s==0||*s==0 检查（否则 std::string(0) 崩溃），ORIG 无检查 ⇒ +10 条；
// p167 CLEAR（表访问调用同形）。
void CHumanCertifier::init()
{
    // ORIG 0x082a73cb：遍历 0..0x63 号人证字符串表（.data 0x936e2a0，64 项
    // char*），以字符串为键向 static map<string, vector<int>>
    // humanCertifyTable_（0x93f5758）插入下标。
    // 表指针原始值为 0x0814af00+2i（指向 .text，运行时不可用），重建工程无法
    // 还原内容，故以空串占位：循环结构/符号保持真实，表内容缺失（推断）。
    // [G7 语义修正·ORIG 全函数直证 0x082a73df..0x082a747c] ORIG 循环体仅
    // SaI ctor → SsC1EPKcRKSaIcE → map::operator[] → push_back → SsD1 → SaI D1，
    // **无任何判空/空串检查**；旧码 `if (s == 0 || *s == 0) continue;` 系发明防御
    // （多发 12 条 + 一具名 s 槽）。按 ORIG 删判空、去具名局部（P0/P183 实参内联）。
    static const char* const emptyStr[64] = {0};
    for (int i = 0; i <= 0x63; ++i)
    {
        humanCertifyTable_[std::string(emptyStr[i])].push_back(i);
    }
}