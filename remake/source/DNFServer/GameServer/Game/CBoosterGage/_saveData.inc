// [b28d-solo-CBoosterGage N104] 81/80（ORIG 尾含 1 条对齐 nop ⇒ 净 80=80），p167 15/15 CLEAR、
// 槽位集全等（guard@-0x14、pData@-0xc，bRet 走裸 register %ebx 与 ORIG 同）、逐条同构；
// 唯一残差 = EH 段物理位置：ORIG 正常路径 `jmp` 尾岛、landing pad 居中，4.4 恒正常路径
// fall-through 到 guard D1、pad 置尾（N104 无源码旋钮；throw() 会删 ORIG 存在的 pad）。ORIG 0x080dd340。
bool CBoosterGage::_saveData(CUser* pUser)
{
    // [b28c-11] N 封顶（81/80）：残余 = EH 段布局——ORIG 正常路径 `jmp` 尾岛
    // （guard D1 析构置于 +0x113 共享尾）且异常段在中间；4.4 正常路径
    // fall-through 到 guard D1、异常段置尾（负例表「EH 双份 dtor epilogue vs
    // merge」）。语义等价，无源码解。
    // [M4FreeNext15C.M4S1 round5 逐块复核 · 判 N104（父泳道要求）]
    // ① 块内容逐条全等、仅物理顺序差：两侧同为「正常尾块 = lea -0x14/mov/call
    //    CStreamGuardD1 + mov %ebx,%eax + epilogue」与「landing pad = mov edx/esi +
    //    lea/mov/call D1 + mov esi→eax/ebx→edx + call _Unwind_Resume」各一份，
    //    逐条同构；ORIG 尾 +0x127 另有 1 条对齐 `nop`（净 80=80）。
    //    ORIG `jmp +0x113`（尾岛）/ pad 居中；我方 fall-through 尾块 / pad 置尾。
    // [P6S3 round6 · 第三轮复核 · 维持 N104] 按父泳道要求只复核不重投：n1_v 读数
    // DIFF orig=81 our=81 gap=+0（旧判词 81/80）；逐块比对仍为「两块内容逐条同构、
    // 仅 EH 段物理顺序差」——ORIG 0x080dd43f `jmp +0x113` 跳尾岛、landing pad 居中，
    // 我方正常路径 fall-through 到 guard D1 尾块、pad 置尾。四数（identical/ae/
    // near/diff）无变化。本波新钥匙（P339 声明序 / P341 共享尾 / §6.5 串）对本件
    // 无作用面（无具名局部轴、无串缺口）⇒ N104 结论维持，不再重投入。
    // ② 语义轴全核：串 `"BoosterGage.cpp"` + 行 `0x193` 与源码逐字一致（我方 .o
    //    .rodata 偏移 0x90 处 `\0BoosterGage.cpp\0` 直证）；`SIG_BOOSTER_GAGE_DATA`
    //    写入 = ORIG 0x080dd3ff `call get_acc_id; mov %eax,(%edx)` 落 **+0**
    //    + 0x080dd40c `mov 0x8(%eax),%edx; mov %edx,0x4(%eax)` 落 **+4**（= this+8
    //    m_dwTotalGage），我方逐条相同；memset 8B、`<<0x2d8`、`<<GetUID()`、
    //    `MsgQueueMgr::put(DB_Q=2, guard)` 亦逐字一致 ⇒ 无语义残留。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("BoosterGage.cpp", 0x193), true);
    **guard << 0x2d8;
    **guard << (int)pUser->GetUID();
    SIG_BOOSTER_GAGE_DATA* pData = guard->GetInBuffer<SIG_BOOSTER_GAGE_DATA>();
    register bool bRet;
    if (pData == NULL)
    {
        bRet = false;
    }
    else
    {
        memset(pData, 0, sizeof(SIG_BOOSTER_GAGE_DATA));
        pData->m_dwAccountNo = pUser->get_acc_id();
        pData->m_dwBoosterGage = m_dwTotalGage;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        bRet = true;
    }
    return bRet;
}