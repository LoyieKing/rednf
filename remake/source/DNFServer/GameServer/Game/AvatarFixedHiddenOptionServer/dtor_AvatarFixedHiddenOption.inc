// ORIG D1 0x088aed5e：clearScript 后逆序析构 6 个 vector（成员析构自动完成）。
ARAD::SCRIPT::AvatarFixedHiddenOption::~AvatarFixedHiddenOption()
{
    clearScript();
}