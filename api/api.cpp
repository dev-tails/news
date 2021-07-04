#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        crow::mustache::context ctx;
        std::vector<crow::json::wvalue> articles(1);

        articles[0]["title"] = "Headline";
        articles[0]["href"] = "http://google.ca";
        articles[0]["url"] = "google.ca";

        ctx["articles"] = std::move(articles);
        auto page = crow::mustache::load("index.html");
        return page.render(ctx);
    });

    app.port(18080).multithreaded().run();
}