# 41343146

作業二

## 解題說明

這次作業要做一個 Polynomial 類別，完成多項式的加法、乘法、代入 x，以及輸入和輸出。

### 解題策略

1. Term 用來儲存每一項的係數 coef 和指數 exp。
2. Polynomial 用動態陣列儲存各項，capacity 代表容量，terms 代表目前的項數。
3. newTerm 負責新增項目，容量不夠時就變成原本的兩倍，再把舊資料複製過去。
4. Add 先複製第一個多項式，再加入第二個多項式。遇到相同指數就把係數相加。
5. Mult 用兩層迴圈把各項相乘，係數相乘、指數相加，再找有沒有相同指數可以合併。
6. Eval 使用 pow 計算每一項，最後全部加起來。
7. >> 先讀項數，再讀每項的係數和指數；<< 負責輸出正負號、係數和指數。
8. 輸入和運算後會整理各項，合併相同指數，並移除係數為 0 的項。

例如 A = 3x^2 + 2x + 1，B = x - 1：

- A + B = 3x^2 + 3x。
- A * B = 3x^3 - x^2 - x - 1。
- 代入 x = 2，A(2) = 17，B(2) = 1。

## 程式實作

類別寫在 src/polynomial.h，主程式寫在 src/main.cpp。

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

設 A 有 n 項，B 有 m 項，r 是乘法過程中建立的不同指數數量，最多為 nm。

| 函式 | 時間複雜度 | 空間複雜度 |
| --- | --- | --- |
| newTerm | 平均每次 O(1)，擴容時 O(k) | 擴容時 O(k) |
| Add | O(n + m(n+m) + (n+m)log(n+m+1)) | O(n+m) |
| Mult | O(nm(r+1) + r log(r+1)) | O(r+1) |
| Eval | O(n)，將一次 pow 視為固定成本 | O(1) |
| 輸入 | O(k log(k+1)) | O(k) |
| 輸出 | O(n) | O(1) |

Add 和 Mult 都要搜尋結果中有沒有相同指數，所以不能只計算外面的迴圈。當 n=m=N 時，Add 最壞為 O(N²)，Mult 最壞可達 O(N⁴)。

表格的排序成本以 stable_sort 有足夠暫存記憶體的情況計算。如果排序無法配置暫存空間，log 因子會變成 log²。newTerm 表中的 k 是當時已儲存的項數。

## 測試與驗證

### 編譯方式

在 homework2 資料夾執行：

```shell
g++ -std=c++11 -O2 -Wall -Wextra -pedantic src/main.cpp -o polynomial
```

本次實際使用 Visual Studio 2022 的 MSVC 編譯，參數如下：

```shell
cl /EHsc /O2 /W4 /std:c++14 /utf-8 src/main.cpp /Fe:polynomial.exe
polynomial.exe < sample-input.txt
```

### 測試一：一般多項式

輸入格式為「項數，接著每項的係數、指數」，最後輸入 x。

```text
3 3 2 2 1 1 0
2 1 1 -1 0
2
```

計算結果：

```text
A(x) = 3x^2 + 2x + 1
B(x) = x - 1
A + B = 3x^2 + 3x
A * B = 3x^3 - x^2 - x - 1
A(2) = 17
B(2) = 1
```

### 其他測試

| 測試內容 | 預期結果 | 實際結果 |
| --- | --- | --- |
| 多項式乘以 0 | 0 | 0 |
| 多項式加上 0 | 原多項式 | 相同 |
| x^3 與 -x^3 合併 | 0 | 0 |
| 常數 7 代入 x=0 | 7 | 7 |
| 0.5x^2 - 0.25 代入 x=2 | 1.75 | 1.75 |
| 輸入負指數 | 輸入失敗 | 相同 |
| 複製後修改原物件 | 複本不變 | 相同 |

另外使用 src/tests.cpp 做固定測試和隨機測試，結果為：

```text
PASS 1518 checks
```

### 效能量測

使用 i7-12700KF 電腦，以 PowerShell 量測完整程式執行時間。兩個多項式都輸入 1+x+…+x^(N-1)，代入 x=1，每組執行 5 次。

| 每個多項式的項數 N | 平均時間（毫秒） |
| --- | --- |
| 10 | 10.178 |
| 50 | 6.734 |
| 100 | 6.347 |
| 200 | 9.723 |

這些時間包含程式啟動、輸入和輸出，所以小資料不一定比較快，也不能直接當作乘法本身的時間。量測資料放在 measurements.csv，量測工具為 measure.ps1。

### 結論

程式可以完成多項式的加法、乘法和代入求值，也能處理零多項式、相同指數與負係數。

## 心得討論

這題比較需要注意的是，相同指數的項要合併，係數相加後變成 0 的項也要移除，不然輸出會多出沒有用的項。

另外，動態陣列不能只複製指標，所以程式補上複製建構子和指定運算子，讓每個物件有自己的陣列，避免重複釋放記憶體。

目前用迴圈搜尋同類項比較容易理解，但項數多時會比較慢。之後可以把加法改成教材的雙指標方式，減少搜尋次數。
