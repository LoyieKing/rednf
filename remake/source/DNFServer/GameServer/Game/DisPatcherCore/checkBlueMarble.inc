// ORIG T 0x081c9172 _ZN24DisPatcher_SelectDungeon15checkBlueMarbleEP6CParty
// [b28d-29] AE：38=38 全等（同上重定位归一）  (ORIG 0x081c9172 insn 38->39)
// （checkBlueMarble.md 全文转写）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 38 / ours 38 insn 全等（AE 口径，重定位归一）。
// [R21bool/T1] 返回类型 int→bool（声明侧 DisPatcherCore.h 同步）。
bool DisPatcher_SelectDungeon::checkBlueMarble(CParty* party)

{
    if (party == 0)
    {
        return 0;
    }
    // ORIG +0x13：getManager()==0 → test/sete/test/je 物化（bool 判据，无
    // 具名槽）；IsSinglePlay(bool) 裸 test/je + `(... == 1) == true` 双层
    // 分支物化（c6root 探针 h14 逐条一致）。
    if (party->getManager() == 0)
    {
        return 0;
    }
    // [b28e-L5] 判据① 唯一残差：ORIG 0x081c….+0x5c 处 `je <0x5c>`（IsSinglePlay
    // false）与 `jne <0x5c>`（getMoveSpace!=1）**两跳同落** packed-false 岛，随后
    // `0x5c: mov $0,%eax; 0x61: test %al,%al; je <0x6c>; mov $1,%eax; jmp; 0x6c:
    // mov $0,%eax` 为 4.1 的 bool→int 归一岛；我方该 je 直落 0x6c（4.4 直跳折叠）。
    // 已试三目形（`return cond ? 1 : 0;`）实测 **ae -1 回退**（4.4 折成单层直跳，
    // 丢 getManager 二次调用路径）⇒ 恢复原双层 if 形，本处记 N（4.1 归一岛不可造）。
    if (party->IsSinglePlay())
    {
        if ((party->getManager()->getMoveSpace() == 1) == true)
        {
            return 1;
        }
    }
    return 0;
}
