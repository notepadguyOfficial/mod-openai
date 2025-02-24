#include "mod_openai.h"

ChatCommandTable Phi2ModelCommandScript::GetCommands() const
{
    static ChatCommandTable child =
    {
        { "ask", HandleAskPhi2Model, SEC_PLAYER, Console::Yes }
    };

    static ChatCommandTable parent =
    {
        { "ai", child }
    };

    return parent;
}

bool Phi2ModelCommandScript::HandleAskPhi2Model(ChatHandler* handler, const char* args)
{
    if (sConfigMgr->GetOption<bool>("Phi2Model.Enable", false))
        return false;

    if (!*args)
        return false;

    Player* player = handler->GetPlayer();
    std::string playerName = player ? player->GetName() : "Unknown";
    std::string prompt = "Provide a guide or answer for World of Warcraft WotLK 3.3.5a: ";
    prompt += args;

    std::string response = MakePhi2ModelRequest(prompt);
    handler->PSendSysMessage("AI Response: %s", response.c_str());

    LOG_INFO("server.worldserver", "[OpenAICommandScript] Player: %s, Prompt: %s, Response: %s", playerName.c_str(), prompt.c_str(), response.c_str());

    return true;
}

std::string Phi2ModelCommandScript::MakePhi2ModelRequest(const std::string& prompt)
{
    try {
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve("127.0.0.1", "5000");
        stream.connect(results);

        std::ostringstream requestBody;
        requestBody << "{\"prompt\": \"" << prompt << "\"}";

        http::request<http::string_body> req{http::verb::post, "/ask", 11};
        req.set(http::field::host, "127.0.0.1");
        req.set(http::field::content_type, "application/json");
        req.body() = requestBody.str();
        req.prepare_payload();

        http::write(stream, req);
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);

        boost::json::value jsonResponse = boost::json::parse(res.body());
        if (jsonResponse.as_object().contains("response"))
        {
            return jsonResponse.as_object()["response"].as_string().c_str();
        }

        return "Failed to get a response from Phi-2.";
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("server.worldserver", "[OpenAICommandScript] Error making OpenAI request: %s", e.what());
    }
}
