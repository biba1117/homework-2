# 41343146

作業二：Polynomial 多項式物件

## 解題說明

### 問題描述

依 Homework 2 的 ADT 與私有資料成員，使用 C++ 實作多項式加法 Add、乘法 Mult、代入求值 Eval，並多載輸入 >> 與輸出 <<。係數使用 float，指數使用非負 int。每個 Term 儲存一組係數與指數；Polynomial 使用 Term* termArray、capacity、terms 管理非零項。

### 解題策略

沿用課堂「多項式物件」的動態陣列設計：初始容量為 2，陣列滿時加倍容量、複製舊項目，再釋放舊陣列。有效項目依指數由大到小排列，同一指數只留一項，係數為 0 的項不儲存。零多項式的 terms 為 0。

輸入格式自行定義為「項數，接著每項的係數、指數」。例如 `3 3 2 2 1 1 0` 表示 3x^2 + 2x + 1。輸入可不排序，也可有重複指數；讀完後排序、合併。`0` 表示零多項式。負項數、負指數、缺少資料或非數字會使輸入失敗，原物件保持不變。係數必須有限，計算溢位則回報錯誤。

加法沿用提供的程式碼：先複製第一個多項式，再逐項搜尋第二個多項式的指數是否已存在。存在就累加係數，不存在就新增項，最後正規化。

乘法沿用提供的三層迴圈：外面兩層列舉項目配對，相乘係數並相加指數，內層搜尋結果中的相同指數。找到就累加，找不到就新增。最後正規化並移除抵銷項，指數相加前先檢查 int 上限。

求值沿用提供的 pow 寫法，逐項計算 coef × pow(x, exp) 並累加。另檢查輸入和結果是否有限。

例如 A = 3x^2 + 2x + 1、B = x - 1：

- A + B = 3x^2 + 3x，因為常數 1 與 -1 抵銷。
- A × B = 3x^3 - x^2 - x - 1。
- x = 2 時，A(2) = 17、B(2) = 1。

## 程式實作

主程式為 src/main.cpp，類別定義為 src/polynomial.h，測試為 src/tests.cpp。Add、Mult 使用教材補充版的 const Polynomial& 參數，避免傳值造成額外複製，功能與題目 ADT 相同。

動態陣列必須搭配解構子、複製建構子與指定運算子。複製建構子配置自己的陣列，指定運算子採複製後交換，避免兩個物件共用指標而重複釋放。錯誤訊息使用 std::string，程式明確引用的標準標頭均在指引的上學期清單內。

輸出運算子透過成員函式 write 存取 Term。Polynomial 的友元函式不會自動成為 Term 的友元，因此不能直接在非成員的 << 中讀取 Term 的私有欄位。

### polynomial.h

