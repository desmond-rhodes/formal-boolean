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
	size_t size;
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

std::unordered_set<unsigned char> const ignore {' ', '\t', '\n'};

bool test_var(std::string const&);
opr_t test_opr(std::string const&, size_t, size_t&);
mod_t test_mod(std::string const&, size_t, size_t&);
bool test_ignore(unsigned char);

std::vector<token_t> token_parse(std::string const&);
bool token_validate(std::vector<token_t> const&, std::string const&);

void error_draw(std::string const&, size_t, size_t);

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

bool test_var(std::string const& s) {
	for (auto i : s)
		if (valid_var.find(i) == valid_var.end())
			return false;
	return true;
}

opr_t test_opr(std::string const& s, size_t p, size_t& len) {
	for (auto const& i : valid_opr)
		if (!s.compare(p, i.first.size(), i.first)) {
			len = i.first.size();
			return i.second;
		}
	return opr_t::unk;
}

mod_t test_mod(std::string const& s, size_t p, size_t& len) {
	for (auto const& i : valid_mod)
		if (!s.compare(p, i.first.size(), i.first)) {
			len = i.first.size();
			return i.second;
		}
	return mod_t::unk;
}

bool test_ignore(unsigned char c) {
	return ignore.find(c) != ignore.end();
}

std::vector<token_t> token_parse(std::string const& s) {
	std::vector<token_t> token;

	std::string tmp {""};
	for (size_t i {0}; i < s.size(); ++i) {

		auto const ignore {test_ignore(s[i])};
		size_t opr_len;
		auto const opr {test_opr(s, i, opr_len)};
		size_t mod_len;
		auto const mod {test_mod(s, i, mod_len)};

		if (ignore || opr != opr_t::unk || mod != mod_t::unk || i == s.size()-1) {
			if (!ignore && opr == opr_t::unk && mod == mod_t::unk) {
				tmp += s[i];
				++i;
			}
			if (tmp.size()) {
				token_t t;
				if (test_var(tmp))
					t._class = token_c::var;
				else
					t._class = token_c::unk;
				t.loc = i - tmp.size();
				t.size = tmp.size();
				t.str = std::move(tmp);
				tmp = "";
				token.push_back(std::move(t));
			}
			if (opr != opr_t::unk) {
				token_t t;
				t._class = token_c::opr;
				t.loc = i;
				t.size = opr_len;
				t.opr = opr;
				i = i + opr_len - 1;
				token.push_back(std::move(t));
			}
			if (mod != mod_t::unk) {
				token_t t;
				t._class = token_c::mod;
				t.loc = i;
				t.size = mod_len;
				t.mod = mod;
				i = i + mod_len - 1;
				token.push_back(std::move(t));
			}
		}
		else
			tmp += s[i];

	}

	return token;
}

bool token_validate(std::vector<token_t> const& t, std::string const& s) {
	size_t error {0};

	for (auto const& i : t) {
		error_draw(s, i.loc, i.loc+i.size);
		std::cout << '\n';
	}

	return !error;
}

void error_draw(std::string const& s, size_t start, size_t end) {
	std::cerr << s << '\n';
	for (size_t i {0}; i < end; ++i)
		std::cout << ((i < start) ? ' ' : '"');
	std::cout << '\n';
}
