// [b23S3C] A：真修 counts[j*4]→counts[j]（原越界读）、P48 内层每次重读 counts[j]、
// icon 字段拆两句对齐 this 重载 store；残余 = N86(4.1 临时+拷贝 vs 4.4 直构形)
// + N87(ARP 索引 add/lea vs 折叠位移) + CDispositionC1vsC2 基构造（跨版本），
// fresh .o 复验 our=154/orig=158，调用点差集仅 C1/C2 同名差，语义等价。
// [round1-L29 2026-09-10 复核] 判据⑪ 槽位直方图逐值全等 ⇒ 非槽问题。唯一残差 = 索引两步寻址
//   `j*4+k`：ORIG `mov j,%edx; mov k,%eax; shl $2,%edx; lea (%edx,%eax,1),%eax` vs 4.4
//   `mov j,%eax; shl $2,%eax; add k_mem,%eax`（add-vs-lea，P30/N4/N102；八形探针全败）⇒ N。
//   ORIG 0x082e6386。
// [S6 2026-09-11 复核] 清单所指「缺 `CDisposition::C1Ev` 调用」= **C1/C2 同址别名假缺口**
//   （非语义）：ORIG 调用点 0x082e62b9 → callee 0x082f1052（14B，体只写 vptr 0x8c1d1c8）；
//   我方同位置 call 重定位到 `_ZN11pvp_assault12CDispositionC2Ev`，本 .o 内 C1Ev/C2Ev
//   皆 0x0e 字节弱符号**同址别名** ⇒ 运行/布局等价；p167 差集 CLEAR（余项仅 C1/C2、
//   D1/D2 别名）。改动前后 158/157、ndiff 17 不变 ⇒ 判 N（无源码旋钮）。
// [P8S1 2026-09-11 本波复核] 维持封顶：N4/P30 索引展开(add-vs-lea;ORIG 0x82e8d60 段 lea(%edx,%eax,1))；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
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
}