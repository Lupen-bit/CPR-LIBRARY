#include "httpsServer.hpp"
#include <system_error>

namespace cpr {
HttpsServer::HttpsServer(std::string&& baseDirPath, std::string&& sslCertFileName, std::string&& sslKeyFileName) : baseDirPath_{baseDirPath}, sslCertFileName_(std::move(sslCertFileName)), sslKeyFileName_(std::move(sslKeyFileName)) {
    // See https://mongoose.ws/tutorials/tls/
    memset(static_cast<void*>(&tlsOpts_), 0, sizeof(tlsOpts_));
    tlsOpts_.cert = sslCertFileName_.c_str();
    tlsOpts_.certkey = sslKeyFileName_.c_str();
}

std::string HttpsServer::GetBaseUrl() {
    return "https://127.0.0.1:" + std::to_string(GetPort());
}

uint16_t HttpsServer::GetPort() {
    // Unassigned port in the ephemeral range
    return 61937;
}

mg_connection* HttpsServer::initServer(mg_mgr* mgr, mg_event_handler_t event_handler) {
    mg_mgr_init(mgr);

    std::string port = std::to_string(GetPort());
    mg_connection* c = mg_http_listen(mgr, GetBaseUrl().c_str(), event_handler, this);
    return c;
}

void HttpsServer::acceptConnection(mg_connection* conn) {
    // See https://mongoose.ws/tutorials/tls/
    mg_tls_init(conn, &tlsOpts_);
}

void HttpsServer::OnRequest(mg_connection* conn, mg_http_message* msg) {
    std::string uri = std::string(msg->uri.ptr, msg->uri.len);
    if (uri == "/hello.html") {
        OnRequestHello(conn, msg);
    } else {
        OnRequestNotFound(conn, msg);
    }
}

void HttpsServer::OnRequestNotFound(mg_connection* conn, mg_http_message* /*msg*/) {
    mg_http_reply(conn, 404, nullptr, "Not Found");
}

void HttpsServer::OnRequestHello(mg_connection* conn, mg_http_message* /*msg*/) {
    std::string response{"Hello world!"};
    std::string headers{"Content-Type: text/html\r\n"};
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

const std::string& HttpsServer::getBaseDirPath() const {
    return baseDirPath_;
}

const std::string& HttpsServer::getSslCertFileName() const {
    return sslCertFileName_;
}

const std::string& HttpsServer::getSslKeyFileName() const {
    return sslKeyFileName_;
}

} // namespace cpr
