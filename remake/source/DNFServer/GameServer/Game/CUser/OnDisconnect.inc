// 0x086804ce
// ORIG：断线回调。state==7（PvP 房间）时通知 PvP_Room::Disconnect；
// 在袭击区域（GetAssaultPlace != 0）时通知 CAssaultMgr::OnLeaveAssaultPlace(user, false)。
void CUser::OnDisconnect()
{
    if (get_state() == 7)
    {
        // ORIG 0x08??：pvpIdx 走 %ebx（`movswl %ax,%ebx` 单条跨 G_CGameManagerv
        // 存活）。short→int 拓宽必须作为 GetPvp 首实参**直传**（隐式转换，同
        // getBlueMarble probe s3）；`register int asm("ebx")` 中间变量反产 cwtl+mov。
        // pRoom 槽落 -0xc（无 short spill，技巧 21）。末段 ORIG 对
        // GetInstanceAssaultMgr 结果**无判空**、%eax 直用（技巧 37）。
        PvP_Room* pRoom =
            G_CGameManager()->GetPvp(GetPvpIndex(), this, 0);
        if (pRoom != 0)
            pRoom->Disconnect(this);
    }
    if (GetAssaultPlace() != 0)
        pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(this, false);
}