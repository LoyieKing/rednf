// ORIG _ZN5CUser17CheckCoolTimeItemEm @ 0x0865e994 (T)
// 当前角色 cooltime 列表中存在 itemIdx==param_1 且 expireTime>curSec 的条目。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865e994 | _ZNK5CUser17CheckCoolTimeItemEm
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
        // b28d-lane5 翻案（probe_slots f1/f2，4.4.6 同路由）：仍缓存 end()，但具名序必须是
        //   **it 先声明、itEnd 后声明**（4.4 先声明更浅 ⇒ it -0x10 / itEnd -0x14 = ORIG
        //   0x0865e9c1/0x0865e9d6 槽址；前轮试的是 endIt 先声明 = 方向相反故反劣）。
        //   本函数容器在循环内只读（无 erase）⇒ 缓存 end() 语义安全。
        std::vector<ContinuousItemInfo>::const_iterator it = cool->begin();
        std::vector<ContinuousItemInfo>::const_iterator itEnd = cool->end();
        for (; it != itEnd; ++it)
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
}