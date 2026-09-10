// [N1F 2026-09-11 §6.5 串盲区] 同 makeRequest.inc：ORIG prettiness 串为
//   `static void InterSelectCreateDnfUserInfo::makeRequest(int, memberIdentificationNumber_t, bool)`
//   （strall 审计实读；我方原写 `unsigned int`）。mangled `...Eijb` 不变。
typedef unsigned int memberIdentificationNumber_t;
void InterSelectCreateDnfUserInfo::makeRequest(int uid, memberIdentificationNumber_t memberId,
