const wchar_t* RDARScriptStringManager::getTagByName(int table, const wchar_t* a,
                                                     const wchar_t* b, bool flag) const
{
    // ORIG 0x8aa5e40（169 指令逐段转写）：全程窄 char[256] TLS 缓冲
    //（%gs 相对 -0x3c248 / -0x3c148）+ ss_sprintf<256> + strcmp，
    // 没有任何 std::wstring / std::allocator<wchar_t> 临时对象
    //（ORIG 调用点里 _ZNSbIwSt11char_traitsIwESaIwEEC1* / _ZNSaIwEC1Ev 全为 0）。
    // 原来的宽串拼接实现属臆造，每轮循环都多出 basic_string/allocator 构造
    // 与其 EH 清理（eager-ctor 冗余），故按 ORIG 形态改回窄串缓冲。
    // 【b28c-36 复验】fresh ndiff=4 / NEAR（169=169，助记符序列全同）：
    // 唯一残差 = 末尾 ss_sprintf 第 3 参 `(const char*)a` 的装载时刻——
    // ORIG `mov %eax,0xc(%esp); mov 0x10(%ebp),%eax; mov %eax,0x8(%esp)`
    // （先存 callee 返回值再载 a）；4.4 先 `mov 0x10(%ebp),%edx` 缓存再双 store
    // （P75 跨版本实参装载序）。探针 /tmp/b28c_l36_p5.cpp 五形实测：A(reinterpret_cast)
    // 与 B(C cast) 同产 edx 提前缓存；C(文件名局部)/D(指针局部)/E(双局部) 均新增
    // 栈槽且装载序更远 ⇒ 源形不可翻，判 NEAR 封顶。
    // TLS `lea -0x3c248(%eax)` vs `lea 0x0(%eax)` 与 .rodata addend 差属链接产物
    // （.o 未解析重定位；P155 TLS 豁免 + P196 .o 伪差），链接后随 <A> 归一化。
    static __thread char buf[256];      // ORIG TLS -0x3c248
    static __thread char errBuf[256];   // ORIG TLS -0x3c148

    (void)flag;                         // ORIG -0x2c：bool 形参归一化存栈后未再使用

    int idx = -1;                       // ORIG -0x24
    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        // ORIG -0x20 存的是 const operator[] 返回的引用地址，每次使用再解引用
        StringDictionary* const& d = dictionaries_[table];
        idx = getKeyIndex(table, a);
        if (idx == -1)
        {
            if (b != 0 && *b != L'\0')
                idx = getKeyIndex(table, b);

            const char* base = 0;       // ORIG -0x18
            if (idx != -1)
                base = d->getKey(idx);
            else
                base = reinterpret_cast<const char*>(tag_.c_str());  // ORIG 按窄串直用

            for (int i = 0; i < 10000; ++i)
            {
                ss_sprintf(buf, "%s_%d", base, i);
                bool exists = false;                       // ORIG -0xd
                for (int j = 0; j < d->getCount(); ++j)    // ORIG 每轮重调 getCount()
                {
                    if (strcmp(d->getKey(j), buf) == 0)    // ORIG 无 NULL 检查
                    {
                        exists = true;
                        break;
                    }
                }
                if (!exists)
                {
                    // ORIG：add(key=buf, value=a[0x10(%ebp)]) 直接把 wchar_t* 当 char* 传。
                    d->add(buf, reinterpret_cast<const char*>(a));
                    idx = d->getCount() - 1;
                    break;
                }
            }
        }

        int unusedSlot = 0;             // ORIG -0x1c：清零后未再引用
        (void)unusedSlot;
        if (idx != -1)
        {
            ss_sprintf(buf, "<%d::%s>", table, d->getKey(idx));
            return toUnicode(buf);
        }
    }

    ss_sprintf(errBuf, "`%s is not found, %s`",
               reinterpret_cast<const char*>(a), fileList_.GetFileName(table));
    return reinterpret_cast<const wchar_t*>(errBuf);
