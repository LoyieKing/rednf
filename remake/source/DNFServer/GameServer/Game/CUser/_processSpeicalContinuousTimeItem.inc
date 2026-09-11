// ORIG _ZN5CUser33_processSpeicalContinuousTimeItemEj @ 0x08659b60 (T)
// 特殊连续时间道具：查 item→is_stackable→IsExpAffect→GetExpAffectRatio>0
// 时 SetExpAffectItemEffect(true, ratio)。
// b28d-lane5 N封顶: N85 x87比较lowering | ORIG 0x08659b60 | _ZN5CUser33_processSpeicalContinuousTimeItemEj
// b28d-PoolCUser P230③ -march=i486 手工路由实验（2026-09-09，Main 批准，读数并列）：
//   (a) 现路由（4.4.6 后端，无 march）：本件 ndiff **5**（fucompp/fnstsw/sahf vs
//       fucomip/fstp 段）；整 TU identical=703/ae=1508/near=4/diff=115（合计 2211）。
//   (b) +`-march=i486 -mtune=generic`：本件 **AE 归零**；整 TU 703/1509/4/114
//       （合计 2212，净 +1，near/diff 零回退）。
//   (c) 仅 `-march=i486`（不带 -mtune）：整 TU **崩** 674/142/2/1512——默认 tune 改
//       调度，**不可单加**；b 形（带 -mtune=generic）为唯一可用组合（同 CCreatureMgr
//       先例 416/422/437 行记法）。⇒ 待 Main 落路由表。
void CUser::_processSpeicalContinuousTimeItem(unsigned int itemIdx)
{
    CItem* pItem = G_CDataManager()->find_item((int)itemIdx);
    if (pItem == 0)
        return;
    // ORIG 0x08659b60：is_stackable/IsExpAffect 均正向 `test %al,%al; je 共享尾
    // 出口`（无 xor/jne 反形）⇒ 源码正向嵌套 if（P59 反向）；ratio 槽 -0xc
    // fstps+flds 往返；x87 fucompp/fnstsw/sahf 段属 N85 跨版本豁免。
    if (pItem->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)pItem;
        if (stack->IsExpAffect())
        {
            float ratio = stack->GetExpAffectRatio();
            if (ratio > 0.0f)
                SetExpAffectItemEffect(true, ratio);
        }
    }
}