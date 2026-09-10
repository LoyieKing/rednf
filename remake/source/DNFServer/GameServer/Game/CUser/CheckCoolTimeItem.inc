bool CUser::CheckCoolTimeItem(unsigned long itemIdx) const
{
    // ORIG 0x0865e9a6 `test %eax,%eax; setne %al; test %al,%al; je <尾 0xc4>，主体 fall-through`
    // ⇒ 正向包裹（P59/P222）；写 `if (f()==0) return false;` 会产 sete + 早退岛（多 1 条）。
    if (getCurCharacR() != 0)
    {
        const std::vector<ContinuousItemInfo>* cool =
            GetCurCharacCoolTimeItemListR();
        // 负结论（2026-09-10）：ORIG it@-0x10 / end@-0x14 与我方 it@-0x14 / end@-0x10
        //   互换；试「头部裸声明 endIt 先声明（先声明深）+ 先赋值后 for」(P212/P309)
        //   实测反劣（missing_in_game 588→590，尾段多 island），已回滚 ⇒ 维持本形，
        //   残差 = it/end 两级槽互换（N93 边界，无三级转换临时 ⇒ P326 不适用）。
        for (std::vector<ContinuousItemInfo>::const_iterator it = cool->begin();
             it != cool->end(); ++it)
        {
            // b28d-solo-CUser p167 修正：ORIG 0x0865e994 发 deEv（`(*it).`）非 ptEv。
            if ((unsigned long)(*it).m_itemIdx == itemIdx)
            {
                // ORIG 0x0865ea04 才发 getCurSec（命中后求值，%ebx 跨 operator* 保活，
                // **无具名 curSec 槽**）⇒ 内联表达式，勿提到循环外（P0/P183）。
                if ((unsigned int)GlobalData::s_systemTime_.getCurSec()
                    < (*it).m_expireTime)
                    return true;
                return false;
            }
        }
    }
    return false;
