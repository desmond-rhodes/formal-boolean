#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <utility>

enum class opr_t { conjunction, inclusive_disjunction, exclusive_disjunction, implication, equivalence, unk };
enum class mod_t { start_precedent, end_precedent, negation, unk };

enum class token_c { var, opr, mod, unk };

struct token_t {
	token_c _class;
	size_t loc;
	std::string str;
	opr_t opr;
	mod_t mod;
};

std::unordered_set<unsigned char> const valid_var {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', '-', '.'};

std::vector<std::pair<std::string, opr_t>> const valid_opr {
	{"&",   opr_t::conjunction},
	{"|",   opr_t::inclusive_disjunction},
	{"^",   opr_t::exclusive_disjunction},
	{"=>",  opr_t::implication},
	{"<=>", opr_t::equivalence}
};

std::vector<std::pair<std::string, mod_t>> const valid_mod {
	{"(", mod_t::start_precedent},
	{")", mod_t::end_precedent},
	{"!", mod_t::negation}
};

std::unordered_set<unsigned char> const whitespace {' ', '\t', '\n'};

std::vector<token_t> token_parse(std::string const&);
bool token_validate(std::vector<token_t> const&, std::string const&);

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::vector<std::string> args(argv+1, argv+argc);

	if (args.size() < 1) {
		std::cerr
			<< "usage:\n"
			<< "\t()  for precedent\n"
			<< "\t!   for negation\n"
			<< "\t&   for conjunction\n"
			<< "\t|   for inclusive_disjunction\n"
			<< "\t^   for exclusive_disjunction\n"
			<< "\t=>  for implication\n"
			<< "\t<=> for equivalence\n"
		;
		return -1;
	}

	std::vector<token_t> token {token_parse(args.back())};
	if (!token_validate(token, args.back()))
		return -1;

	std::cout << "Hello, world!\n";
	return 0;
}

std::vector<token_t> token_parse(std::string const& s) {
	std::vector<token_t> token;

	std::string tmp {""};
	for (size_t i {0}; i < s.size(); ++i) {

		auto ignore {whitespace.find(s[i]) != whitespace.end()};

		auto opr_shift {i};
		auto opr {opr_t::unk};
		for (auto const& j : valid_opr)
			if (!s.compare(i, j.first.size(), j.first)) {
				opr_shift += j.first.size() - 1;
				opr = j.second;
			}

		auto mod_shift {i};
		auto mod {mod_t::unk};
		for (auto const& j : valid_mod)
			if (!s.compare(i, j.first.size(), j.first)) {
				mod_shift += j.first.size() - 1;
				mod = j.second;
			}

		if (ignore || opr != opr_t::unk || mod != mod_t::unk || i == s.size()-1) {
			if (!ignore && opr == opr_t::unk && mod == mod_t::unk) {
				tmp += s[i];
				++i;
			}
			if (tmp.size()) {
				token_t t;
				t._class = token_c::var;
				for (auto j : tmp)
					if (valid_var.find(j) == valid_var.end()) {
						t._class = token_c::unk;
						break;
					}
				t.loc = i - tmp.size();
				t.str = std::move(tmp);
				token.push_back(std::move(t));
				tmp = "";
			}
			if (opr != opr_t::unk) {
				token_t t;
				t._class = token_c::opr;
				t.loc = i;
				t.opr = opr;
				token.push_back(std::move(t));
				i = opr_shift;
			}
			if (mod != mod_t::unk) {
				token_t t;
				t._class = token_c::mod;
				t.loc = i;
				t.mod = mod;
				token.push_back(std::move(t));
				i = mod_shift;
			}
		}
		else
			tmp += s[i];

	}

	return token;
}

void error_draw(std::string const& s, token_t const& t) {
	std::cerr << s << '\n';
	for (size_t i {0}; i < t.loc; ++i)
		std::cerr << ' ';
	std::cerr << "\"\n";
	if (t._class == token_c::var || t._class == token_c::unk)
		std::cerr << t.str << '\n';
}

bool token_validate(std::vector<token_t> const& t, std::string const& s) {
	size_t error {0};

	for (auto const& i : t) {
		error_draw(s, i);
		std::cout << '\n';
	}

	return !error;
}
