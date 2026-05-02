# Como executar a IDE

## 1. Debian/Ubuntu
1) Instalar as depedências <br>
`sudo apt update`
`sudo apt install -y build-essential qt6-base-dev qt6-tools-dev-tools`

2) Compilar <br>
`cd ide && qmake6 ide.pro && make -j"$(nproc)"`

3) Executar <br>
`./ide`

## 2. Windows
1) Instalar o qt6 pelo MinGW ou MSVC

2) Compilar <br>
`C:\caminho\para\compilers\ide qmake ide.pro m ingw32-make-j%NUMBER_OF_PROCESSORS%`

3) Executar <br>
`./ide`

## 3. MacOS
1) Install dependencies
`brew install qt@6`

2) Put Qt tools in PATH (Apple Silicon) <br>
`export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"` <br>

*Obs:* Se estiver utilizando Mac com Intel usar:
`export PATH="/usr/local/opt/qt@6/bin:$PATH"`

3) Compilar <br>
`cd ide && qmake ide.pro && make -j"$(sysctl -n hw.ncpu)"`

4) Executar <br>
`open ide.app` ou `/ide.app/Contents/MacOS/ide`