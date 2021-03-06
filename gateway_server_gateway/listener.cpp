﻿#include "listener.h"
#include "server.h"
#include "cpeer.h"
#include "speer.h"
#include "config.h"

Server &Listener::GetServer() {
    // 拿到服务上下文
    return *(Server *) &*ec;
}

void Listener::Accept(std::shared_ptr<CPeer> const &cp) {
    // 没连上
    if (!cp) return;

    // 加持
    cp->Hold();

    // 引用到 server 备用
    auto &&s = GetServer();

    // 检查是否已经与 0 号服务( server_base )建立了连接. 如果没有，则直接退出( cp 无加持会直接断开 )
    auto&& s0 = s.dps[0].second;
    if (!s0) return;

    // 填充自增id
    cp->clientId = ++s.cpeerAutoId;

    // 放入容器
    s.cps[cp->clientId] = cp;

    // 设置自动断线超时时间
    cp->SetTimeoutSeconds(config.clientTimeoutSeconds);

    // 向默认服务发送 accept 通知
    s0->SendCommand("accept", cp->clientId, EP::AddressToString(cp->addr));
}
