// ---------------------------------------------------------------------------
// loadFromBinary —— ORIG T 0x08ad4e04（报告全文；§6.2 大函数口径）。
// 【验收 batch-11】orig_insns=361 / our=363（gap −2）；P167 调用点差集归零
// （new[]/delete[]×2、memcpy×2、convertFromUtf8×2、convertToCharString×3、
// CharString D1×10/aSER×4/WString D1×6 全命中）。残余 2 行 = round4 的
// 4.1 sar/shr 有符号折叠 vs 4.4 cmovs（N81 族）与 XOR 循环体 shl+add vs
// 4.4 lea 折叠（N18a/P30），封顶勿再追。
// 分支一（SC01）：magic "S","C","0","1"；origSize=*(int*)(data+4)；
// payloadBytes=(size+3)/4*4（ORIG sar/shr 有符号 /4*4 形态 @0x069）；
// new[](payloadBytes) 后 memcpy(buf, data+8, size)（拷贝长度=size，
// P171 照抄）；running-key XOR：k0@-0x24 先于 new、cur/key 二次复制槽
// （-0x18/-0x14）、循环体三处地址重算（*p=*p^key; key^=*p，P38/P157）；
// convertFromUtf8(buf, origSize, ws) 结果落 byte 槽，delete[] 带显式判空
// 守卫，失败 return false（ws 具析构）；成功 content=convertToCharString(ws)。
// 分支二（UTF-8 BOM EF BB BF）：同形（无 buf）。
// 分支三（UTF-16LE BOM FF FE）：ORIG 直接 WideString(data+2,(size-2)/2)
// （wchar_t* 别名字节借用，i386 wchar=4B；P171 照抄，不做大小端重组）。
// 默认分支：具名 CharString(data,size) 临时 + op= + 具析构。
// 尾部统一（ORIG @0x3e3）：copyBuf=new char[content.length()+1];
// memcpy(copyBuf, content.c_str(), length+1); loadFromString(copyBuf,
// content.length(), error)（length 共调用 3 次，P48 无 CSE）;
// if (copyBuf) delete[] copyBuf; 返回结果 byte 槽聚合。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N(N97/N82/N95): (size+3)/4*4 的 4.1 sar/shr/lea 有符号 lowering
// vs 4.4 lea/test/cmovs（N97）+ 标量槽基深 4（ORIG origSize -0x2c 起，4.4 -0x30 起，
// 帧同 0x60；未使用局部 pad 反致整体再深 4）+ XOR 循环地址重算（P205/P183）；
// 361/363 gap −2，p167 CLEAR 42=42，语义完成 | ORIG 0x08ad4e04]
// [M2S5StringRaw 2026-09-11 复核] gap −2 = N97（(size+3)/4*4 的 ORIG `sar/shr/lea`
// 有符号 lowering vs 4.4 `lea/test/cmovs`）+ N18a（XOR 循环 ORIG `shl $2,%eax;
// add <D>(%ebp),%eax` vs 4.4 `lea (%eax,%edx,1)` 地址折叠），均 4.4 指令选择，
// P329/P231/P226 无对应源形可翻 ⇒ 封顶维持。
bool StringResource::loadFromBinary(const unsigned char* data, int size, CharString* error)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a/N97）→ 试=④P226/N97：
    //   `payloadBytes=(size+3)/4*4` 的 ORIG 形 `mov %eax,%edx; sar $0x1f,%edx; shr $0x1e,%edx;
    //   lea (%edx,%eax,1),%eax; sar $2; shl $2`（4.1 expand_divmod）vs 4.4
    //   `lea 0x3(%eax),%edx; test %eax,%eax; cmovs %edx,%eax; sar $2; shl $2`；显式位运算
    //   展开需引两次 `(size+3)`（净 +3 条、且 ORIG 的 sar/shr 是作用在已 +3 的 %eax 上）⇒
    //   反劣；XOR 循环 `shl $2; add <D>(%ebp)` vs 4.4 `lea (%eax,%edx,1)` 折叠同属指令选择。
    //   ②prett ③strall CLEAR。⇒ 维持 N97/N18a。
    CharString content;                              // -0x48

    if (size > 4 && data[0] == 'S' && data[1] == 'C' &&
        data[2] == '0' && data[3] == '1')
    {
        int origSize = *reinterpret_cast<const int*>(data + 4);     // -0x2c
        int payloadBytes = (size + 3) / 4 * 4;                      // -0x28
        unsigned int k0 = 0x5819af17u;                              // -0x24
        char* buf = new char[payloadBytes];                         // -0x20
        memcpy(buf, data + 8, size);

        // [N1F 2026-09-11 实测复核] 该未用 pad 局部是**必需**的：删除后本函数槽基
        //   整体上浮 4（ORIG content -0x48 vs 我方 -0x44），归一 diff 208 行 → 398 行
        //   ⇒ 该 pad 净收益为正，保留（N97/N18a 残余本身仍封顶）。
        int slotPad;
        char* cur = buf;                                            // -0x18
        unsigned int key = k0;                                      // -0x14
        for (unsigned int i = 0; i < (unsigned int)payloadBytes / 4; ++i)
        {
            *reinterpret_cast<unsigned int*>(cur + i * 4) =
                *reinterpret_cast<unsigned int*>(cur + i * 4) ^ key;
            key = key ^ *reinterpret_cast<unsigned int*>(cur + i * 4);
        }

        WideString ws;
        bool converted = convertFromUtf8(buf, origSize, &ws);
        delete[] buf;                        // ORIG @0x12b 单对判空守卫为
                                             // 编译器隐式形（P141），勿再显式
        if (!converted)
            return false;
        content = convertToCharString(ws);
    }
    // BOM 常量用无符号立即数值（ORIG cmp $0xef/$0xbb/$0xbf,%al）：
    // '\xef' 等 char 字面量为负值，与 uchar 提升值恒不等，4.4 折叠死分支。
    else if (size > 3 && data[0] == 0xef &&
             data[1] == 0xbb && data[2] == 0xbf)
    {
        WideString ws;
        bool converted = convertFromUtf8(
            reinterpret_cast<const char*>(data + 3), size - 3, &ws);
        if (!converted)
            return false;
        content = convertToCharString(ws);
    }
    else if (size > 2 && data[0] == 0xff && data[1] == 0xfe)
    {
        WideString ws(reinterpret_cast<const wchar_t*>(data + 2),
                      (size - 2) / 2);
        content = convertToCharString(ws);
    }
    else
    {
        CharString tmp(reinterpret_cast<const char*>(data), size);
        content = tmp;
    }

    char* copyBuf = new char[content.length() + 1];                 // -0x34
    memcpy(copyBuf, content.c_str(), content.length() + 1);
    bool result = loadFromString(copyBuf, content.length(), error); // -0x2d
    delete[] copyBuf;                    // 同上 P141：隐式单守卫
    return result;
}