# Homework 2

學號：41343146

本資料夾可作為 GitHub 專案的根目錄，結構為 `.github/`、`homework2/src/` 及 `homework2/report.md`。若放進既有專案，請將這兩個目錄放在專案根目錄。

## 檔案

- `homework2/src/main.cpp`：主程式。
- `homework2/src/polynomial.h`：多項式類別，依提供程式碼修正。
- `homework2/src/tests.cpp`：固定與隨機測試。
- `homework2/report.md`：包含完整程式碼的作業報告。
- `homework2/sample-input.txt`、`sample-output.txt`：示範資料。
- `homework2/measure.ps1`、`measurements.csv`：完整程式量測工具與實測結果。
- `.github/`：官方 Homework-template 的格式檢查 workflow 與 Linux validator。

## 編譯

在 homework2 目錄下：

```shell
g++ -std=c++11 -O2 -Wall -Wextra -pedantic src/main.cpp -o polynomial
g++ -std=c++11 -O2 -Wall -Wextra -pedantic src/tests.cpp -o tests
./tests
./polynomial < sample-input.txt
```

Windows 的執行檔可使用 `polynomial.exe` 與 `tests.exe`。使用 MSVC 時，在 Developer Command Prompt 內：

```shell
cl /EHsc /O2 /W4 /std:c++14 /utf-8 src/main.cpp /Fe:polynomial.exe
cl /EHsc /O2 /W4 /std:c++14 /utf-8 src/tests.cpp /Fe:tests.exe
tests.exe
polynomial.exe < sample-input.txt
```

本次實際使用 MSVC，1,518 項檢查通過。未在本機執行官方 Linux validator，也尚未上傳 GitHub；需在上傳後確認 Actions 結果，再將專案連結交到 uLearn。

官方檢查檔來源：
https://github.com/NFU-OpenDataStructure/Homework-template

validator_linux 的 SHA256：
3B26F85710A6D2C9B5164A75908EA8455F22C9CEFAFA9BC8A969F2EFBB31EB08
