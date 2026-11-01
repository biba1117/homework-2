#include "header.h"

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
    cout << "A(" << x << ") = " << A.Eval(x) << endl;
    cout << "B(" << x << ") = " << B.Eval(x) << endl;
    return 0;
}