```cpp
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>



class Polynomial;
class Term {
    friend class Polynomial;
private:
    float coef;
    int exp;
};

class Polynomial {
private:
    Term* termArray;
    int capacity;
    int terms;

    // Append a nonzero term; normalize after input and arithmetic.
    void newTerm(float coef, int exp) {
        if (!std::isfinite(coef)) throw std::string("Non-finite coefficient");
        if (coef == 0) return;
        if (terms == capacity) {
            if (capacity > static_cast<int>(~0u >> 1) / 2)
                throw std::string("Too many terms");
            Term* temp = new Term[capacity * 2];
            std::copy(termArray, termArray + terms, temp);
            delete[] termArray;
            termArray = temp;
            capacity *= 2;
        }
        termArray[terms].coef = coef;
        termArray[terms++].exp = exp;
    }

    // Normalize arbitrary input: sort, combine equal exponents, remove zeros.
    void normalize() {
        std::stable_sort(termArray, termArray + terms,
            [](const Term& a, const Term& b) { return a.exp > b.exp; });
        int out = 0;
        for (int i = 0; i < terms;) {
            int exponent = termArray[i].exp;
            float sum = 0;
            do { sum += termArray[i++].coef; }
            while (i < terms && termArray[i].exp == exponent);
            if (!std::isfinite(sum)) throw std::string("Coefficient overflow");
            if (sum != 0) {
                termArray[out].coef = sum;
                termArray[out++].exp = exponent;
            }
        }
        terms = out;
    }
public:
    Polynomial() : termArray(new Term[2]), capacity(2), terms(0) {}
    ~Polynomial() { delete[] termArray; }
    // Rule of Three: each polynomial owns its own dynamic array.
    Polynomial(const Polynomial& other)
        : termArray(new Term[other.capacity]), capacity(other.capacity), terms(other.terms) {
        std::copy(other.termArray, other.termArray + terms, termArray);
    }
    Polynomial& operator=(Polynomial other) {
        std::swap(termArray, other.termArray);
        std::swap(capacity, other.capacity);
        std::swap(terms, other.terms);
        return *this;
    }
    // const-reference parameters follow the supplementary lecture version.
    Polynomial Add(const Polynomial& poly) const {
        Polynomial result(*this);
        for (int i = 0; i < poly.terms; ++i) {
            bool found = false;
            for (int j = 0; j < result.terms; ++j) {
                if (result.termArray[j].exp == poly.termArray[i].exp) {
                    result.termArray[j].coef += poly.termArray[i].coef;
                    found = true;
                    break;
                }
            }
            if (!found) result.newTerm(poly.termArray[i].coef, poly.termArray[i].exp);
        }
        result.normalize();
        return result;
    }
    Polynomial Mult(const Polynomial& poly) const {
        Polynomial result;
        for (int i = 0; i < terms; ++i) {
            for (int j = 0; j < poly.terms; ++j) {
                if (termArray[i].exp > static_cast<int>(~0u >> 1) - poly.termArray[j].exp)
                    throw std::string("Exponent overflow");
                float newCoef = termArray[i].coef * poly.termArray[j].coef;
                int newExp = termArray[i].exp + poly.termArray[j].exp;
                if (!std::isfinite(newCoef)) throw std::string("Coefficient overflow");
                bool found = false;
                for (int k = 0; k < result.terms; ++k) {
                    if (result.termArray[k].exp == newExp) {
                        result.termArray[k].coef += newCoef;
                        found = true;
                        break;
                    }
                }
                if (!found) result.newTerm(newCoef, newExp);
            }
        }
        result.normalize();
        return result;
    }
    float Eval(float x) const {
        if (!std::isfinite(x)) throw std::string("x must be finite");
        float sum = 0;
        for (int i = 0; i < terms; ++i) {
            sum += termArray[i].coef * static_cast<float>(std::pow(x, termArray[i].exp));
            if (!std::isfinite(sum)) throw std::string("Evaluation overflow");
        }
        return sum;
    }
    // Input: count followed by (coefficient, exponent) pairs.
    // On failure, leave the original polynomial unchanged.
    friend std::istream& operator>>(std::istream& in, Polynomial& p) {
        int count;
        if (!(in >> count)) return in;
        if (count < 0) { in.setstate(std::ios::failbit); return in; }
        Polynomial temp;
        for (int i = 0; i < count; ++i) {
            float coef; int exp;
            if (!(in >> coef >> exp)) return in;
            if (!std::isfinite(coef) || exp < 0) {
                in.setstate(std::ios::failbit); return in;
            }
            temp.newTerm(coef, exp);
        }
        temp.normalize();
        p = temp;
        return in;
    }
    std::ostream& write(std::ostream& out) const {
        if (terms == 0) return out << 0;
        for (int i = 0; i < terms; ++i) {
            float c = termArray[i].coef;
            int e = termArray[i].exp;
            if (i) out << (c < 0 ? " - " : " + ");
            else if (c < 0) out << '-';
            float magnitude = std::fabs(c);
            if (e == 0 || magnitude != 1) out << magnitude;
            if (e > 0) { out << 'x'; if (e != 1) out << '^' << e; }
        }
        return out;
    }
    friend std::ostream& operator<<(std::ostream& out, const Polynomial& p) {
        return p.write(out);
    }
};
#endif





```

### main.cpp

```cpp
#include "polynomial.h"
int main() {
    try {
        Polynomial a, b;
        float x;
        std::cout << "Enter A: term count, then coefficient exponent pairs\n";
        if (!(std::cin >> a)) { std::cerr << "Invalid polynomial A\n"; return 1; }
        std::cout << "Enter B: term count, then coefficient exponent pairs\n";
        if (!(std::cin >> b)) { std::cerr << "Invalid polynomial B\n"; return 1; }
        std::cout << "Enter x:\n";
        if (!(std::cin >> x)) { std::cerr << "Invalid x\n"; return 1; }
        std::cout << "A(x) = " << a << "\nB(x) = " << b
                  << "\nA + B = " << a.Add(b) << "\nA * B = " << a.Mult(b)
                  << "\nA(" << x << ") = " << a.Eval(x)
                  << "\nB(" << x << ") = " << b.Eval(x) << '\n';
    } catch (const std::string& e) { std::cerr << e << '\n'; return 1; }
}


```

