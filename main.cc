#include <iostream>
#include <vector>
#include <string>

void parse_sanity();
void parse_stack();
void evaluate_matrix();

bool negation(bool);
bool conjunction(bool, bool);
bool disjunction_inclusive(bool, bool);
bool disjunction_exclusive(bool, bool);
bool implication(bool, bool);
bool equivalence(bool, bool);

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::cout << "Hello, world!\n";
	return 0;
}

void parse_sanity() {
}

void parse_stack() {
}

void evaluate_matrix() {
}

bool negation(bool a) {
	return !a;
}

bool conjunction(bool a, bool b) {
	return a && b;
}

bool disjunction_inclusive(bool a, bool b) {
	return a || b;
}

bool disjunction_exclusive(bool a, bool b) {
	return a ^ b;
}

bool implication(bool a, bool b) {
	return disjunction_inclusive(negation(a), b);
}

bool equivalence(bool a, bool b) {
	return conjunction(implication(a, b), implication(b, a));
}
