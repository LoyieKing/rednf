// process_update_ingame_history（ORIG 0x0810c82e）
// updates/history 均强制 const map*（ORIG begin/end/find 全为 const 版本，
// _Rb_tree_const_iterator 直返无转换）。makeRequest 参数装载：右→左
// hit->second.second、hit->second.first、it->first（updates 迭代器 key）、
// get_acc_id —— 与 ORIG +0x0b8..+0x0ed 压栈一致（当前实现曾误用 hit->first
// 且 second.first/second.second 颠倒）。ORIG 的 ++it 位于外层 if 体内
// （second==false 时不递增——忠实转写 ORIG 控制流）。尾部不清 update 表。
// [b28d-L14 N82附] ORIG 0x0810f5b8；p167 CLEAR（18/18）；残差=槽深/寄存器分配。
// [b28d-solo-W8_EventClassifyB 判词·N7 封顶，残差 17] hend 内联后槽位全对齐
// （it -0x24 / hit -0x28 / 内层 end -0x1c / 循环条件 end -0x20，与 ORIG 逐槽一致）；
// 残差为 4 处 sret 地址装载形：
//   ORIG `lea -0xXX(%ebp),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)`
//   vs 4.4 `mov %eax,%edx; lea -0xXX(%ebp),%eax; mov %edx,0x4(%esp); mov %eax,(%esp)`
// —— 4.4 的 lea 目标恒 %eax（call 结果须先中转 %edx），4.1 恒 %edx；属 N7
// 寄存器角色族（P230 补分诊：N7 判词限定 4.4.7 后端）。「先声明后赋值」
// 形实测反劣（17→21，且引入默认 ctor 风险）。语义/调用点全等。
// [b28d-W8EventB 复核] jcc 归一化交叉验证：6 条 jcc 助记符与目标偏移序列
// 逐位一致（仅整体 +8 常数位移，源于 4.4 sret 装载多 4 条）；p167 18/18 CLEAR。
void CEventScriptMng::process_update_ingame_history(CUser* user)
{
    if (user == 0)
        return;

    std::map<int, bool>::const_iterator it =
        static_cast<const std::map<int, bool>*>(
            user->get_ingame_event_history_update())->begin();

    while (it != static_cast<const std::map<int, bool>*>(
                      user->get_ingame_event_history_update())->end())
    {

        if (it->second)
        {
            std::map<int, std::pair<int, int> >::const_iterator hit =
                static_cast<const std::map<int, std::pair<int, int> >*>(
                    user->get_ingame_event_history())
                    ->find(it->first);
            if (hit !=
                static_cast<const std::map<int, std::pair<int, int> >*>(
                    user->get_ingame_event_history())->end())
            {
                DB_UpdateInGameEventHistory::makeRequest(  // UNLINKED_CALLEE
                    user->get_acc_id(), it->first,
                    hit->second.first, hit->second.second);
            }
            ++it;
        }
    }
}