## 效能分析

設輸入原始項數為 k，兩個正規化多項式分別有 m、n 項。加法整理前最多有 m+n 項；乘法曾建立的不同指數最多有 r 項，r ≤ mn。因為直到最後才清除抵銷項，r 可能大於最終非零項數。空間包含結果陣列。

| 操作 | 時間複雜度 | 額外空間 |
| --- | --- | --- |
| 建構零多項式 | O(1) | O(1) |
| newTerm | 攤銷 O(1)，單次擴容 O(k) | 擴容時 O(k) |
| 輸入與正規化 | 一般 O(k log(k+1)) | O(k) |
| Add 搜尋階段 | O(m + n(m+n)) | O(m+n) |
| Add 含最後排序 | 一般 O(m+n(m+n)+(m+n)log(m+n+1)) | O(m+n) |
| Mult 含最後排序 | 一般 O(mn(r+1)+r log(r+1)) | O(r+1) |
| Eval | m 次 pow 呼叫及 O(m) 次累加 | O(1) |
| 輸出 | O(m)，另加字元格式化成本 | O(1) |
| 複製、指定 | O(m) 複製 | O(capacity) |

當 m=n=N 時，加法最壞為 O(N²)，乘法最壞可達 O(N⁴)，因為 mn 個配對的每次搜尋都可能走訪 O(mn) 個結果項。若指數連續，結果不同指數僅 O(N) 個，乘法搜尋部分則為 O(N³)。不能將這份程式誤寫為雙指標加法的 O(m+n)，或只看乘法外層兩個迴圈就認定為 O(mn)。

stable_sort 在有足夠暫存記憶體時需要 O(k log k) 比較，若無法配置排序緩衝區，後備版本最壞為 O(k log²(k+1)) 比較。因此表格中正規化時間的 log 因子，保守最壞分析應改為 log²。排序緩衝區不改變表列的線性空間上限。

Eval 通常在把 pow 視為固定成本時記為 O(m)，但 C++ 並未為 pow 的實作規定這個時間界限。更精確可寫為 O(m × C_pow)，C_pow 是該平台一次 pow 的成本。本程式未使用自行實作的平方求冪，不能直接套用其 O(log E) 分析。

容量每次加倍，追加 k 項時複製量為 2 + 4 + 8 + …，總量為 O(k)，所以追加攤銷成本為 O(1)。抵銷後不縮容，物件使用的記憶體應以 capacity 計算，而非只看目前 terms。

### 正確性說明

正規化將同指數的項排在一起，逐群加總後只留下非零係數，因此保持多項式的值，並讓指數嚴格遞減。

加法先將 A 複製到結果。每處理 B 的一項，就將係數加到相同指數，或建立新的項。因此處理 B 的前 i 項後，結果等於 A 加上這 i 項之和；所有項處理完就得到 A+B。

乘法的外面兩層迴圈恰好列舉所有項目配對，依分配律產生完整乘積。內層搜尋只把相同指數的係數相加，最後正規化不改變多項式的值。

Eval 逐項累加 coef × x^exp，正是多項式的數學定義。上述代數推論採實數運算，實際 float 仍可能有捨入或下溢，係數以恰好等於 0 判定抵銷，沒有任意以 epsilon 刪除很小但非零的項。

## 測試與驗證

### 編譯與執行指令

以下為 GNU C++ 的重現方式；程式使用 C++11 功能，但本次環境實際使用 MSVC 編譯，未宣稱執行過 g++。

```shell
$ cd homework2
$ g++ -std=c++11 -O2 -Wall -Wextra -pedantic src/main.cpp -o polynomial
$ g++ -std=c++11 -O2 -Wall -Wextra -pedantic src/tests.cpp -o tests
$ ./tests
PASS 1518 checks
$ ./polynomial < sample-input.txt
```

實際使用 Windows x64、Visual Studio 2022 MSVC 19.44，編譯參數為 `/EHsc /O2 /W4 /std:c++14 /utf-8`。MSVC 沒有獨立 C++11 模式，因此選用 C++14 模式。執行測試後輸出為 `PASS 1518 checks`。

### 完整示範輸入

```text
3 3 2 2 1 1 0
2 1 1 -1 0
2
```

### 實際示範輸出
```text
Enter A: term count, then coefficient exponent pairs
Enter B: term count, then coefficient exponent pairs
Enter x:
A(x) = 3x^2 + 2x + 1
B(x) = x - 1
A + B = 3x^2 + 3x
A * B = 3x^3 - x^2 - x - 1
A(2) = 17
B(2) = 1

```

