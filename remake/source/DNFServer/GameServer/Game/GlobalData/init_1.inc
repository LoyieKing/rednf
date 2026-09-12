// [b28d-18 N / b28d-solo-GlobalData 旧判词作废 · R19scanPH §24.1 ④ 修复] ORIG 切片缺失
// （objdump 直转 082a73cb）；p167 CLEAR（9/9）。旧重建以 `static const char* const
// emptyStr[64] = {0}` 占位 ⇒ 实测（R19scanPH）：0x8b318a0 处 64 项**全 NULL** 且循环
// i=0..0x63（100 次）索引它 —— 前 64 次 `std::string(NULL)`（libstdc++ __throw_logic_error
// ⇒ 进程终止）、后 37 次越界读 .rodata 垃圾指针；ORIG 无此对象（NEW-only、零写点）。
// 活表 = HUMAN_CERTIFY_EXAM_TABLE（本 TU 584 行定义；ORIG D 0x0936e2a0，100×4B，
// raw 逐项核与 ORIG 100/100 串一致）⇒ 恢复 ORIG 取址形 `mov 0x936e2a0(,%eax,4),%eax`。
void CHumanCertifier::init()
{
    // ORIG 0x082a73cb：遍历 0..0x63 号人证字符串表（.data 0x936e2a0，100 项
    // char*），以字符串为键向 static map<string, vector<int>>
    // humanCertifyTable_（0x93f5758）插入下标。
    // [G7 语义修正·ORIG 全函数直证 0x082a73df..0x082a747c] ORIG 循环体仅
    // SaI ctor → SsC1EPKcRKSaIcE → map::operator[] → push_back → SsD1 → SaI D1，
    // **无任何判空/空串检查**；旧码 `if (s == 0 || *s == 0) continue;` 系发明防御
    // （多发 12 条 + 一具名 s 槽）。按 ORIG 删判空、去具名局部（P0/P183 实参内联）。
    for (int i = 0; i <= 0x63; ++i)
    {
        humanCertifyTable_[std::string(reinterpret_cast<const char*>(HUMAN_CERTIFY_EXAM_TABLE[i]))].push_back(i);
    }
}