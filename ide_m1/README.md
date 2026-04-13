# Como executar a IDE

## 1. Debian/Ubuntu
1) Instalar as depedências <br>
`sudo apt update`
`sudo apt install -y build-essential qt6-base-dev qt6-tools-dev-tools`

2) Compilar <br>
`cd ide_m1 && qmake6 ide_m1.pro && make -j"$(nproc)"`

3) Executar <br>
`./ide_m1`

## 2. Windows
1) Instalar o qt6 pelo MinGW ou MSVC

2) Compilar <br>
`C:\caminho\para\compilers\ide_m1 qmake ide_m1.pro m ingw32-make-j%NUMBER_OF_PROCESSORS%`

3) Executar <br>
`./ide_m1`

## 3. MacOS
1) Install dependencies
`brew install qt@6`

2) Put Qt tools in PATH (Apple Silicon) <br>
`export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"` <br>

*Obs:* Se estiver utilizando Mac com Intel usar:
`export PATH="/usr/local/opt/qt@6/bin:$PATH"`

3) Compilar <br>
`cd ide_m1 && qmake ide_m1.pro && make -j"$(sysctl -n hw.ncpu)"`

4) Executar <br>
`open ide_m1.app` ou `/ide_m1.app/Contents/MacOS/ide_m1`