// switchInputStream（ORIG 0x8aceaa0）：按流内容魔数判断编译态/文本态：
//   - s==0 时先 clear 局部 stream_data_t 并 LoadStream；失败经 lexerError_
//     回调报错返回 0；
//   - size>1 时读 2 字节魔数 0xd0b0 → COMPILED：转发 CompiledDNFLex::
//     SwitchInputStream（buffer 为读取区或新分配拷贝；若用 sd->stream 读则
//     完成后释放 istream）；否则 NORMAL：转发 DNFLex::SwitchInputStream。
// b28d-21 + E06 复测: NEAR 封顶 — 120=120 助记符序列全同，唯一实质残差为
// lexerError_（0x6108(%esi)）载入寄存器（ORIG `mov 0x6108(%esi),%ebx; test %ebx,
// %ebx` vs 4.4.x `%edx`）。E06 实测矩阵：①本 TU 路由 c6446r -O2 → %edx；
// ②c6root 4.4.7 全旗标 -O2（无路由改写）→ %edx。⇒ N7/P321-T5 跨版本寄存器
// 角色差，无源码旋钮（后续同址 `call *0x6108(%esi)` 两侧逐条一致，证明语义等价）。
// 另余 ORIG `nop; lea 0x0(%esi,%eiz,1),%esi` 与本方 `lea…; nop` 的对齐填充顺序差
// （§6.1 填充豁免）。ORIG 0x8aceaa0。
bool DNFLexWrapper::switchInputStream(const char* dir, const char* filename,
                                      DNFLex::stream_data_t* s)
{
    char errbuf[0x400];
    DNFLex::stream_data_t local;
    short magic;
    // [P6S3 round6 · §6.5 串审计命中复核 → 判**工具归因假阳性**，非语义缺口]
    // strall.py 报本函数 `O-only[1] %s/%s Read failed!`；`strings -a
    // /tmp/tu_DNFLexWrapperStream.o` 直证该串在我方 .o 中（首串），且下方
    // `ss_sprintf(errbuf, "%s/%s Read failed!", toMbcs(dir), toMbcs(filename))`
    // 逐字存在 ⇒ 非缺串。残余真差为 N22/N82 寄存器-槽族（124/127）。

    if (s == 0)
    {
        s = &local;
        s->clear();
        if (!LoadStream(dir, filename, s))
        {
            if (lexerError_ == 0)
                return false;
            ss_sprintf(errbuf, "%s/%s Read failed!",
                       toMbcs(dir), toMbcs(filename));
            reinterpret_cast<void (*)(const char*)>(lexerError_)(errbuf);
            return false;
        }
    }
    if (1 < (unsigned)s->size)
    {
        reinterpret_cast<std::istream*>(s->stream)->read(
            reinterpret_cast<char*>(&magic), 2);
        reinterpret_cast<std::istream*>(s->stream)->seekg(-2, std::ios::cur);
        if (magic == static_cast<short>(0xd0b0))
        {
            char* pc = s->buffer;
            lexType_ = LEX_TYPE_COMPILED;
            if (pc == 0)
            {
                pc = new char[s->size];
                reinterpret_cast<std::istream*>(s->stream)->read(pc, s->size);
                return compiledDnfLex_.SwitchInputStream(
                    dir, filename, pc, static_cast<unsigned int>(s->size));
            }
            bool ret = compiledDnfLex_.SwitchInputStream(
                dir, filename, pc, static_cast<unsigned int>(s->size));
            if (s->stream != 0)
            {
                void (**vtable)(void*) =
                    *reinterpret_cast<void (***)(void*)>(s->stream);
                vtable[1](s->stream);
            }
            return ret;
        }
    }
    lexType_ = LEX_TYPE_NORMAL;
    return dnfLex_.SwitchInputStream(dir, filename, s);
}