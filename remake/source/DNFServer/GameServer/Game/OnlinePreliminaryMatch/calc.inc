void CEloRating::calc(float& ratingSelf, float& ratingOther,
                      ENUM_ELO_RATING_SYSTEM sys, int k)
{
    float ra = ratingSelf;
    float rb = ratingOther;
    float ea = (float)(1.0 / (1.0 + pow(10.0, (double)((rb - ra) / 400.0f))));
    float eb = (float)(1.0 / (1.0 + pow(10.0, (double)((ra - rb) / 400.0f))));
    float score = 0.0f;
    float lose  = 0.0f;

    // ORIG 0x0844dcf0：`mov 0x10(%ebp),%eax; cmp $1/je; cmp $2/je; test/jne`
    // 决策树（P17/P46/P208）⇒ 源形为 switch（if-else 链恒内存直比）。
    // case 按数值升序书写（P46：4.4 树根取 (lo+hi)/2=1，叶块按 case 序排放）
    // [b28d-solo-OnlinePreliminaryMatch] P305：ENUM_ELO_RATING_SYSTEM 仅定义
    // `_0=0`（SchoolMatchEnums.h），-fstrict-enums 判值域 {0} ⇒ 裸 switch(sys)
    // 静默删 case 1/2（实测 .o 仅剩 0.0f/0.5f 两叶，1.0f 块消失=真语义 bug，
    // ORIG 三叶全在）。改 `switch((int)sys)` 绕开值域裁剪（S18 规避法）。
    switch ((int)sys)
    {
    case 0:
        score = 0.0f;
        break;
    case 1:
        score = 0.5f;
        break;
    case 2:
        score = 1.0f;
        break;
    }

    lose = 1.0f - score;
    ratingSelf  = ra + (float)k * (score - ea);
    ratingOther = rb + (float)k * (lose - eb);
