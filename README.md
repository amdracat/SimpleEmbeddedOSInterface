# SimpleEmbeddedOSInterface

ビルド方法
make

実行方法(Windows)
./main.exe


動作モードの変更
SimpleEmbeddedOSInterface\main.c
#define OS_MANUAL
初期状態はMANUALモード


//#define OS_MANUAL
コメントアウトすることでASNCモード

ASNCモードで有効なコマンド
停止指示
motor 0

低速指示
motor 1

高速指示
motor 2

温度設定（0℃）
temp 0

温度設定（-10℃）
temp -10

