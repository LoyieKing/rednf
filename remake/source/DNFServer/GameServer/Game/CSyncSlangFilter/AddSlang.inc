// [b28d-solo-CSyncSlangFilter] N：P75/N-G —— snprintf 首参 buf 装载时刻跨版本差（4.1 在 c_str 后立即 lea→%edx 并跨 varargs 保活，4.4 末位 lea→%eax）；(char*)buf 与逗号形实测同（&buf[0] 历史结论亦同）；ORIG 0x0862e72a。指令形直证：ORIG 0x0862e79c `lea -0x12d(%ebp),%edx` 紧随 0x0862e797 的 c_str call、先于 0x0862e7a6 `movl $"%s",0x8(%esp)`，保活到 0x0862e7b6 `mov %edx,(%esp)`。本波复核（我方 .o 与 ORIG 同口径逐条计数）两方均 113、仅此 2 行错位（N-G 六形穷举不可翻）。
void CSyncSlangFilter::AddSlang(const std::string& str)
{
    char buf[0x101] = {0};
    OS_API::snprintf(buf, 0x100, "%s", str.c_str());
    buf[0x100] = 0;
    SlangNode* node = impl_->root;
    if ((unsigned char)buf[0] == 0xbb &&
        (unsigned char)buf[1] == 0xd8 &&
        (unsigned char)buf[2] == 0xc3)
    {
        unsigned skip0 = 0;
        unsigned skip1 = 0;
        (void)skip0;
        (void)skip1;
    }
    unsigned i = 0;
    for (i = 0; i < str.size(); ++i)
    {
        writeGuard<rwLock> guard(&lock_);
        node = node->AddChild((unsigned char)buf[i]);
    }
    node->SetEndNode(true);
}