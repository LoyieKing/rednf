// ORIG 0x084247e0：季度表名 = "%02d%d"(tm_year+1900 后两位, tm_mon/3+1)；
// 主 insert 失败回退 prod_sale_entry_073 固定表，均失败记日志。
// [b28d-4] DIFF：N18a/P211：4.4 多 {'movl': 2} 中转 mov（单段直落/同型化待验）；ORIG 084247e0
void DB_BillingRecord::InsertSaleEntry(SIG_BILLING_RECORD* sig, bool isNew)
