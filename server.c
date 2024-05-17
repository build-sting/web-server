#include "mongoose.h"

static struct mg_http_serve_opts opts;
const char *s_http_port = "8000";

// Connection event handler function
static void fn(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {                                                                   // New HTTP request received
        struct mg_http_message *hm = (struct mg_http_message *)ev_data; // Parsed HTTP request
        if (mg_match(hm->uri, mg_str("/api/hello"), NULL))
        {                                                                // REST API call?
            mg_http_reply(c, 200, "", "{%m:%d}\n", MG_ESC("status"), 1); // Yes. Respond JSON
        }
        else
        {
            // For all other URLs,
            mg_http_serve_dir(c, hm, &opts); // Serve static files
        }
    }
}

int main(int argc, char const *argv[])
{
    struct mg_mgr mgr; // Mongoose event manager. Holds all connections

    opts.root_dir = ".";

    int i;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-r") == 0)
        {
            opts.root_dir = argv[++i];
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0)
        {
            s_http_port = argv[++i];
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("\n  $ ./serve -r [dir] -p [port]\n\n");
            return 0;
        }
    }

    mg_mgr_init(&mgr); // Initialise event manager

    char url[50];

    sprintf(url, "%s%s", "http://0.0.0.0:", s_http_port);

    mg_http_listen(&mgr, url, fn, NULL); // Setup listener
    for (;;)
    {
        mg_mgr_poll(&mgr, 1000); // Infinite event loop
    }
    return 0;
}