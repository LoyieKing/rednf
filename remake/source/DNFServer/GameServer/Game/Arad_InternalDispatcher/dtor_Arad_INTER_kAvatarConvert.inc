Arad_INTER_kAvatarConvert::~Arad_INTER_kAvatarConvert() {}

// ============================================================================
// cmd 流业务派发器 checkWorkState —— ORIG 各 W 函数逐条转写。
//   依赖：GlobalData::s_pSecuProtectionField、ARAD::Singleton<T>::Get、
//         各 Server::CheckCondition / AvatarRechargeServer::
//         CheckConditionUseRecharge（定义见各 Server 权威 TU）。
// ============================================================================
