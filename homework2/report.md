# 41343146

作業二

## 解題說明

完成多項式計算

### 解題策略

1. Term 儲存係數 coef 和指數 exp。
2. Polynomial 使用動態陣列，初始容量為 10，容量不夠就加倍。
3. newTerm 把新的一項放到陣列最後。
4. Add 先放入第一個多項式，再逐項搜尋第二個多項式的指數，指數相同就把係數相加。
5. Mult 把兩個多項式的各項配對，係數相乘、指數相加，再合併相同指數。
6. Eval 使用 pow 計算每項的值，再加總。
7. ">>" 讀入項數、係數和指數；"<<" 輸出各項。

## 程式實作


```cpp

#include <iostream>
#include <cmath>
using namespace std;

// 儲存多項式的單一項
class Term
{
public:
    float coef; // 係數
    int exp;    // 指數
};

// 多項式類別
class Polynomial
{
private:
    Term* termArray; // 儲存多項式的動態陣列
    int capacity;    // 陣列容量
    int terms;       // 目前項數

    // 新增一項
    void newTerm(float coef, int exp)
    {
        // 容量不足時擴充兩倍
        if (terms >= capacity)
        {
            capacity *= 2;

            Term* temp = new Term[capacity];

            // 複製舊資料
            for (int i = 0; i < terms; i++)
            {
                temp[i] = termArray[i];
            }

            delete[] termArray;
            termArray = temp;
        }

        // 儲存係數和指數
        termArray[terms].coef = coef;
        termArray[terms].exp = exp;
        terms++;
    }

public:
    // 建構子，預設容量10
    Polynomial(int cap = 10)
    {
        capacity = cap;
        terms = 0;
        termArray = new Term[capacity];
    }

    // 解構子，釋放記憶體
    ~Polynomial()
    {
        delete[] termArray;
    }

    // 輸入與輸出運算子
    friend istream& operator>>(istream& in, Polynomial& p);
    friend ostream& operator<<(ostream& out, const Polynomial& p);

    // 多項式加法
    Polynomial Add(const Polynomial& poly);

    // 多項式乘法
    Polynomial Mult(const Polynomial& poly);

    // 計算多項式的值
    float Eval(float x);
};

// 輸入多項式
istream& operator>>(istream& in, Polynomial& p)
{
    int n;

    cout << "請輸入項數: ";
    in >> n;

    p.terms = 0;

    cout << "請輸入係數與指數\n";

    // 逐項輸入係數與指數
    for (int i = 0; i < n; i++)
    {
        float c;
        int e;

        in >> c >> e;

        p.newTerm(c, e);
    }

    return in;
}

// 輸出多項式
ostream& operator<<(ostream& out, const Polynomial& p)
{
    // 沒有項數時輸出0
    if (p.terms == 0)
    {
        out << "0";
        return out;
    }

    for (int i = 0; i < p.terms; i++)
    {
        float coef = p.termArray[i].coef;
        int exp = p.termArray[i].exp;

        // 正數加上加號
        if (i > 0 && coef >= 0)
            out << " + ";

        // 負數加上減號
        if (coef < 0)
            out << " - ";

        if (coef < 0)
            coef = -coef;

        out << coef;

        // 指數大於0才輸出x
        if (exp > 0)
        {
            out << "x";

            if (exp > 1)
                out << "^" << exp;
        }
    }

    return out;
}

// 多項式加法
Polynomial Polynomial::Add(const Polynomial& poly)
{
    Polynomial result;

    // 先複製第一個多項式
    for (int i = 0; i < terms; i++)
    {
        result.newTerm(
            termArray[i].coef,
            termArray[i].exp
        );
    }

    // 加入第二個多項式
    for (int i = 0; i < poly.terms; i++)
    {
        bool found = false;

        // 尋找相同指數的項
        for (int j = 0; j < result.terms; j++)
        {
            if (result.termArray[j].exp ==
                poly.termArray[i].exp)
            {
                // 相同指數就合併係數
                result.termArray[j].coef +=
                    poly.termArray[i].coef;

                found = true;
                break;
            }
        }

        // 沒有相同指數就新增
        if (!found)
        {
            result.newTerm(
                poly.termArray[i].coef,
                poly.termArray[i].exp
            );
        }
    }

    return result;
}

// 多項式乘法
Polynomial Polynomial::Mult(const Polynomial& poly)
{
    Polynomial result;

    // 每一項互相相乘
    for (int i = 0; i < terms; i++)
    {
        for (int j = 0; j < poly.terms; j++)
        {
            // 係數相乘
            float newCoef =
                termArray[i].coef *
                poly.termArray[j].coef;

            // 指數相加
            int newExp =
                termArray[i].exp +
                poly.termArray[j].exp;

            bool found = false;

            // 尋找相同指數
            for (int k = 0; k < result.terms; k++)
            {
                if (result.termArray[k].exp == newExp)
                {
                    // 合併同類項
                    result.termArray[k].coef += newCoef;
                    found = true;
                    break;
                }
            }

            // 沒有相同指數就新增
            if (!found)
            {
                result.newTerm(newCoef, newExp);
            }
        }
    }

    return result;
}

// 代入x計算多項式的值
float Polynomial::Eval(float x)
{
    float sum = 0;

    for (int i = 0; i < terms; i++)
    {
        // 係數乘上x的指數次方
        sum += termArray[i].coef *
            pow(x, termArray[i].exp);
    }

    return sum;
}

int main()
{
    // 建立兩個多項式
    Polynomial A;
    Polynomial B;

    // 輸入多項式
    cout << "輸入 Polynomial A\n";
    cin >> A;

    cout << "\n輸入 Polynomial B\n";
    cin >> B;

    // 顯示多項式
    cout << "\nA(x) = " << A << endl;
    cout << "B(x) = " << B << endl;

    // 計算加法
    Polynomial C = A.Add(B);
    cout << "\nA + B = " << C << endl;

    // 計算乘法
    Polynomial D = A.Mult(B);
    cout << "A * B = " << D << endl;

    float x;

    // 輸入x並計算結果
    cout << "\n輸入 x：";
    cin >> x;

    cout << "A(" << x << ") = "
        << A.Eval(x) << endl;

    cout << "B(" << x << ") = "
        << B.Eval(x) << endl;

    return 0;
}

```



