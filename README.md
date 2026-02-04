# BeChromium

---

## 要件

- macOS (Apple Silicon, arm64)
- Xcode Command Line Tools（未導入なら `xcode-select --install`）
- Homebrew
- 依存ライブラリ：SDL2, libcurl, pkg-config  
  （将来の拡張で CMake/Ninja を使う場合はそれらも入れておくと便利）

```bash
brew update
brew install sdl2 curl pkg-config
# （CMake ビルドも使うなら）brew install cmake ninja
```

```bash
# ビルド
clang++ -std=c++20 -O2 main.cpp -o be_chromium $(pkg-config --cflags --libs sdl2 libcurl)

## fish
set -l CFLAGS (pkg-config --cflags sdl2 libcurl | string split " ")
set -l LIBS   (pkg-config --libs   sdl2 libcurl | string split " ")
clang++ -std=c++20 -O2 $CFLAGS main.cpp -o be_chromium $LIBS

# 実行（既定 URL は https://example.com）
./be_chromium

# 任意 URL
./be_chromium https://www.chromium.org/
```

起動すると白いウィンドウが開きます（まだ描画していないため）。
標準出力には取得した HTML の先頭が表示されます。

- `R` … 同じ URL を再取得（Reload）
- `Esc` … 終了

---

## （任意）CMake プロジェクトでのビルド

複数ファイルへ拡張したい場合に。下のような `CMakeLists.txt` を置けば OK です。

```cmake
cmake_minimum_required(VERSION 3.22)
project(be_chromium LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
find_package(CURL REQUIRED)

add_executable(be_chromium
  src/main.cpp
  # src/net/http_client.cpp など、ファイルを増やしていく
)

target_include_directories(be_chromium PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(be_chromium PRIVATE ${SDL2_LIBRARIES} CURL::libcurl)
```

ビルド:

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
ninja
./be_chromium
```