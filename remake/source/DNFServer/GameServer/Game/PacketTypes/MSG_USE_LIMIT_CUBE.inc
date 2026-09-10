MSG_USE_LIMIT_CUBE::MSG_USE_LIMIT_CUBE()
{  // ORIG C1：memset(this,0,4)（同 MSG_SKILL_COMMAND）
    memset(this, 0, 4);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转(其致 this 先装,与 ORIG 常数先不符)
