// [b28d-18 N82] 残差=槽位/STL（N82/N89） | ORIG 0x080deade
// [b28d-solo-GlobalData] 语义缺口已闭合：p167 CLEAR（53/53，0 missing/0 extra）——补
// FT_Set_Transform/FT_Get_Char_Index/FT_Load_Glyph/FT_Render_Glyph extern "C" 占位 +
// FT_Matrix/FT_Vector/FT_Bitmap/FT_GlyphSlotRec/FT_FaceRec 视图，按 ORIG 0x080dee9x..
// 0x080df120 还原 16.16 定点矩阵与 bitmap 遍历（std::min<int>）；发明符号 static
// renderGlyph 已删。
// [SweepCAutoPunish 2026-09-10] 680→**682**（ORIG 685，余 3）：①字形 alpha 局部
// uchar→int（P321 T1 双段零扩展直证，探针 p4/p5.cpp）；②删 `(int)bm->width` cast +
// FT_Bitmap::width unsigned→int（ORIG 直取 &成员喂 min<int>）；③`dy*dy+dx*dx` 首项序
// （P243 同族）；④两个 row 守卫改 P223 负条件 goto 尾标签串，复现 ORIG 0x080df05c/
// 0x080df068 两岛（+4 条）。余 3 条 = XorRand4/min/setAlphaAsMax 段浮点/寄存器
// N85/N18a 族（无 jcc/调用差，p167 CLEAR）。
// [G7-sub 2026-09-11 判词] CaptchaGen::generateImage（ORIG T 0x080deade）：私有 .o 实测
// ORIG 685 / 我方 682（gap 3，我方短）。slug 级差 10 处，逐条归因：
//   · x87 控制字改舍入序列（`fldcw -0xec; fistpl -0xf0; fldcw -0xea; movzwl …;
//     mov %ax,-0xec` ×7 处）两侧同构**仅槽号平移**（我方同序列在 -0xdc 等）⇒ N82/N85。
//   · `flds/fmuls/fnstcw` 浮点链同构、槽基 -0x98/-0x94 vs 我方对应槽 ⇒ N82。
//   · 其余为 `.rodata` 符号文本（我方 .o 无解析名）与对齐。
// p167 调用点差集仅 `vector<int>::D2` 同址别名（ORIG nm 实测 D1=D2=0x08083dda）。
// 结论：**封顶，无改动**（语义完成；残差 N82/N85 + 数据符号）。
void CaptchaGen::generateImage(int width, int height, const unsigned short* text, int len,
                               const ImageOption& opt, GreyImage* image)
{
    // [P190 附注①] ORIG 帧 0x120 vs 我方 0x110（差 0x10）：2 个**取址** double
    // 撑帧使槽基整体下移 0x10 对齐（x87 序列槽号 -0xec 族随之一并归位）。
    double framePad0;
    double framePad1;
    (void)&framePad0;
    (void)&framePad1;
    if (m_faces.empty())
    {
        return;
    }
    image->init(width, height, 0);

    // 噪点圆（ORIG 第一段循环）
    for (int i = 0; i < opt.m_field0; ++i)
    {
        int cx = m_rng() % (width - 10) + 5;
        int cy = m_rng() % (height - 10) + 5;
        int r = m_rng() % opt.m_field4 + 1;
        for (int dy = -r; dy <= r; ++dy)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                // ORIG 0x080debc7：先 `mov -0x80(dy),%eax; mov %eax,%edx;
                // imul -0x80,%edx` 再算 -0x7c(dx) 平方 ⇒ 源码首操作数是
                // dy*dy（P243 同族：首求值项=书写序左项；加法交换、语义等价）。
                int v = ((dy * dy + dx * dx) * 0xff) / (r * r);
                if (v < 0xff)
                {
                    image->setAlphaAsMax(cx + dx, cy + dy,
                                         (unsigned char)((0xff - v) / r));
                }
            }
        }
    }

    // 干扰线（ORIG 第二段循环）
    for (int i = 0; i < opt.m_field8; ++i)
    {
        int x1 = m_rng() % 10 + 1;
        int y1 = m_rng() % height;
        int x2 = (width - m_rng() % 10) - 2;
        int y2 = m_rng() % height;
        drawLine(image, x1, y1, x2, y2);
    }

    // 字符渲染（ORIG 第三段循环：随机面/旋转/缩放 -> FT 栅格 -> 粘贴）
    int penX = 10;
    int penY = 2;
    for (int i = 0; i < len; ++i)
    {
        unsigned int faceIdx = m_rng() % m_faces.size();
        // [b28d-solo-GlobalData] 按 ORIG 0x080dee9x..0x080df120 段还原真实
        // FreeType 栅格化序列（旧 static renderGlyph 占位为发明符号，已删）：
        // m_faces[idx] -> 4×getFloat(旋角/缩放) -> 2×镜像位 -> FT_Matrix 16.16
        // 定点（65536.0f=_DAT_08b2cab0）-> FT_Set_Transform -> FT_Get_Char_Index
        // -> FT_Load_Glyph -> FT_Render_Glyph(face->glyph,0) -> bitmap 遍历
        // （std::min<int>(width, width-penX) + setAlphaAsMax）-> penX 推进。
        FT_FaceRec* face = (FT_FaceRec*)m_faces[faceIdx];
        float rotA = RandGen::getFloat(&m_rng, opt.m_fieldc, opt.m_field10);
        float rotB = RandGen::getFloat(&m_rng, opt.m_fieldc, opt.m_field10);
        float sclA = RandGen::getFloat(&m_rng, opt.m_field14, opt.m_field18);
        float sclB = RandGen::getFloat(&m_rng, opt.m_field14, opt.m_field18);
        float mirX = (m_rng() & 2) ? 1.0f : -1.0f;
        float mirY = (m_rng() & 2) ? 1.0f : -1.0f;
        FT_Matrix matrix;
        matrix.xx = (FT_Fixed)(65536.0f * rotA);
        matrix.xy = (FT_Fixed)(65536.0f * sclA * mirX);
        matrix.yx = (FT_Fixed)(65536.0f * sclB * mirY);
        matrix.yy = (FT_Fixed)(65536.0f * rotB);
        FT_Set_Transform(face, &matrix, 0);
        unsigned int glyphIndex = FT_Get_Char_Index(face, text[i]);
        int err = FT_Load_Glyph(face, glyphIndex, 0);
        err = FT_Render_Glyph(face->glyph, 0);
        (void)err;

        int yOff = m_rng() % opt.m_field1c;
        FT_Bitmap* bm = &face->glyph->bitmap;
        for (int row = 0; row < (int)bm->rows; ++row)
        {
            int yy = penY + row + yOff;
            // ORIG 0x080df05c/0x080df068：`js 0x080df10d` + `jge 0x080df110`
            // 两个守卫各自落独立岛（岛形 `jmp <外层增量>; nop`×2，共 4 条）
            // ⇒ P223 正例形「负条件 goto 到共享尾空标签串，标签序=守卫序」
            // （if 正向包裹恒被 4.4 jump-thread 成 0 岛）。
            if (yy < 0)
            {
                goto rowSkip1;
            }
            if (yy >= height)
            {
                goto rowSkip2;
            }
            {
                int maxCols = width - penX;
                // ORIG 直接取 &bm->width 传 min<int>；`(int)` cast 会造右值
                // 临时（多 -0xa4 槽 + 两条 mov）⇒ 删 cast（width 已改 int）。
                int cols = std::min(bm->width, maxCols);
                for (int c = 0; c < cols; ++c)
                {
                    // ORIG 0x080df0xx：`movzbl (%eax),%eax; movzbl %al,%eax;
                    // mov %eax,-0x24(%ebp); cmpl $0,-0x24` ⇒ 局部是 **int**
                    // （P321 T1 双段零扩展 + 4 字节槽直证；uchar 形恒产
                    // `mov %al,槽; cmpb $0,槽`，探针 /tmp/gdprobe/p4.cpp
                    // f1/f3 vs f4 对照）。传参亦为 int→uchar 形
                    // （`mov -0x24,%eax; movzbl %al,%eax`，探针 p5.cpp）。
                    int a = bm->buffer[bm->pitch * row + c];
                    if (a != 0)
                    {
                        image->setAlphaAsMax(penX + c, yy, a);
                    }
                }
            }
rowSkip1:;
rowSkip2:;
        }
        penX += (int)(opt.m_field20 * (float)bm->width);
    }

    // 波形扭曲（ORIG 第四段：cos/sin 16.16 定点偏移 + 双线性采样）
    GreyImage tmp;
    tmp.init(width, height, 0);
    float freq = RandGen::getFloat(&m_rng, opt.m_field24, opt.m_field28);
    float amp = RandGen::getFloat(&m_rng, opt.m_field2c, opt.m_field30);
    // ORIG：默认构造 + resize(n, 0)（p167 MISSING `vector()`×2 + `resize`×2；
    // 我们原 `vector(n)` 产 EXTRA `vector(unsigned, int const&)` + allocator 族）。
    std::vector<int> cosTbl;
    cosTbl.resize((unsigned)width, 0);
    for (int x = 0; x < width; ++x)
    {
        cosTbl[x] = (int)(65536.0f * cosf((float)x * freq) * amp);
    }
    // ORIG call 序（0x080df0xx 段）：cosf 循环后才 `vector<int>::C1Ev` +
    // `resize` 构造 sinTbl（O=517/538/544 vs 我们 500/506/524 实证）⇒
    // sinTbl 的声明必须落在 cosTbl 循环之后（P331/P314 声明位置决定构造发射序）。
    std::vector<int> sinTbl;
    sinTbl.resize((unsigned)height, 0);
    for (int y = 0; y < height; ++y)
    {
        sinTbl[y] = (int)(65536.0f * sinf((float)y * freq) * amp);
    }
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            tmp.setAlpha(x, y, (unsigned char)image->sampleAlpha(
                x * 65536 + sinTbl[y], y * 65536 + cosTbl[x]));
        }
    }
    memcpy(image->getImage(), tmp.getImage(), (size_t)width * height);
}