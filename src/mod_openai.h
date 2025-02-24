#include "ScriptMgr.h"
#include "Config.h"
#include "Player.h"
#include "Chat.h"
#include "Log.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>
#include <sstream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace json = boost::json;

using namespace Acore::ChatCommands;

class Phi2ModelCommandScript : public CommandScript
{
public:
    Phi2ModelCommandScript() : CommandScript("Phi2ModelCommandScript") { }

    ChatCommandTable GetCommands() const override;

private:
    static bool HandleAskPhi2Model(ChatHandler* handler, const char* args);
    static std::string MakePhi2ModelRequest(const std::string& prompt);
};

void AddSC_Phi2ModelommandScript()
{
    new Phi2ModelCommandScript();
}
