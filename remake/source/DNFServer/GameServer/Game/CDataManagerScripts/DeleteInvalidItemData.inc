// ============================================================================
// DeleteInvalidItemScript::loadScript（ORIG 0x08ac1e60 T）
// tw_DeleteInvalidItem.etc：[delete item] 三元组 (idx, rewardIdx, count)
// 以 idx 为 key、{idx,rewardIdx,count} 为值 insert；[send mail title]/
// [send mail content] 分别 ScanStr 进 +0/+4 字符串，ScanStr 失败直接结束；
// ScanType 耗尽正常结束返回 1。
// ============================================================================
DeleteInvalidItemScript::DeleteInvalidItemData::DeleteInvalidItemData()
{
    // ORIG C1 0x08ac220c W：转调 init()（三 int 清零）。
    init();
}