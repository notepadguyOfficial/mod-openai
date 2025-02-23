#include "mod_openai.h"

ChatCommandTable OpenAICommandScript::GetCommands()
{
    static ChatCommandTable child =
    {
        { "ask", HandleAskAICommand, SEC_PLAYER, Console::Yes }
    };

    static ChatCommandTable parent =
    {
        { "openai", child }
    };

    return parent;
}

static bool OpenAICommandScript::HandleAskAICommand(ChatHandler* handler, const char* args)
{
    if (sConfigMgr->GetOption<bool>("OpenAI.Enable", false))
        return false;

    if (!*args)
        return false;

    Player* player = handler->GetPlayer();
    std::string playerName = player ? player->GetName() : "Unknown";

    std::string prompt = "Provide a guide or answer for World of Warcraft WotLK 3.3.5a: ";
    prompt += args;

    std::string response = MakeOpenAIRequest(prompt);

    json::value jsonResponse = json::parse(response);
    if (jsonResponse.as_object().contains("choices"))
    {
        std::string aiResponse = jsonResponse.as_object()["choices"].as_array()[0].as_object()["message"].as_object()["content"].as_string().c_str();
        handler->PSendSysMessage("AI Response: %s", aiResponse.c_str());
        LOG_INFO("server.worldserver", "[OpenAICommandScript] %s asked AI: %s", playerName.c_str(), args);
        LOG_INFO("server.worldserver", "[OpenAICommandScript] AI Response: %s", aiResponse.c_str());
    }
    else
    {
        handler->PSendSysMessage("Failed to get a valid response from OpenAI.");
        LOG_ERROR("server.worldserver", "[OpenAICommandScript] Failed to get a valid response from OpenAI.");
    }

    return true;
}

std::string OpenAICommandScript::MakeOpenAIRequest(const std::string& prompt)
{
    try {
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve("api.openai.com", "https");
        stream.connect(results);

        std::ostringstream requestBody;
        requestBody << "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"" << prompt << "\"}]}";

        http::request<http::string_body> req{http::verb::post, "/v1/chat/completions", 11};
        req.set(http::field::host, "api.openai.com");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.set("Authorization", "Bearer " + OPENAI_API_KEY);
        req.body() = requestBody.str();
        req.prepare_payload();

        http::write(stream, req);
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);
        return res.body();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("server.worldserver", "[OpenAICommandScript] Error making OpenAI request: %s", e.what());
    }
}
