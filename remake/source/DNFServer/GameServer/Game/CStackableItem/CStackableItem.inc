// 最小构造/析构（ORIG 逐成员构造属后续精修批次；保证可链接）
// b28c-L8 判词 [N(布局建模)]：fresh 复验 _ZN14CStackableItemC1Ev 292=292，唯一残差
// 4 行 = 本类 vptr 写入时机。ORIG 0x08515763 构造序 = `call _ZN5CItemC1Ev`(0x85152d4)
// → `add $0x148; call _ZN12AradItemInfoC1Ev`(0x8515618) → **然后**才
// `movl $0x8c96ea8,(%eax)` 写本类 vptr → RecipeInfo(+0x174) / LotteryInfo(+0x1c0) / …
// ⇒ AradItemInfo 必为**次基类**而非成员（CItem sizeof=0x148 权威 ⇒ 次基类恰落 +0x148，
// 全部偏移/sizeof 不变；AradItemInfo 无虚函数 ⇒ 不引入第二 vptr）。成员模型下 4.4
// 恒在首成员 ctor 之前写 vptr，源码不可翻。同法先例：C28L9 AvatarRechargeServer.h、
// C28L16 AvatarFixedHiddenOptionServer.h。本波不改 CStackableItem.h（跨
// C28L17/18/21 三方写权 CItemList.cpp + Arad_ItemBonus.cpp 消费面），spec 已广播交
// Main 统一批次；改动清单=头继承表 + CStackableItem.cpp 12 处 + Arad_ItemBonus.cpp:431
// + CItemList.cpp:1986 去 `m_arad148.` 前缀。
// [b28d-N(布局建模)｜待专项批次] fresh 复验 292=292 维持 N：唯一残差 = 本类 vptr
// 写入时机（AradItemInfo 须为次基类，跨 CItemList.cpp/Arad_ItemBonus.cpp 消费面）。
// Main 2026-09-09 裁决：**本轮不做**（收益仅本件 4 行残差，代价为全域构造/析构语义
// 变更 + 14 处消费点 + 多基类 RTTI/EH 风险），spec 存档待专项批次：
//   头改 `class CStackableItem : public CItem, public AradItemInfo`，删 m_arad148 成员，
//   去 CStackableItem.cpp 12 处 `m_arad148.` 前缀 + CItemList.cpp:1986 + Arad_ItemBonus.cpp:431；
//   CItem sizeof=0x148 权威 ⇒ 全部偏移/sizeof 不变、AradItemInfo 无虚函数 ⇒ 不引入第二 vptr。
// [S1Charac 2026-09-11 复核] 292=292 维持 N（残差 4 行=vptr 写入时机，属 AradItemInfo
// 次基类布局建模，Main 已裁决本轮不做）；§6.5：本函数无 rodata 串引用 ⇒ CLEAR。
CStackableItem::CStackableItem()
{
}