namespace
{
// ORIG 0x086520b8/0x086520d3/0x08652103 直证：+0x4 起的 6 字节数组按**单 disp**
// `movb $0x0,0x4(%edx,%eax,1)` 访问（P311 成员数组视图），+0xc/+0xd/+0xe 亦为
// `mov %al,0xc(%edx)` 单 disp 直落。用具名视图替代 `((char*)sig)[4+i]`（后者在 4.4 生
// `add $0x4,%edx; add %edx,%eax` 两步 + 跨 call 的 lea 地址缓存）。
struct UpdateDimensionInoutSigView
{
    unsigned int m_characNo;    // +0x0
    char m_inoutValue[6];       // +0x4..+0x9
    char m_reservedA[2];        // +0xa..+0xb（ORIG 不写）
    // 三字段按 ORIG 0x0865217a 段形定 uchar：源 uchar 返回值同型直落单步
    // `mov %al,0xc(%edx)`；声明 char 会多一条 uchar→char 的 `mov %eax,%edx` 中转。
    unsigned char m_ultimateInout;  // +0xc
    unsigned char m_saveDimension;  // +0xd
    unsigned char m_saveBlood;      // +0xe
};
}

void CUser::UpdateDimensionInout()
{
    // [A06 2026-09-10 P259] ORIG 0x08651fba 段同形（Acquire 直落实参，无具名 stream 槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bd0), true);
    guard.operator*()->operator<<(0x14f);
    // ORIG 08652030 call operator<<(int)：传参为 int（GetUID 返回 uint，须显式转 int
    // 选中 operator<<(int) 重载——否则误落 operator<<(unsigned int)，const 重载甄别）。
    guard.operator*()->operator<<(static_cast<int>(GetUID()));

    UpdateDimensionInoutSigView* sig = reinterpret_cast<UpdateDimensionInoutSigView*>(
        guard->GetInBuffer<SIG_UPDATE_DIMENSION>());
    memset(sig, 0, 0x10);
    sig->m_characNo = getCurCharacNo();

    for (int i = 0; i <= 5; ++i)
    {
        char v = static_cast<char>(getDemensionInoutValue(static_cast<char>(i)));
        // [A06 2026-09-10 P0/求值序] ORIG 0x086520bc 先把 v 升格进 callee-saved
        // %ebx 再跨 G_CDataManager/get_dimensionInout 调用；`v > dim+10` 写法
        // 令 4.4 也先物化 v（左操作数在前），`dim+10 < v` 则先算右侧。
        // [A06 2026-09-10 P198②] 条件**整体内联进 if**：ORIG `test %al,%al; je`
        // 直测寄存器、无 bool 槽；提具名 bool 局部会多 `mov %al,-0x9; cmpb`。
        if (
            (v < 0) ||
            (static_cast<int>(v) > (G_CDataManager()->get_dimensionInout(i) + 10)))
        {
            sig->m_inoutValue[i] = 0;
            // [FL5 2026-09-10 P37/P227] 全表达式临时链
            cMyTrace(__PRETTY_FUNCTION__, 0x1bde, 0)(
                "DIMENSION DUNGEON INOUT_COUNT ABNORMAL USER!! CHARAC_NO(%d)",
                sig->m_characNo);
        }
        else
        {
            sig->m_inoutValue[i] = v;
        }
    }

    // [A06 2026-09-10 P300] 三处 uchar 返回值直落 uchar 成员：ORIG `mov %al,0xc(%edx)`
    // 单步；加 `(char)` 转换反而多一条 `mov %eax,%edx` 中转。
    sig->m_ultimateInout = getUltimateInoutValue('\x02');
    sig->m_saveDimension = IsSaveDemensionInoutValue();
    sig->m_saveBlood = IsSaveBloodInoutValue();
    SetSaveDemensionInoutValue(false);
    SetSaveBloodInoutValue(false);

    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