### 測試案例

| 案例 | 預期結果 | 實際結果 |
| --- | --- | --- |
| 範例加法 | 3x^2 + 3x | 相同 |
| 範例乘法 | 3x^3 - x^2 - x - 1 | 相同 |
| 範例代入 2 | 17 | 17 |
| 零多項式與乘零 | 0 | 0 |
| 加上零多項式 | 原多項式 | 相同 |
| 未排序、重複常數抵銷 | 3x^4 + x | 相同 |
| x^3 與 -x^3 合併 | 0 | 0 |
| 係數為 ±1 的輸出 | -x^2 + x - 5 | 相同 |
| 常數 7 代入 0 | 7 | 7 |
| 0.5x^2 - 0.25 代入 2 | 1.75 | 1.75 |
| 複製後修改原物件 | 複本不受影響 | 通過 |
| 自我指定 | 值不變 | 通過 |
| 負項數、負指數、不完整輸入、非數字 | failbit 且保留原物件 | 通過 |
| 指數相加超過 int 範圍 | 回報錯誤 | 通過 |

另外以 mt19937 固定種子 2026 產生 100 組多項式，每個多項式含指數 0 至 5 的係數，係數為 -2 至 2。測試以獨立的整數陣列卷積算乘積，以整數 Horner 法算預期值，再在 x = -2、-1、0、1、2 比較 Eval、Add 與 Mult。共 1,500 次隨機交叉檢查，加上 18 次固定檢查，合計 1,518 次。

這些測試涵蓋正常、邊界與錯誤輸入，但不是對所有浮點數輸入的窮舉證明。完整測試程式如下。

### tests.cpp
```cpp
#include "polynomial.h"
#include <sstream>
#include <string>
#include <random>

static int checks = 0;
void check(bool value) {
    ++checks;
    if (!value) throw std::string("Test failed: " + std::to_string(checks));
}
Polynomial parse(const std::string& s) {
    Polynomial p; std::istringstream in(s);
    if (!(in >> p)) throw std::string("Bad test input");
    return p;
}
std::string show(const Polynomial& p) { std::ostringstream out; out << p; return out.str(); }
int main() {
    Polynomial a = parse("3 3 2 2 1 1 0"), b = parse("2 1 1 -1 0");
    check(show(a.Add(b)) == "3x^2 + 3x");
    check(show(a.Mult(b)) == "3x^3 - x^2 - x - 1");
    check(a.Eval(2) == 17);
    check(show(Polynomial()) == "0");
    check(show(a.Mult(Polynomial())) == "0");
    check(show(a.Add(Polynomial())) == show(a));
    check(show(parse("4 2 0 3 4 -2 0 1 1")) == "3x^4 + x");
    check(show(parse("2 1 3 -1 3")) == "0");
    check(show(parse("3 -1 2 1 1 -5 0")) == "-x^2 + x - 5");
    check(parse("1 7 0").Eval(0) == 7);
    check(parse("2 0.5 2 -0.25 0").Eval(2) == 1.75f);
    Polynomial copy(a); a = b; check(show(copy) == "3x^2 + 2x + 1");
    copy = copy; check(show(copy) == "3x^2 + 2x + 1");
    for (const std::string& input : {"-1", "1 2 -1", "2 3 1", "hello"}) {
        Polynomial p = b; std::istringstream in(input); in >> p;
        check(in.fail() && show(p) == show(b));
    }
    bool overflow = false;
    try { parse("1 1 2147483647").Mult(b); }
    catch (const std::string&) { overflow = true; }
    check(overflow);
    // Independent dense-array oracle, small integer coefficients are exact as float.
    std::mt19937 rng(2026);
    for (int test = 0; test < 100; ++test) {
        int ac[6], bc[6], product[11] = {};
        std::ostringstream ai, bi; ai << 6; bi << 6;
        for (int i = 0; i < 6; ++i) {
            ac[i] = static_cast<int>(rng() % 5) - 2;
            bc[i] = static_cast<int>(rng() % 5) - 2;
            ai << ' ' << ac[i] << ' ' << i; bi << ' ' << bc[i] << ' ' << i;
        }
        for (int i = 0; i < 6; ++i) for (int j = 0; j < 6; ++j) product[i+j] += ac[i]*bc[j];
        Polynomial p = parse(ai.str()), q = parse(bi.str());
        for (int x = -2; x <= 2; ++x) {
            int av = 0, bv = 0, pv = 0;
            for (int i = 5; i >= 0; --i) { av = av*x + ac[i]; bv = bv*x + bc[i]; }
            for (int i = 10; i >= 0; --i) pv = pv*x + product[i];
            check(p.Eval(static_cast<float>(x)) == av);
            check(p.Add(q).Eval(static_cast<float>(x)) == av+bv);
            check(p.Mult(q).Eval(static_cast<float>(x)) == pv);
        }
    }
    std::cout << "PASS " << checks << " checks\n";
}


```


