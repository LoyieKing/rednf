bool CaptchaGen::addFont(const char* font, int size)
{
    // ORIG 0x080de794（report 汇编+反编译 C 全读）：FT_New_Face(m_library, font, 0,
    // &face) → 失败返 false → FT_Set_Pixel_Sizes(face, 0, size) → 失败返 false →
    // FT_Select_Charmap(face, FT_ENCODING_UNICODE=0x756e6963) → 失败返 false →
    // m_faces.push_back(face)（vector<void*>）→ 返 true。
    // [b28 BM2 判词·AE] 清单 NEAR(2) 为全二进制全局地址标签伪差；fresh
    // /tmp/tu_GlobalData.o fn_diff_tu 归一 0 差（48=48 逐条）。
    // 原转写以 fopen/fclose/malloc/strcpy 假实现替代真 FT 序列（违 §4.1），已改为
    // 忠实 ORIG；face 错误码 slot 落 -0xc 与 `cmpl $0,slot; je 成功` 逐指令对应。
    void* face;       // ORIG local_18 @ -0x14（FT_New_Face 出参，未初始化）
    int err = FT_New_Face(m_library, font, 0, &face);   // ORIG local_10 @ -0xc
    if (err != 0)
    {
        return false;
    }
    err = FT_Set_Pixel_Sizes(face, 0, (unsigned int)size);
    if (err != 0)
    {
        return false;
    }
    err = FT_Select_Charmap(face, 0x756e6963ul);
    if (err != 0)
    {
        return false;
    }
    // [b28d-solo-GlobalData] P315/P234：ORIG 0x080de81a `mov -0x14,%eax; mov %eax,-0x10;
    // lea -0x10,%eax; mov %eax,0x4(%esp)` 调 `push_back(OS0_)`（void*&& 右值重载）；
    // 具名左值 toPush 选 RKS0_（p167 MISSING OS0_/EXTRA RKS0_）⇒ 写 `(void*)face`
    // 产生 prvalue 临时（-0x10 拷贝槽）绑定右值引用。
    m_faces.push_back((void*)face);
    return true;
