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
