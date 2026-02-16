#include "js.h"
#include <quickjs/quickjs.h>
#include <SDL.h>
#include <string>
#include <vector>

static JSValue js_alert(JSContext* ctx, JSValueConst, int argc, JSValueConst* argv){
    const char* s = JS_ToCString(ctx, argv[0]);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "alert", s ? s : "", nullptr);
    if(s) JS_FreeCString(ctx, s);
    return JS_UNDEFINED;
}

// JS method implementations
void JS::init(){
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);
    JSValue g = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, g, "alert", JS_NewCFunction(ctx, js_alert, "alert", 1));
    JS_SetPropertyStr(ctx, g, "count", JS_NewInt32(ctx, 0));
    JS_FreeValue(ctx, g);
}
void JS::exec(const std::string& code){
    JSValue v = JS_Eval(ctx, code.c_str(), code.size(), "<onclick>", JS_EVAL_TYPE_GLOBAL); 
    JS_FreeValue(ctx, v);
}
int JS::count(){
    JSValue g = JS_GetGlobalObject(ctx);
    JSValue v = JS_GetPropertyStr(ctx, g, "count");
    int32_t out = 0; 
    JS_ToInt32(ctx, &out, v);
    JS_FreeValue(ctx, v); 
    JS_FreeValue(ctx, g);
    return (int)out;
}
JS::~JS(){ if(ctx) JS_FreeContext(ctx); if(rt) JS_FreeRuntime(rt); }


// extract ... of onclick="..." 
std::string extract_onclick(const std::string& tag){
    auto p = tag.find("onclick=\""); 
    if(p==std::string::npos) return "";
    p += 9; 
    auto q = tag.find('"', p); 
    if(q==std::string::npos) return "";
    return tag.substr(p, q-p); //(e.g. "alert('hi'); count += 1" )
}

void dispatch_click(JS& js, const DisplayList& dl, int mx, int my){
    for(const auto& cmd: dl){
        if(cmd.onclick.empty()) continue;
        const SDL_Rect& b = cmd.box; // get render area {x0, y, w, h} where onclick string exist. 
        // check hit point (mx, my) in b × b rect area
        if(mx>=b.x && mx<b.x+b.w && my>=b.y && my<b.y+b.h){ 
            js.exec(cmd.onclick); 
            break; 
        }
    }
}
