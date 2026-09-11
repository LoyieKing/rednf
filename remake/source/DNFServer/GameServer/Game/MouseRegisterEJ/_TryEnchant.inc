// ORIG T 0x0849dcc4（_TryEnchant.md / objdump 全文核对）。分派三种试附魔路径：
//   type==0 自家材料：_TryEnchantMine 后触发基类虚槽1（vptr+0x04）(user,0)；
//   type==1 对家材料：_TryEnchantOther，失败时复位材料②(this+0x18)并清登记配方(+0x40)；
//   type==2 仅虚槽(user,2)；其它类型无副作用。
// 虚槽经 vptr 偏移 +0x04 显式调用：与 ORIG 指令逐条对应；CExpertJob 虚表槽序
// 审计收敛后可替换回语义虚调用。
// [b28d-47 / b28c-R26 复验] verdict=NEAR(routed) [N7] ORIG 0x0849dcc4 orig/ours=64/64
//   本轮改动（DIFF→NEAR，两处）：
//   ① if/else-if 链 → switch(type)：ORIG 0x0849dcd1 先 `mov 0x14(%ebp),%eax` 再
//      寄存器比较链（cmp $1/je、cmp $2/je、test/jne），三叶块物理序 = case 书写序
//      0→1→2（ORIG 0x0849dcea case0 直落 / +0x65 case1 / +0xa6 case2）⇒ P251/P286/P324；
//   ② case1 内 `if (ret != 0)` → `while (ret != 0) { …; break; }`：复现 ORIG
//      0x0849dd45 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`（P225）。
//   残差 = N7（4.4.7 间接 call 目标恒 %eax；ORIG 0x0849dd0e `mov (%eax),%edx` +
//   `call *%edx`，我方 `mov (%eax),%eax` + `call *%eax`），属寄存器角色跨版本差。
int CEnchanter::_TryEnchant(CUser* user, stItemPos_t* pos,
