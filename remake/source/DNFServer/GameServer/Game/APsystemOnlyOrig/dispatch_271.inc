// ============================================================================
// DB_MailBoxSendNewMailWithDBWork::dispatch —— ORIG 0x084272de
// GetDBHandle(DB_HANDLE_3,0) → GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_
// DBWORK>。本地 SIG_MAILBOX_SEND_NEW_MAIL 0x168 清零后：strncpy(sender,
// sig+0x04,0x1d)；+0x22=sig+0x24、+0x1e=sig+0x28、+0x26=0、+0x57=sig+0x1a2；
// 标题长度 sig+0xa0 非 0 时 +0x59=长度、strncpy(+0x5b, sig+0xa2, 长度)。
// 附件：sig+0x00==0 → InsertAvatarItem(sig+0x2c) 失败 throw 0x6871、
// identity=GetIdentity(db)（0 → throw 0x6874）、+0x2b=sig+0x30、
// +0x34=sig+0x35、+0x2f=identity、+0x36=sig+0x54、+0x56=1；
// sig+0x00==1 → insertCreatureItem(db, sig+0x2c) 失败 throw 0x6883、
// identity（0 → throw 0x6886）、+0x2b=sig+0x34、+0x34=sig+0x40、
// +0x2f=identity、+0x58=1；其余无附件（+0x2a=0、+0x33=0 已清零）。
// insertMailData(db, buf) 失败 throw 0x6891；sendResult(slot,0,sig+0x24,0)
// 失败 throw 0x6894。成功返 1；catch(unsigned int) → 日志（省略）→ 返 1。
// （N 封顶：ndiff 171 残差为局部栈槽分配差异 + EH/catch 布局，见报告
// dispatch.md）
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084272de
bool DB_MailBoxSendNewMailWithDBWork::dispatch(int userIdx, int slot,
                                               Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    DB_MailBoxSendNewMailView* sig =
        reinterpret_cast<DB_MailBoxSendNewMailView*>(
            stream->GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    // [K5W8LTDBTask W8DiagD 旋钮 P161/P311] ORIG 0x0842735b/…67/…7a/…96 对 mailBuf
    // 字段是**单条 ebp 偏移 store**（`mov %eax,-0x172(%ebp)`、`mov %al,-0x13d(%ebp)`、
    // `mov %dx,-0x13b(%ebp)`），无取址；我方逐次
    // `reinterpret_cast<DB_MailSendDataView*>(mailBuf)->X` 每写多发 1 条
    // `lea -0x194(%ebp),%eax`（chunk4/5/11/17/21 合计 +11）⇒ 改用具名视图对象。
    // DB_MailSendDataView 位于 `#pragma pack(push,1)` 内、末字段 m_title[0x10d]@+0x5b
    // ⇒ sizeof 恰 0x168，槽大小与全部偏移不变。
    DB_MailSendDataView mailBuf;
    memset(&mailBuf, 0, sizeof(mailBuf));
    strncpy(mailBuf.m_sender, sig->m_sender, 0x1d);
    mailBuf.m_field22 = sig->m_field24;
    mailBuf.m_field1e = sig->m_field28;
    mailBuf.m_field26 = 0;
    mailBuf.m_field57 = sig->m_field1a2;
    // [P5S2W8LT r1] 补 && 短路首项：ORIG 0x0842738a `mov -0x18(%ebp),%eax;
    //   add $0xa2,%eax; test %eax,%eax; je +0x104`（数组名退化指针真值测试，
    //   恒真但 4.1 照发）→ 0x08427396 才测 m_titleLen；原缺首项 ⇒ 少 4 条。
    if (sig->m_title != 0 && sig->m_titleLen != 0)
    {
        mailBuf.m_field59 = sig->m_titleLen;
        strncpy(mailBuf.m_title, sig->m_title, sig->m_titleLen);
    }
    try
    {
        if (sig->m_type == 0)
        {
            if (!DB_AvatarItemInsert::InsertAvatarItem(
                    reinterpret_cast<SIG_AVATAR_INSERT*>(sig->m_attach)))
                throw (int)0x6871;
            unsigned int identity = WongWork::DBCommon::GetIdentity(db);
            if (identity == 0)
                throw (int)0x6874;
            // [P5S2W8LT r1] P212/P309 物化序：ORIG 0x08427470/0x08427552
            //   `movb $0x0,-0x16a`（=mailBuf+0x2a）在各附件分支**内部**首条，
            //   0x08427494/0x08427573 的 `movb $0x0,-0x161`（=+0x33）在
            //   m_field34 之后、m_identity 之前——原两行提到 try 之前共用
            //   ⇒ 少 2 条 store 且块序错。
            mailBuf.m_field2a = 0;
            mailBuf.m_field2b = sig->m_field30;
            mailBuf.m_field34 = (unsigned short)sig->m_field35;
            mailBuf.m_field33 = 0;
            mailBuf.m_identity = identity;
            mailBuf.m_field36 = sig->m_field54;
            mailBuf.m_field56 = 1;
        }
        else if (sig->m_type == 1)
        {
            if (!DB_InsertCreatureItem::insertCreatureItem(
                    db, reinterpret_cast<SIG_CREATURE_ITEM*>(sig->m_attach)))
                throw (int)0x6883;
            unsigned int identity = WongWork::DBCommon::GetIdentity(db);
            if (identity == 0)
                throw (int)0x6886;
            mailBuf.m_field2a = 0;   // ORIG 0x08427552
            mailBuf.m_field2b = sig->m_field34;
            mailBuf.m_field34 = sig->m_field40;
            mailBuf.m_field33 = 0;   // ORIG 0x08427573
            mailBuf.m_identity = identity;
            mailBuf.m_field58 = 1;
        }
        if (!DB_MailBox_Req_Send::insertMailData(
                db, reinterpret_cast<const SIG_MAILBOX_SEND_NEW_MAIL*>(&mailBuf)))
            throw (int)0x6891;
        if (!DB_MailBox_Req_Send::sendResult(slot, 0, sig->m_field24,
                                             (ENUM_SERVER_GROUP)0))
            throw (int)0x6894;   // ORIG 0x084275f7 `xor $0x1,%eax; test %al,%al; je`
                                 // vs 我方 `test %eax,%eax; sete %al`（ndiff 唯一残差）。
                                 // [Q5W8LT 2026-09-11 报告·未落] `xor $1` + `test %al` 表明
                                 // ORIG 的 sendResult 返回 **1 字节类型**（bool/char）；
                                 // 改我方**声明侧**（cpp:1103 `static int sendResult`）为 bool
                                 // 即可消此差，但**定义侧**在 DB_MailBox_Req_Send.cpp（非本
                                 // TU 写权）⇒ 声明/定义须同轮同步，留报告（禁单侧改致 ABI 不一致）。
    }
    catch (int e)
    {
        // ORIG 0x0842761c：cMyTrace(__PRETTY_FUNCTION__, 0x6899, 5) + trace("%d", e)。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6899, 5);
        trace("%d", e);
    }
    return 1;
}