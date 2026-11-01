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

