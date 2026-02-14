#include <string>
#include <vector>
#include <functional>
#include <cctype>
#include <cstdio>
#include <SDL.h>
#include <SDL_ttf.h>
#include "tokenizer.h"
#include "css.h"

static int hex2(char a,char b){
  auto v=[](char x){
    x=std::tolower((unsigned char)x);
    return (x<='9') ? x-'0' : 10+x-'a';
  };
  return v(a)*16+v(b);
}

std::vector<Rule> parse_css(std::string css){

  std::vector<Rule> rs; 
  size_t i = 0;

  while((i = css.find('{',i)) != std::string::npos){
    size_t s0 = css.rfind('}',i); 
    s0 = (s0==std::string::npos) ? 0 : s0+1;
    std::string sel = css.substr(s0, i-s0); 

    size_t j = css.find('}', i); 
    std::string body = css.substr(i+1, j-i-1); 
    Rule r{sel, {}};
    for(size_t p=0;;) { 
      size_t q = body.find(';',p); 
      std::string d = body.substr(p, q-p); 

      if(d.find(':') != std::string::npos){
        auto k = d.find(':'); 
        auto key = d.substr(0,k), val = d.substr(k+1);

        if(key=="font-size"){ 
          r.st.hasS=true; 
          r.st.px=std::stoi(val); 
        }
        if(key=="color"){ 
          r.st.hasC = true; 
          r.st.c = { (Uint8)hex2(val[1],val[2]), (Uint8)hex2(val[3],val[4]), 
                      (Uint8)hex2(val[5],val[6]), 255 }; 
        }
      } 
      if(q==std::string::npos) {break;} 
      p = q+1;
    }
    rs.push_back(r); 
    i = j+1;
  } 
  return rs;
}

// add tiny css cascade
Style style_for(const std::vector<Rule>& rs, const std::string& tag){
  Style s;
  for (const auto& r : rs) {
    if (r.sel == "body") {
      if (r.st.hasC) s.c  = r.st.c;
      if (r.st.hasS) s.px = r.st.px;
    }
  }
  for (const auto& r : rs) {
    if (r.sel == tag) {
      if (r.st.hasC) s.c  = r.st.c;
      if (r.st.hasS) s.px = r.st.px;
    }
  }
  return s;
}