## 效能分析

設兩個多項式分別有 n、m 項，k 是輸入項數。

| 函式 | 時間複雜度 | 空間複雜度 |
| --- | --- | --- |
| newTerm | 攤銷 O(1)，擴容時 O(k) | 擴容時 O(k) |
| Add | O(n + m(n+m+1)) | O(n+m+1) |
| Mult | O(n + nm(1+nm))，n、m 皆大於 0 時最壞 O((nm)^2) | O(nm+1) |
| Eval | O(n)，將 pow 視為固定成本 | O(1) |
| 輸入 | O(k) | O(k) |
| 輸出 | O(n) | O(1) |

加法和乘法需要搜尋結果中的相同指數，所以乘法不能只看外層兩個迴圈就當作 O(nm)。

## 測試與驗證



### 測試案例

輸入:

```text
3 3 2 2 1 1 0
2 1 1 -1 0
2
```

：

```text
A(x) = 3x^2 + 2x + 1
B(x) = 1x - 1
A + B = 3x^2 + 3x + 0
A * B = 3x^3 - 1x^2 - 1x - 1
A(2) = 17
B(2) = 1
```


### 結論

1. 功能：程式實作了 Polynomial 類別，包含新增項目（newTerm）、相加（Add）、相乘（Mult）、代入求值（Eval）以及輸入、輸出運算子，可完成範例的多項式運算與顯示。
2. 記憶體管理：使用動態陣列儲存各項，容量不足時加倍並複製原有資料，最後由解構子釋放陣列。目前仍會保留零係數項，也尚未處理物件的深層複製。
3. 複雜度：設兩個多項式分別有 n、m 項，Add 的時間為 O(n + m(n+m+1))，空間為 O(n+m+1)；當 n、m 皆大於 0 時，Mult 最壞時間為 O((nm)^2)，空間為 O(nm+1)；Eval 將 pow 視為固定成本時，時間為 O(n)，空間為 O(1)。

## 心得討論

這次作業主要是利用 C++ 類別來實作多項式的加法、乘法及代入計算。透過這次練習，我更加了解類別、動態陣列及函式的使用方式。在完成程式我用chat gpt檢查。對 ai說我的程式碼如果用太古早味的c++編譯器很可能壞掉雖然我用c++20測試上都能正常使用，經了解後得知是程式可能因為編譯器對物件回傳的最佳化方式不同，而產生記憶體管理問題。


