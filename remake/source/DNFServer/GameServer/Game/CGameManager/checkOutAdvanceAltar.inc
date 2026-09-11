void CGameManager::checkOutAdvanceAltar(CUser* user)
{
    // P133 独立守卫 + X1 自然形（xor 物化）+ P63 goto 尾（空块 4.4 穿透，N 残差）。
    advancealtar::StageControl* control;
    if (user == 0) goto end;
    {
        // N96/P0：bool callee 裸形。
        if (!user->checkInAdvanceAltar()) goto end;
    }
    control = getAdvanceAltar(user->getAdvanceAltarIndex());
    if (control == 0) goto end;
    {
        // leaveUser 返回类型 bool 化后裸 !call() 直产 call;xor$1,%eax;test%al;je（P168a 技巧1）。
        if (!control->leaveUser())
        {
            LogManager::logFormat(1, "App.cpp",
                                  __PRETTY_FUNCTION__, 0x11ef,
                                  "invalid user : advance altar logout ID[%d] ACCID[%s]",
                                  control->getIndex(),
                                  NumberToString(user->get_acc_id(), 0));
        }
    }
end:
    ;
}