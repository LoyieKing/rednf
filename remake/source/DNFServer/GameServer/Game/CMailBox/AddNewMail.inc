// ORIG 0x08551f5a (T)——docs/class_func_reports/WongWork__CMailBox/AddNewMail.md
// stAddNewMailInput 布局（AddNewMail 装载证据，与 CMailBoxHelper 写入一致）：
//   +0x00 mailIndex   +0x04 senderCharacNo  +0x08 packageMoney
//   +0x0c letterId    +0x10 remainSec       +0x14 packageType
//   +0x18 period      +0x1c unlimit(byte)   +0x20 const char* senderName
//   +0x24 Inven_Item*（可空）
CMailBox::CMail* CMailBox::AddNewMail(const stAddNewMailInput& inputRaw)
{
    CMail* mail = m_mailPool.Acquire();
    if (mail == 0)
        return 0;

    // 具名成员直访（ORIG 0x08551f86/0x08551f90/0x08551f9c 均为单条
    // `mov K(%edx),%edx` disp 折叠；SIG_OFF 的 char*+offset 形会多发 `add $K`）。
    mail->m_mailIndex      = inputRaw.m_mailIndex;
    mail->m_senderCharacNo = inputRaw.m_senderCharacNo;
    mail->m_package.m_packageMoney   = inputRaw.m_packageMoney;

    // ORIG：srcItem 分支逐字段展开拷贝；空时构造 Inven_Item() 后逐字段搬移。
    // ORIG 0x08551fa8/0x08551fb2 对该指针两次独立装载（`mov 0xc(%ebp),%eax;
    // mov 0x24(%eax),%eax` + `test %eax,%eax` 无栈槽）⇒ 表达式直读两遍（P0）。
    // [b28d 二轮 L6S6] ORIG 0x08551fb0 `je 0855201a<+0xc0>`：**then=非空分支**
    //   （拷贝体直落 0x58..0xbe，空分支后置 0xc0）⇒ 必须写正极性 `!= 0`
    //   （P59/P222；负极性 4.4 恒 `jne` 逆序）。
    if (inputRaw.m_packageItem != 0)
        mail->m_package.m_item = *inputRaw.m_packageItem;
    else
        mail->m_package.m_item = Inven_Item();

    mail->m_period = inputRaw.m_period;

    std::strncpy(mail->m_senderName, inputRaw.m_senderName, 0x1d);

    mail->m_packageType = inputRaw.m_packageType;
    // P292 全链 bool：ORIG 0x085520c4 `movzbl 0x1c(%edx),%edx; mov %dl,0x74(%eax)`
    // 单条直存（无 uchar→bool 归一化）⇒ 源类型同为 bool。
    mail->m_unlimit     = inputRaw.m_unlimit;
    mail->m_letterId    = inputRaw.m_letterId;

    // ORIG 0x085520d7 首实参装载 = `mov 0xc(%ebp),%edx`（&input.m_mailIndex 即
    // input 自身）⇒ 源用 inputRaw.m_mailIndex 而非 mail->m_mailIndex（后者多 1 条）。
    m_mailMap.insert(std::make_pair(inputRaw.m_mailIndex, mail));

    mail->m_remainSec = inputRaw.m_remainSec;
    return mail;
}