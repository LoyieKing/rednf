// [b28d-solo-CSyncSlangFilter] N：P75/N-G —— 同 AddSlang（snprintf 首参 buf 装载时刻跨版本差）。
// 指令形直证：ORIG 0x0862e2e8 `lea -0x125(%ebp),%edx` 紧随 0x0862e2e3 的 c_str call、先于
// 0x0862e2f2 `movl $"%s",0x8(%esp)`，保活到 0x0862e302 `mov %edx,(%esp)`；4.4 恒末位 lea→%eax。
// 本波复核（我方 .o 与 ORIG 同口径逐条计数）两方均 102、仅此 2 行错位（N-G 六形穷举不可翻，勿再赌）。ORIG 0x0862e276。
void CSyncSlangFilter::AddSlangName(const std::string& str)
{
    char buf[0x101] = {0};
    OS_API::snprintf(buf, 0x100, "%s", str.c_str());
    buf[0x100] = 0;
    SlangNode* node = implName_->root;
    unsigned i = 0;
    for (i = 0; i < str.size(); ++i)
    {
        writeGuard<rwLock> guard(&lock_);
        node = node->AddChild((unsigned char)buf[i]);
    }
    node->SetEndNode(true);
}