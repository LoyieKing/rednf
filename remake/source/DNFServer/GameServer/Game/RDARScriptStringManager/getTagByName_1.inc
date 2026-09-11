// b28d-solo-NEAR 封顶（P75）：169=169 助记符序列全同，唯一残差 = 末尾
// ss_sprintf 第 3 变参的装载时刻——ORIG `mov %eax,0xc(%esp); mov 0x10(%ebp),%eax;
// mov %eax,0x8(%esp)`（先存 callee 返回值再载 a），4.4.6 恒
// `mov 0x10(%ebp),%edx; mov %eax,0xc(%esp); mov %edx,0x8(%esp)`。
// 本波 4.4.6 路由下复测 14 形探针（/tmp/p75.cpp+m9..m14）全部同形：
// reinterpret_cast / 双层 static_cast / C cast / `(unsigned)a` / `(long)a` /
// `+0` / `(0,expr)` / 逗号 / `&p[0]` / `&0xffffffff` / `^0` / `*1` / `/1` / `-0`
// —— 4.4 对「末实参为 call + 倒数第二为形参」恒该序，源形不可翻。
// ORIG 0x8aa5e40。
// [M2S5StringRaw 2026-09-11 复核] 唯一残差（末尾 ss_sprintf 第 3 参装载时刻：
// ORIG 先存 callee 返回值再载 a vs 4.4 先缓存 a 入 %edx）本波新钥匙复核：
// P329 内联回实参位 / P226 语句表达式 / 类型旋钮均无源形可翻（上列 14 形探针
// /tmp/p75.cpp+m9..m14 已覆盖）⇒ P75 封顶维持，169=169 助记符序列全同。
const wchar_t* RDARScriptStringManager::getTagByName(int table, const wchar_t* a,
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
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
    // [Q7S2 2026-09-11 符号名轴] ORIG 局部 static 的 mangled 名 = `buffer` / `errStr`：
    //   nm 直证 `_ZZNK23RDARScriptStringManager12getTagByNameEiPKwS1_bE6buffer` 与
    //   `…E6errStr`（原 `buf` / `errBuf` 为我方发明名，ORIG 无 `…E3buf`/`…E6errBuf`）。
    //   纯改名，TLS 语义与布局不变，零运行期影响。
    static __thread char buffer[256];      // ORIG TLS -0x3c248
    static __thread char errStr[256];      // ORIG TLS -0x3c148

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
                ss_sprintf(buffer, "%s_%d", base, i);
                bool exists = false;                       // ORIG -0xd
                for (int j = 0; j < d->getCount(); ++j)    // ORIG 每轮重调 getCount()
                {
                    if (strcmp(d->getKey(j), buffer) == 0)    // ORIG 无 NULL 检查
                    {
                        exists = true;
                        break;
                    }
                }
                if (!exists)
                {
                    // ORIG：add(key=buf, value=a[0x10(%ebp)]) 直接把 wchar_t* 当 char* 传。
                    d->add(buffer, reinterpret_cast<const char*>(a));
                    idx = d->getCount() - 1;
                    break;
                }
            }
        }

        int unusedSlot = 0;             // ORIG -0x1c：清零后未再引用
        (void)unusedSlot;
        if (idx != -1)
        {
            ss_sprintf(buffer, "<%d::%s>", table, d->getKey(idx));
            return toUnicode(buffer);
        }
    }

    // [M2S5E 2026-09-11] P75 装载序旋钮（变量轴：显式 cast → 裸形参）：
    //   ORIG 0x8aa5e9a `mov %eax,0xc(%esp); mov 0x10(%ebp),%eax; mov %eax,0x8(%esp)`
    //   （先存 callee 返回值，再直接载形参 a）里**没有**任何转换指令；旧形
    //   `reinterpret_cast<const char*>(a)` 让 4.4 视其为需提前求值的表达式，
    //   产 `mov 0x10(%ebp),%edx; mov %eax,0xc(%esp); mov %edx,0x8(%esp)`（+1 条 edx
    //   缓存）。ss_sprintf 为 `template<int N> int ss_sprintf(char (&buffer)[N],
    //   const char* format, ...)`：N 仅由 errBuf[256] 推导，实参位无类型约束
    //   ⇒ 传裸 a（同一指针值，位模式相同）不新实例化符号、语义等价。
    ss_sprintf(errStr, "`%s is not found, %s`",
               a, fileList_.GetFileName(table));
    return reinterpret_cast<const wchar_t*>(errStr);
}