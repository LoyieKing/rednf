// ORIG _ZN10StreamPool4FreeER12CStreamGuard W 0x08101080（弱内联，无独立报告）：
// objdump 0x8101080——取 guard 持有的 Stream* 后复用 Free(Stream*)。
bool StreamPool::Free(CStreamGuard& guard)
{
    return Free(guard.Get());
}