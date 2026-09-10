CStaticDisposition::CStaticDisposition(CUser** users, int* counts) // ORIG 0x82e62aa
{
    // b28d-solo-CAssaultMgr: N 封顶（P30/N4/N102 族；清单 DIFF 98 复验）— 用
    // `reinterpret_cast<const int (&)[16]>(ASSAULT_RECT_PLAYER_POINT)[idx*2+1]`
    // 数组引用视图复现 ORIG m_y 的 `add %eax,%eax; add $1,%eax` 链与 scaled
    // `mov (,%eax,4)`（gap 4→1）。残差 = 2 处 add-vs-lea（ORIG
    // `mov k,%eax; lea (%edx,%eax,1),%eax` vs 4.4 `shl; add k_mem,%eax`，P30/N86b
    // 源形不可择）+ 1 条基址载入 `mov $TABLE,%eax`（引用 cast 恒载基址）。
    // 本轮补测 /tmp/probe_arr.cpp 八形（数组引用/指针 cast/数组指针 cast/元素
    // 取址+1/交换加序/元素成员 m_y/整数索引重组）：唯「数组元素成员访问」
    // 产绝对寻址但索引形态差 3 条；其余 cast 形全载基址 ⇒ 无更优源形。
    // 历史穷举并回退的替代形：①union 双视角（Point2d 有用户 ctor，C++98 union
    // 成员不得有构造函数，编译拒绝）；②指针 cast `((const int*)TABLE)[i]`
    // （our 157→159 更差）；③独立 int[16] 表（可消基址载入但 .rodata 重复
    // 0x40B 且两处符号地址不同，AE 归一化仍不等，回退）。
    // 语义等价（int[16] 视图与 Point2d[8] 同布局；idx*2+1 即 .m_y）。
    m_iconPos.m_x = ASSAULT_RECT_ICON_POINT[0];
    m_iconPos.m_y = ASSAULT_RECT_ICON_POINT[1];
    for (int j = 0; j <= 1; ++j)
    {
        // [b23S3C] counts[j]（非 j*4；int* 天然乘 4 字节）；P48 每循环迭代重读
        // counts[j]（ORIG 内层条件先判 counts[j]>k 再 k<=3）；ORIG 具名临时 Point2d
        // p 在 -0x30，独立于末端 origin(-0x28)，故两处各自命名。
        for (int k = 0; counts[j] > k && k <= 3; ++k)
        {
            // [SweepCAssaultMgr 复验] 索引加法序旋钮实测无效（`j*4+k` 与
            // `k+j*4` 同折 `shl; add k_mem`，ndiff 不变）⇒ 残差维持判词：
            // ORIG 082e6386 `mov j,%edx; shl $0x2,%edx; mov k,%eax;
            // lea (%edx,%eax,1),%eax`（两步 lea）+ `mov TABLE(,%eax,4),%edx`
            // 绝对寻址 vs 4.4 `shl; add k_mem` + 引用 cast 基址载入 —— P30/
            // N4/N102 族，八形探针（/tmp/probe_arr.cpp）无更优，封 N。
            Point2d p(ASSAULT_RECT_PLAYER_POINT[(k + j * 4) * 2],
                      ASSAULT_RECT_PLAYER_POINT[(k + j * 4) * 2 + 1]);
            m_mats[j].AddPoint(p);
        }
    }
    Point2d origin(users[0]->get_posX(), users[0]->get_posY());
    _MoveMatrix(origin);
