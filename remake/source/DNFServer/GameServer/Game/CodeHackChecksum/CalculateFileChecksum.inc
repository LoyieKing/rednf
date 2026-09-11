// ---------------------------------------------------------------------------
// CalculateFileChecksum(CodeHackChecksum*, FILE*, ulong, ulong, void*) T @0x0808a505
//   total==0 → ZeroMemoryChecksum；否则按 chunkSize 循环 fread（每次 1 元素），
//   fread 失败即 finish 收尾；成功则 update 并累计，读满 total 后 finish。
// 第 4 参为每轮读取块大小（ORIG 分支：chunk = min(chunkSize, total - done)）。
// ---------------------------------------------------------------------------
void CalculateFileChecksum(CodeHackChecksum* ck, FILE* fp, unsigned long totalSize,
                           unsigned long chunkSize, void* buffer)
// [b28c-L23 N18a/N7 残余 2 行] 本件已修一处**真语义 bug**（fread 实参序：ORIG
// @0x0808a53c 压参 `(%esp)=buffer / 0x4=chunk / 0x8=$1 / 0xc=fp` ⇒
// fread(ptr,size=chunk,nmemb=1)，原码 `(buffer,1,chunk)` 在 chunk>1 短读时返回值
// 语义分叉）。残 2 行 = `chunkSize + done` 的加法操作数寄存器来源
// （4.1 左操作数入 %eax/右入 %edx，4.4 反向 ⇒ 同助记符异寄存器，NEAR 级）。
{
    unsigned long done = 0;
    if (totalSize == 0)
    {
        ZeroMemoryChecksum(ck);
        return;
    }

    sha256_context ctx;
    sha256_starts(&ctx);
    for (;;)
    {
        unsigned long chunk;
        // [b28d-lane16] ORIG 0x0808a539 `mov 0x14(%ebp),%eax; mov -0x10(%ebp),%edx;
        // lea (%edx,%eax,1),%eax`：4.1 对 `done + chunkSize` 先载右操作数(chunkSize)
        // 入 %eax、左(done)入 %edx；4.4 恒先载右 —— 书写序翻转为 done+chunkSize 即
        // 逐条复现（P243 整数类比：加法书写序决定两操作数装载序）。
        if (done + chunkSize < totalSize)
            chunk = chunkSize;
        else
            chunk = totalSize - done;

        // b28c-l23 语义修正（ORIG 0x0808a53c 实参槽逐位核对）：ORIG 压参为
        // `(%esp)=buffer / 0x4=chunk / 0x8=$1 / 0xc=fp` ⇒ **fread(ptr, size=chunk,
        // nmemb=1, stream)**（每轮 1 个元素、元素大小=chunk），非 `(buffer,1,chunk)`；
        // 尾 `cmp $0x1,%eax` 判的是元素计数。原形在 chunk>1 短读时返回值语义分叉。
        if (fread(buffer, chunk, 1, fp) != 1)
        {
            sha256_finish(&ctx, ck->m_data);
            return;
        }
        sha256_update(&ctx, static_cast<unsigned char*>(buffer), chunk);
        done += chunk;
        if (totalSize <= done)
            break;
    }
    sha256_finish(&ctx, ck->m_data);
}