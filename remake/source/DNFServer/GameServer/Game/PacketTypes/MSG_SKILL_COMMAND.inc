MSG_SKILL_COMMAND::MSG_SKILL_COMMAND()
{  // ORIG 0x8230b?? memset(this,0,4)：仅包头前 4 字节（整体清零语义，CQuickChattingOption::clear 同型）
    memset(this, 0, 4);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转(其致 this 先装,与 ORIG 常数先不符)
}