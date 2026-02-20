#pragma once

#include "layout.h"
#include <SDL.h>
#include <quickjs.h>
#include <string>

struct JS {
  JSRuntime *rt = nullptr;
  JSContext *ctx = nullptr;
  void init();
  void exec(const std::string &code);
  int count();
  ~JS();
};

// extract ... of onclick="..."
std::string extract_onclick(const std::string &tag);

// dispatch click to run JS code from DisplayList
void dispatch_click(JS &js, const DisplayList &dl, int mx, int my);
