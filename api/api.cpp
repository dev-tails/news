#include <regex>
#include "crow.h"
#include <sqlite3.h>

static int callback(void *data, int argc, char **argv, char **azColName)
{
  auto articles = (std::vector<crow::json::wvalue> *)data;

  crow::json::wvalue article;

  for (int i = 0; i < argc; i++)
  {
    if (strcmp(azColName[i], "URL") == 0)
    {
      std::string url = argv[i];
      std::smatch base_match;

      const std::regex base_regex("^https?://([^/]*)/.*");
      if (std::regex_match(url, base_match, base_regex))
      {
        if (base_match.size() == 2)
        {
          std::ssub_match base_sub_match = base_match[1];
          std::string domain = base_sub_match.str();
          article["DOMAIN"] = domain;
        }
      }
      else
      {
        article["DOMAIN"] = argv[i];
      }
    }

    article[azColName[i]] = argv[i];
  }

  articles->push_back(std::move(article));

  return 0;
}

int main()
{
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")
  ([](const crow::request & /*req*/, crow::response &res)
   {
     sqlite3 *db;
     int rc = sqlite3_open("news.db", &db);

     if (rc)
     {
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
     }
     else
     {
       fprintf(stderr, "Opened database successfully\n");
     }

     std::vector<crow::json::wvalue> articles;

     const char *sql = "SELECT * from NEWS ORDER BY ID DESC LIMIT 30";

     char *zErrMsg = 0;
     int selectRc = sqlite3_exec(db, sql, callback, (void *)&articles, &zErrMsg);

     if (selectRc != SQLITE_OK)
     {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
     }
     else
     {
       fprintf(stdout, "Operation done successfully\n");
     }

     sqlite3_close(db);

     crow::mustache::context ctx;
     ctx["articles"] = std::move(articles);
     auto page = crow::mustache::load("index.html");
     res.write(page.render(ctx));
     res.end();
   });

  CROW_ROUTE(app, "/submit").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)
  ([](const crow::request & req, crow::response &res)
   {
     if (req.method == crow::HTTPMethod::POST) {
      sqlite3 *db;
      char *zErrMsg = 0;
      int rc = 0;
      rc = sqlite3_open("news.db", &db);

      if (rc)
      {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return res.end();
      }
      else
      {
        fprintf(stderr, "Opened database successfully\n");
      }

      crow::query_string qs("?" + req.body);
      std::string title = qs.get("title");
      std::string url = qs.get("url");

      std::string sql = "INSERT INTO NEWS (TITLE,URL) VALUES ('" + title + "','" + url + "')";

      rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
      if (rc != SQLITE_OK)
      {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
      }
      else
      {
        fprintf(stdout, "Records created successfully\n");
      }

      sqlite3_close(db);
      res.redirect("/");
      return res.end();
    }

    auto page = crow::mustache::load("submit.html");
    res.write(page.render());
    res.end();
   });

  app.port(18080).multithreaded().run();
}