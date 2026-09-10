bool CLevelRewardTableMgr::IsChallengingJob(char job)
{
    CLevelRewardTable* table = GetProperLevelRewardTableR();
    if (table == 0)
        return false;
    if (job > 0xa || job < 0)
        return false;
    int itemNo = table->GetRewardItemNo(job + 1, 0);
    int count = table->GetItemCount(job + 1, 0);
    // [b28c-L29] ORIG 0x08687792 尾部：两条 `cmpl $0; jne` 各自跳同一 mov$1
    // 尾块（false 走 fall-through）——P222 两形互试定稿：正向包裹
    // `if(itemNo==0 && count==0) return false; return true;` 复现（反形
    // `if(!=0) return true` 被 4.4 折叠成 je+mov$1 fall，实测 44→46 反劣）。
    if (itemNo == 0 && count == 0)
        return false;
    return true;