## 效能量測

量測環境為 Windows x64、Intel Core i7-12700KF、MSVC 19.44，使用 /O2。為量測最後採用的使用者程式演算法，以 measure.ps1 的 Measure-Command 計時完整主程式執行，每個大小重複 5 次。輸入 A=B=1+x+…+x^(N-1)，代入 x=1。量測包含啟動程序、輸入、加法、乘法、求值及完整輸出處理，單位為毫秒。

先前獨立 C++ 計時器在重編譯後受到此電腦 Device Guard 政策阻擋，因此不使用該計時器的舊演算法數據充當最後版本量測。本表採用已正常執行的最後主程式，屬端到端時間，不能當成個別 Add 或 Mult 的時間。

| N | 平均 ms | 最小 ms | 最大 ms |
| --- | --- | --- | --- |
| 10 | 10.178 | 5.814 | 25.993 |
| 50 | 6.734 | 6.358 | 7.016 |
| 100 | 6.347 | 5.947 | 7.024 |
| 200 | 9.723 | 8.240 | 14.021 |

N 較小時程序啟動與量測雜訊佔比大，首次執行也可能較慢，因此平均值不一定隨 N 單調增加。這些結果只代表本機五次執行的觀察，不能用來證明漸進複雜度。此資料的指數連續，乘積僅有 2N-1 個可能指數，未涵蓋 r 接近 N² 的最壞情況。

重現方式（在 homework2 目錄下，以 PowerShell 執行）：

```powershell
.\measure.ps1 -Executable (Resolve-Path .\polynomial.exe).Path
```

## 申論及開發報告

### 動態陣列的選擇

動態陣列符合題目指定的 Term*、capacity、terms，能清楚觀察配置、擴容、複製及釋放記憶體的流程。只儲存非零項，對高次但稀疏的多項式，比依最高次數配置完整係數陣列更省空間。

### 提供程式碼的修正

本作業以提供的線性搜尋 Add、Mult 和 pow 求值為基礎。補上深層複製是因為只寫解構子時，預設複製會讓兩個物件指向同一塊記憶體，可能導致重複釋放。不能只依賴編譯器的回傳值最佳化來避免這個問題。

輸出透過 Polynomial 成員函式讀取 Term，保留 Term 的封裝。輸入先存到暫時物件，驗證與正規化完成才指定給原物件，避免讀到一半出錯就破壞原資料。輸出省略係數 1 和指數 1 的多餘表示，也正確處理首項負號與零多項式。

### 心得討論與改進

這份實作最需要注意的是資料表示是否一直符合規則。即使加法的數值算對了，若留下零係數項，輸出與「非零項陣列」的定義仍會不一致。運算結束後統一正規化能避免這種差異。

線性搜尋同類項容易理解，但三層迴圈在項數增加時會較慢。如果繼續改進，可改成教材的雙指標加法，將時間降為 O(m+n)；乘法可先列出乘積項再排序合併，或使用適合的索引結構。不過這些會改變目前提供程式碼的演算法，本次保留原本搜尋方式並如實分析成本。

浮點數計算也會受到加總順序影響。測試採用小整數係數以便與整數參考答案精確比較；一般小數應搭配絕對與相對誤差檢查。本次沒有宣稱所有浮點輸入都能精確表示。

## 參考資料

- 課程提供的 Homework 2 題目簡報：Polynomial ADT、私有成員與作業評分項目。
- 課程提供的「多項式物件」教材：Term、動態陣列、newTerm 與容量加倍。
- 使用者提供的 C++ 程式碼：以迴圈搜尋同類項的 Add、Mult 與 pow 求值。
- [NFU 資料結構作業規範](https://github.com/NFU-OpenDataStructure/NFU-DS-Instruction)：目錄結構、標頭限制與報告章節。
- [Homework-template](https://github.com/NFU-OpenDataStructure/Homework-template)：報告樣板及官方格式檢查檔。
- [Homework 1 範例](https://github.com/biba1117/homework-1)：程式碼與 Markdown 報告的呈現方式，未使用其中的簡報。
