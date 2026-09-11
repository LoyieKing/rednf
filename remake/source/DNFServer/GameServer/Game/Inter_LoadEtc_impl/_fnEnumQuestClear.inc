// [round5-A20 复核·N107/P23] fresh .o 40/42。残余=ORIG 0x084c2b0e/0x084c2b1a
//   两处 `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)`（2 条/处）vs 4.4 `addl $0x4,0xc(%ebp)`
//   （1 条/处）= N107 lea-vs-add 族（P319 整型加法+回 cast 形已试，4.4 恒 add）
//   + put_short 实参装载序 ORIG 先入 %edx（P23/P75 跨版本）。语义等价，封顶。
int Inter_LoadEtc::_fnEnumQuestClear(unsigned int questIdx, char* ctx)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=N 40/42：ctx 参数槽自增 ORIG 为
    // `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)` 两段、4.4 恒折 `addl $0x4,0xc(%ebp)`；
    // 另 put_short 实参装载序（ORIG 先载 edx）——跨版本 codegen，语义等价（p167 调用点一致）。
    // [b28c-lane25 判词·N] ORIG 0x084c2b0e 为 `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp);
    // mov (%eax),%eax`（先自增写回参数槽再读旧值）；4.4 恒「读值→addl 参数槽」，
    // 且 bool callee 直判恒多 setne（P168a③b 跨版本 bool ABI）。语义等价，封顶。
    // ORIG 0x84c2b08：static（0x8=questIdx 无 this）；ctx 参数槽原地自增
    // （lea 0x4; mov →0xc(%ebp) 后读旧值），源码 = 读旧值 + ctx+=4 两条语句。
    // [L22-L17 L1 已试并回退] P319 整型加法+回 cast 形：4.4 产
    // `mov 0xc(%ebp),%eax; add $0x4,%eax; mov %eax,0xc(%ebp)`（三/四条，`add` 非 `lea`，
    // 第二处再多一条 mov）⇒ 指令数 40→42 但形态仍 DIFF。N107 确认（4.4 恒 add）。
    InterfacePacketBuf* buf = *reinterpret_cast<InterfacePacketBuf**>(ctx);
    ctx += 4;
    int* count = *reinterpret_cast<int**>(ctx);
    ctx += 4;
    Quest* quest = G_CDataManager()->find_quest((int)questIdx);
    // ORIG 两 je 直跳同出口（嵌套 if 消 && 汇聚 bool 岛，P197④ 反向）。
    if (quest != 0)
    {
        // [b28d-53 判词·依赖上报] ORIG 0x084c2b55 调用点 `call; test %al,%al; je`
        // 免归一形（P292/P248 档①）；我方 callee 声明 `char check_clear_send()`
        // （signed char）致 4.4 生 `test %al; setne %al; test %al; je`。
        // ORIG callee 0x084e923c 为 `movzbl 0x44(%eax),%eax; ret` 单字节直返
        // （P9 ⇒ uchar/bool）。已上报 Main 分派 Quest.cpp 属主把返回类型
        // char→unsigned char（Quest.h:403 + Quest.cpp:391），落地后本处裸判归位。
        // 残余：lea-vs-add ×2（N107 封顶）+ put_short 实参装载序（P23）。
        if (quest->check_clear_send())
        {
            buf->put_short(questIdx);   // ORIG 直传完整 32 位（截断在 put_short 内）
            ++(*count);
        }
    }
    return 1;
}