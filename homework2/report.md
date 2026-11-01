# 41343146

作業二

## 解題說明

實作 Polynomial 類別，完成多項式的加法、乘法、代入 x 和輸入輸出。

### 解題策略

1. Term 儲存每一項的係數 coef 和指數 exp。
2. Polynomial 用動態陣列儲存各項，capacity 是容量，terms 是項數。
3. newTerm 新增一項，容量不夠就加倍，再複製原本的資料。
4. addTerm 搜尋相同指數，有找到就加上係數，沒有就新增，並依指數由大到小排列。係數變成 0 就移除。
5. Add 先複製第一個多項式，再把第二個多項式的項逐一加入。
6. Mult 用兩層迴圈把各項配對，係數相乘、指數相加，再合併同類項。
7. Eval 使用 pow 計算各項並累加。
8. >> 讀入項數和每項的係數、指數，<< 負責輸出。

例如 A = 3x^2 + 2x + 1，B = x - 1，相加為 3x^2 + 3x，相乘為 3x^3 - x^2 - x - 1。

## 程式實作

### src/header.h
```cpp
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include <iostream>
#include <cmath>
using namespace std;

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

    void newTerm(float coef, int exp) {
        if (coef == 0) return;
        if (terms == capacity) {
            Term* temp = new Term[capacity * 2];
            for (int i = 0; i < terms; i++)
                temp[i] = termArray[i];
            delete[] termArray;
            termArray = temp;
            capacity *= 2;
        }
        termArray[terms].coef = coef;
        termArray[terms].exp = exp;
        terms++;
    }

    // 找相同指數，沒有就新增，最後依指數排列。
    void addTerm(float coef, int exp) {
        if (coef == 0) return;
        for (int i = 0; i < terms; i++) {
            if (termArray[i].exp == exp) {
                termArray[i].coef += coef;
                if (termArray[i].coef == 0) {
                    for (int j = i; j < terms - 1; j++)
                        termArray[j] = termArray[j + 1];
                    terms--;
                }
                return;
            }
        }
        newTerm(coef, exp);
        for (int i = terms - 1; i > 0; i--) {
            if (termArray[i].exp > termArray[i - 1].exp) {
                Term temp = termArray[i];
                termArray[i] = termArray[i - 1];
                termArray[i - 1] = temp;
            } else break;
        }
    }

public:
    Polynomial() {
        capacity = 2;
        terms = 0;
        termArray = new Term[capacity];
    }
    ~Polynomial() { delete[] termArray; }

    // 複製資料，讓每個物件使用自己的陣列。
    Polynomial(const Polynomial& p) {
        capacity = p.capacity;
        terms = p.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms && i < capacity; i++)
            termArray[i] = p.termArray[i];
    }
    Polynomial& operator=(const Polynomial& p) {
        if (this == &p) return *this;
        Term* temp = new Term[p.capacity];
        for (int i = 0; i < p.terms && i < p.capacity; i++)
            temp[i] = p.termArray[i];
        delete[] termArray;
        termArray = temp;
        capacity = p.capacity;
        terms = p.terms;
        return *this;
    }

    Polynomial Add(const Polynomial& poly) const {
        Polynomial result(*this);
        for (int i = 0; i < poly.terms; i++)
            result.addTerm(poly.termArray[i].coef, poly.termArray[i].exp);
        return result;
    }
    Polynomial Mult(const Polynomial& poly) const {
        Polynomial result;
        for (int i = 0; i < terms; i++) {
            for (int j = 0; j < poly.terms; j++) {
                float coef = termArray[i].coef * poly.termArray[j].coef;
                int exp = termArray[i].exp + poly.termArray[j].exp;
                result.addTerm(coef, exp);
            }
        }
        return result;
    }
    float Eval(float x) const {
        float sum = 0;
        for (int i = 0; i < terms; i++)
            sum += termArray[i].coef * (float)pow(x, termArray[i].exp);
        return sum;
    }

    // 用成員函式讀取 Term 的私有資料。
    ostream& print(ostream& out) const {
        if (terms == 0) return out << 0;
        for (int i = 0; i < terms; i++) {
            float coef = termArray[i].coef;
            int exp = termArray[i].exp;
            if (i > 0) out << (coef < 0 ? " - " : " + ");
            else if (coef < 0) out << '-';
            float absCoef = (coef < 0) ? -coef : coef;
            if (absCoef != 1 || exp == 0) out << absCoef;
            if (exp > 0) {
                out << 'x';
                if (exp > 1) out << '^' << exp;
            }
        }
        return out;
    }
    friend istream& operator>>(istream& in, Polynomial& p) {
        int n;
        if (!(in >> n)) return in;
        if (n < 0) { in.setstate(ios::failbit); return in; }
        Polynomial temp;
        for (int i = 0; i < n; i++) {
            float coef;
            int exp;
            if (!(in >> coef >> exp)) return in;
            if (exp < 0) { in.setstate(ios::failbit); return in; }
            temp.addTerm(coef, exp);
        }
        p = temp;
        return in;
    }
    friend ostream& operator<<(ostream& out, const Polynomial& p) {
        return p.print(out);
    }
};
#endif


```

