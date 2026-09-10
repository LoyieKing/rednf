int Manager::selectStage(CUser* user, int stageId, StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    if (diff < 0 || diff > 2)
    {
        return 8;
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
        return 3;
    }

    const int err = control->selectStage(user, stageId, diff);
    if (err != 0)
    {
        control->destroy();
        return err;
    }
    // ORIG +0xbf..：`call getIndex; cwtl; mov %eax,0x4(%esp); mov user,%eax;
    // mov %eax,(%esp); call CUser::setAdvanceAltarIndex(short)`。getIndex 返回 short，
    // cwtl 符号扩展入参槽，故必须走真实 setter（直写字段生 `mov %dx,disp` 异形）。
    user->setAdvanceAltarIndex(control->getIndex());
    return 0;
