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