### src/main.cpp

```cpp
#include "header.h"

int main() {
    Polynomial A, B;
    cout << "輸入 Polynomial A（項數，接著輸入係數 指數）：\n";
    if (!(cin >> A)) { cout << "輸入錯誤\n"; return 1; }
    cout << "輸入 Polynomial B（項數，接著輸入係數 指數）：\n";
    if (!(cin >> B)) { cout << "輸入錯誤\n"; return 1; }

    cout << "A(x) = " << A << endl;
    cout << "B(x) = " << B << endl;
    Polynomial C = A.Add(B);
    Polynomial D = A.Mult(B);
    cout << "A + B = " << C << endl;
    cout << "A * B = " << D << endl;

    float x;
    cout << "輸入 x：\n";
    if (!(cin >> x)) { cout << "輸入錯誤\n"; return 1; }
    cout << "A(" << x << ") = " << A.Eval(x) << endl;
    cout << "B(" << x << ") = " << B.Eval(x) << endl;
    return 0;
}


```

## 效能分析

設兩個多項式分別有 n、m 項，r 是乘法過程中最多儲存的項數，r ≤ nm。

| 函式 | 時間複雜度 | 空間複雜度 |
| --- | --- | --- |
| newTerm | 攤銷 O(1)，擴容時 O(k) | 擴容時 O(k) |
| addTerm | O(k) | O(k)，包含擴容 |
| Add | O(n + m(n+m)) | O(n+m) |
| Mult | O(nm(r+1))，最壞 O((nm)^2) | O(r+1) |
| Eval | O(n)，將 pow 視為固定成本 | O(1) |
| 輸入 | O(k²) | O(k) |
| 輸出 | O(n) | O(1) |

k 是該操作處理的項數。加法和乘法都會搜尋同類項，因此不能只看外層迴圈計算時間。

## 測試與驗證

### 編譯方式

在 homework2 資料夾執行：

```shell
g++ -std=c++11 -Wall -Wextra -pedantic src/main.cpp -o polynomial
```

本次實際使用 Visual Studio 2022 的 MSVC，編譯參數為 `/EHsc /O2 /W4 /std:c++14 /utf-8`。

### 測試一

輸入兩個多項式，最後輸入 x：

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
| 相同指數的係數互相抵銷 | 移除該項 | 相同 |
| 常數 7 代入 x=0 | 7 | 7 |
| 0.5x^2 - 0.25 代入 x=2 | 1.75 | 1.75 |
| 複製後修改原物件 | 複本不變 | 相同 |
| 輸入負指數 | 顯示輸入錯誤 | 相同 |

程式適用於一般有限係數，指數須為非負整數，且乘法的指數和不能超過 int 範圍。浮點數計算可能有誤差。

### 效能量測

使用 i7-12700KF 電腦，兩個多項式都輸入 1+x+…+x^(N-1)，代入 x=1。每組執行 5 次，量測完整程式執行時間。

| 每個多項式項數 N | 平均時間（毫秒） |
| --- | --- |
| 10 | 8.613 |
| 50 | 7.307 |
| 100 | 7.000 |
| 200 | 9.092 |

時間包含程式啟動、輸入和輸出，所以小資料的結果容易受到其他因素影響，不能當作乘法本身的時間。

## 心得討論

這題要注意相同指數的項要合併，係數加起來變成 0 的項也要刪除，否則輸出會多出沒有用的項。

另外，動態陣列不能只複製指標，所以補上複製建構子和指定運算子，讓每個物件有自己的陣列，避免重複釋放。

目前用迴圈搜尋同類項比較容易理解，不過項數多時會比較慢，之後可以用教材的雙指標方法改進加法。

