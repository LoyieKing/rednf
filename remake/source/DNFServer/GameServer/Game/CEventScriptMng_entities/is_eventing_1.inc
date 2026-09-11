// [b28d-solo-CEventScriptMng_entities] N 封顶（P75/N7 复合；残 2 行，35/35）：
//   P226 已翻内层物化岛（见下），4.4.6 后端已对齐 vfn→%edx。唯一残差 =
//   ORIG `mov -0xc(%ebp),%eax; movl $0x0,0x4(%esp)` vs 4.4 `movl $0x0,0x4(%esp);
//   mov -0xc(%ebp),%eax`——receiver 装载先于常量实参压栈（4.1 this-first /
//   4.4 arg-first，P75 实参装载时刻跨版本差）。18 形探针全败（/tmp/probe_isevt{,2}.cpp：
//   ->、(*e)、int()、0u、*&*e、两种 cast、三目、嵌套 if、register 局部、具名 int
//   实参、语句表达式实参等），源形不可择。P0 禁 register asm 绑定
//   （ORIG 0x0810e75c sz 0x0000006f）
bool CAttendance::is_eventing()
{
    // ORIG 0x810e75c：GetRepeatEvent(0x56) 判空后经 vptr+0x34 判 IsEventing(0)，
    // 结果与判空合成一个 bool 条件（双重归一化形态）。
    if (CEventEntity::is_eventing())
    {
        CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x56);
        // [b28d-solo-CEventScriptMng_entities] P226 扩展：ORIG 内层条件先物化到
        //   %eax（`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`）再判，属
        //   「零槽短路物化」——4.4 对裸 `if (evt!=0 && evt->IsEventing(0))` 恒
        //   jump-thread 成两条件共落 fail 出口（少 5 条）。用语句表达式包住整个
        //   条件（内不含具名变量）复现 ORIG 物化岛。
        if (({ evt != 0 && evt->IsEventing(0); }))
            return true;
    }
    return false;
}