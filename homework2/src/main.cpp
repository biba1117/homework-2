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

