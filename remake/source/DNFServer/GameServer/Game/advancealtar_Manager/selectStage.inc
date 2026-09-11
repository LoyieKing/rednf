// ----------------------------------------------------------------------------
// ORIG T 0x0813090e _ZN12advancealtar7Manager11selectStageEP5CUseriNS_15StageDifficulty1TE
// user 判空 → 3；难度越界(<0||>2) → 8；getAdvanceAltar() 判空 → cMyTrace +
// "StaticPool Defence DungeonNULL" → 3；
//   StageControl::selectStage != 0 → control->destroy() 后透传错误码；
//   == 0 → setAdvanceAltarIndex(control->getIndex())（ORIG 经 CUser::setAdvanceAltarIndex
//   W 0x0813490e 写 m_advanceAltarIdx，此处字段化等价写）→ 返回 0。
// ----------------------------------------------------------------------------
ENUM_ERROR Manager::selectStage(CUser* user, int stageId, StageDifficulty::T diff)
{
    if (user == 0)
    {
        return (ENUM_ERROR)3;
    }
    if (diff < 0 || diff > 2)
    {
        return (ENUM_ERROR)8;
    }

    // ORIG +0x2f `movb $0x0,-0x11(%ebp)`：难度合法后、取 control 前，原码有一枚
    // 具名 char 局部被置 0（P0 槽-局部双向对应律：ORIG 有槽写⇒原码有该具名局部；
    // Ghidra `undefined1 local_15`）。该值后续无读取点（写入即弃），仅占栈槽。
    char isNull = 0;
    (void)isNull;
    StageControl* control = G_CGameManager()->getAdvanceAltar();
    if (control == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x6ab, 5);
        trace("StaticPool Defence DungeonNULL");
        return (ENUM_ERROR)3;
    }

    const int err = control->selectStage(user, stageId, diff);
    if (err != 0)
    {
        control->destroy();
        return (ENUM_ERROR)err;
    }
    // ORIG +0xbf..：`call getIndex; cwtl; mov %eax,0x4(%esp); mov user,%eax;
    // mov %eax,(%esp); call CUser::setAdvanceAltarIndex(short)`。getIndex 返回 short，
    // cwtl 符号扩展入参槽，故必须走真实 setter（直写字段生 `mov %dx,disp` 异形）。
    user->setAdvanceAltarIndex(control->getIndex());
    return (ENUM_ERROR)0;
}