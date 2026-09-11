void Encryption::getOriginalBuf(char* data, int len, unsigned int& seed)
{
    // [round5-Encryption] ORIG 0x0848dc4c 全函数**统一以 -0x144 为数组基址**：
    // 写 `-0x144(%ebp,%eax,1)`(eax=j)、读 `-0x144(%ebp,%edx,1)`(edx=j-1)、
    // 末尾 `-0x144(%ebp,%eax,1)`(eax=pos)、memcpy 源 `lea -0x144(%ebp)` 单条。
    // ⇒ 原码数组为 300 字节、有效数据自 index0 起（`tmp[j]=tmp[j-1]` /
    // `tmp[pos]=data[i]` / `memcpy(data,tmp,len)`）；此前写成「301 字节 +
    // 全体下标 +1」使 4.4 对每个 +1 发 lea/add 且 disp 停在 -0x145（gap=-1）。
    // 槽序 pos=-0x18/idx=-0x14/i=-0x10/j=-0xc；声明序照 ORIG（深→浅）。
    int pos;
    int idx = seed % 200;
    char tmp[300];
    int i;
    int j;
    for (i = 0; i < len; ++i)
    {
        // [round5-A26 判词·N（4.4 mem 直用族）] ORIG 0x0848dc8c 为
        // `mov -0x14,%edx; mov -0x10,%eax; imul $0x12c,%edx,%edx;
        // lea (%edx,%eax,1),%eax`（**两操作数均入寄存器**后 lea 合并，4 条）；
        // 4.4 恒折 `mov idx,%eax; imul $0x12c,%eax,%eax; add -0x10(%ebp),%eax`
        // （mem 直用，3 条，gap=-1）。本轮穷举 6 形均不产 ORIG 形：
        // ①`idx*300+i`（本形，56 条）②`i+idx*300`（同 56）③`300*idx+i`（同 56）
        // ④`*(index_array + idx*300 + i)` 数组名指针算术（60 条，劣）
        // ⑤`register int i`（57 条但整帧改观：i 失槽 + 数组 disp -0x140，语义等价性
        //   尚可，但改变了 ORIG 的 i 槽形 ⇒ 不用）⑥`unsigned int idx`（59 条，劣）。
        // ⇒ 4.4 对「寄存器+内存」加法恒 mem 直用（P209 附同族），源形不可择，封顶。
        pos = (int)index_array[idx * 300 + i];
        j = i;
        while (pos < j)
        {
            tmp[j] = tmp[j - 1];
            --j;
        }
        tmp[pos] = data[i];
    }
    std::memcpy(data, tmp, len);
}