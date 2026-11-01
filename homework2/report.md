# 41343146

作業二

## 解題說明

使用 Polynomial 類別完成多項式的加法、乘法、代入 x，以及輸入輸出。

### 解題策略

1. Term 儲存係數 coef 和指數 exp。
2. Polynomial 使用動態陣列，初始容量為 10，容量不夠就加倍。
3. newTerm 把新的一項放到陣列最後。
4. Add 先放入第一個多項式，再逐項搜尋第二個多項式的指數，指數相同就把係數相加。
5. Mult 把兩個多項式的各項配對，係數相乘、指數相加，再合併相同指數。
6. Eval 使用 pow 計算每項的值，再加總。
7. >> 讀入項數、係數和指數；<< 輸出各項。

## 程式實作


```cpp
#include <iostream>
#include <cmath>
using namespace std;

class Term
{
public:
	float coef;
	int exp;
};

class Polynomial
{
private:
	Term* termArray;
	int capacity;
	int terms;

	void newTerm(float coef, int exp)
	{
		if (terms >= capacity)
		{
			capacity *= 2;

			Term* temp = new Term[capacity];

			for (int i = 0; i < terms; i++)
			{
				temp[i] = termArray[i];
			}

			delete[] termArray;
			termArray = temp;
		}

		termArray[terms].coef = coef;
		termArray[terms].exp = exp;
		terms++;
	}

public:
	Polynomial(int cap = 10)
	{
		capacity = cap;
		terms = 0;
		termArray = new Term[capacity];
	}

	~Polynomial()
	{
		delete[] termArray;
	}

	friend istream& operator>>(istream& in, Polynomial& p);
	friend ostream& operator<<(ostream& out, const Polynomial& p);

	Polynomial Add(const Polynomial& poly);

	Polynomial Mult(const Polynomial& poly);

	float Eval(float x);
};

istream& operator>>(istream& in, Polynomial& p)
{
	int n;

	cout << "請輸入項數: ";
	in >> n;

	p.terms = 0;

	cout << "請輸入係數與指數\n";

	for (int i = 0; i < n; i++)
	{
		float c;
		int e;

		in >> c >> e;

		p.newTerm(c, e);
	}

	return in;
}

ostream& operator<<(ostream& out, const Polynomial& p)
{
	if (p.terms == 0)
	{
		out << "0";
		return out;
	}

	for (int i = 0; i < p.terms; i++)
	{
		float coef = p.termArray[i].coef;
		int exp = p.termArray[i].exp;

		if (i > 0 && coef >= 0)
			out << " + ";

		if (coef < 0)
			out << " - ";

		if (coef < 0)
			coef = -coef;

		out << coef;

		if (exp > 0)
		{
			out << "x";

			if (exp > 1)
				out << "^" << exp;
		}
	}

	return out;
}

Polynomial Polynomial::Add(const Polynomial& poly)
{
	Polynomial result;

	for (int i = 0; i < terms; i++)
	{
		result.newTerm(
			termArray[i].coef,
			termArray[i].exp
		);
	}

	for (int i = 0; i < poly.terms; i++)
	{
		bool found = false;

		for (int j = 0; j < result.terms; j++)
		{
			if (result.termArray[j].exp ==
				poly.termArray[i].exp)
			{
				result.termArray[j].coef +=
					poly.termArray[i].coef;

				found = true;
				break;
			}
		}

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

Polynomial Polynomial::Mult(const Polynomial& poly)
{
	Polynomial result;

	for (int i = 0; i < terms; i++)
	{
		for (int j = 0; j < poly.terms; j++)
		{
			float newCoef =
				termArray[i].coef *
				poly.termArray[j].coef;

			int newExp =
				termArray[i].exp +
				poly.termArray[j].exp;

			bool found = false;

			for (int k = 0; k < result.terms; k++)
			{
				if (result.termArray[k].exp == newExp)
				{
					result.termArray[k].coef += newCoef;
					found = true;
					break;
				}
			}

			if (!found)
			{
				result.newTerm(newCoef, newExp);
			}
		}
	}

	return result;
}

float Polynomial::Eval(float x)
{
	float sum = 0;

	for (int i = 0; i < terms; i++)
	{
		sum += termArray[i].coef *
			pow(x, termArray[i].exp);
	}

	return sum;
}

int main()
{
	Polynomial A;
	Polynomial B;

	cout << "輸入 Polynomial A\n";
	cin >> A;

	cout << "\n輸入 Polynomial B\n";
	cin >> B;

	cout << "\nA(x) = " << A << endl;
	cout << "B(x) = " << B << endl;

	Polynomial C = A.Add(B);
	cout << "\nA + B = " << C << endl;

	Polynomial D = A.Mult(B);
	cout << "A * B = " << D << endl;

	float x;

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
| Add | O(n + m(n+m)) | O(n+m) |
| Mult | 最壞 O((nm)^2) | O(nm+1) |
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

實際輸出：

```text
A(2) = 17
B(2) = 1
```


## 心得討論

加法和乘法都要注意相同指數的項，找到後把係數加進去，找不到才新增一項。用迴圈搜尋比較容易理解，但是項數增加後會比較慢。

這個版本沒有整理輸入中重複的指數，也沒有移除零係數項。輸入時應使用不同指數、非零係數，容量需大於 0，且數值不要超出型別範圍。

目前沒有另外寫複製建構子和指定運算子，複製物件時可能共用同一個陣列。這次範例執行成功，但不能保證其他編譯設定下也沒有重複釋放的問題，後續可補上深層複製。
