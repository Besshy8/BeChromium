#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "tokenizer.h"
#include "layout.h"

DisplayList dl;

// URLからhost, port, pathを取得
static void split_url(const std::string& url,
                      std::string& host, std::string& port, std::string& path) {
    const std::string pfx = "http://";
    if (!url.starts_with("http://")) { 
        throw std::runtime_error("only http:// supported");
    }
    // hostとport部を取得
    auto rest = url.substr(pfx.size());
    auto slash = rest.find('/');
    auto hostport = rest.substr(0, slash);
    auto colon = hostport.find(':');
    if (colon == std::string::npos) { 
        host = hostport; 
        port = "80"; 
    }
    else { 
        host = hostport.substr(0, colon); 
        port = hostport.substr(colon + 1); 
    }
    // path部を取得
    path = (slash == std::string::npos) ? "/" : rest.substr(slash);
}

// get body element via HTTP/1.1
static std::string http11_get(const std::string& url) {
    std::string host, port, path;
    split_url(url, host, port, path);

    addrinfo hints{}; 
    hints.ai_socktype = SOCK_STREAM;  // TCP/UDP
    hints.ai_family = AF_UNSPEC; // IPv4/IPv6
    addrinfo* res = nullptr;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
        throw std::runtime_error("getaddrinfo failed");

    int fd = -1;    
    // use socket api and create connection to server (3-way handshake)
    for (auto r = res; r; r = r->ai_next) {
        fd = ::socket(r->ai_family, r->ai_socktype, r->ai_protocol);
        if (fd == -1) continue;
        if (::connect(fd, r->ai_addr, r->ai_addrlen) == 0) break;
        ::close(fd); fd = -1;
    }
    freeaddrinfo(res);
    if (fd == -1) {
        throw std::runtime_error("connect failed");
    }

    // RFC 9112, HTTP/1.1 GET request
    std::string req =
        "GET " + path + " HTTP/1.1\r\n"
        "Host: " + host + "\r\n"
        "Connection: close\r\n"
        "User-Agent: BeChromium/0.1\r\n"
        "Accept: */*\r\n\r\n";
    ::send(fd, req.data(), req.size(), 0);

    std::string resbuf;
    char buf[4096];
    ssize_t n;
    while ((n = ::recv(fd, buf, sizeof(buf), 0)) > 0) {
        resbuf.append(buf, n);
    }
    ::close(fd);

    auto p = resbuf.find("\r\n\r\n");
    // compatibility with HTTP/0.9
    return (p == std::string::npos) ? resbuf : resbuf.substr(p + 4); 
}

static std::string extract_title(const std::string body) {
    auto start = body.find("<title>");
    auto end = body.find("</title>");
    if (start == std::string::npos || end == std::string::npos)
        return "No Title";
    // std::cout << "start: " << std::type_info(start) << "\n"; // compile error
    start += 7; 
    return body.substr(start, end - start);
}

// drawing Display list 
static void draw_display_list(SDL_Renderer* ren, TTF_Font* font, const DisplayList& dl){
    SDL_Color col{0,0,0,255};
    for(const auto& cmd : dl){
        SDL_Surface* s = TTF_RenderText_Blended(font, cmd.text.c_str(), col); 
        if(!s) continue;
        SDL_Texture* t = SDL_CreateTextureFromSurface(ren, s);
        SDL_Rect dst{cmd.x, cmd.y, s->w, s->h};
        SDL_FreeSurface(s);
        if(t){
            SDL_RenderCopy(ren, t, nullptr, &dst);
            SDL_DestroyTexture(t);
        }
    }
}


// html tokenizer is implemented in tokenizer.cpp

int main(int argc, char** argv) {
    std::string url = (argc >= 2) ? argv[1] : "http://example.com/";

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return 1;
    }

    // set font (day4)
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("/Library/Fonts/Arial Unicode.ttf", 18); // 手元のttfに合わせて
    if (!font) { 
        std::cerr << TTF_GetError() << "\n"; 
        return 1; 
    }

    SDL_Window* win = SDL_CreateWindow(
        "BeChromium (step1: SDL + HTTP/1.1)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);

    std::cout << "Press R to GET: " << url << "\nEsc or close window to quit.\n";

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (e.key.keysym.sym == SDLK_r) {
                    try {
                        //auto body = http11_get(url);
                        std::string title = "Test html";
                        auto body = "<body> <div> <p>Hello world!</p> <p>Second paragraph.</p> </div> </body>";
                        SDL_SetWindowTitle(win, title.c_str());
                        // tokenize html body
                        std::vector<Token> tokens = tokenize_html(body);
                        DomTree* dom = make_dom_tree(tokens);

                        int w=0, h=0; 
                        SDL_GetWindowSize(win, &w, &h);
                        dl = build_display_list(dom, font, w);

                    } catch (const std::exception& ex) {
                        std::cerr << "Error: " << ex.what() << "\n";
                        SDL_SetWindowTitle(win, "BeChromium - error");
                    }
                }
            }
        }
        SDL_SetRenderDrawColor(ren, 250, 250, 250, 255);
        SDL_RenderClear(ren);
        draw_display_list(ren, font, dl); 
        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}