#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <utility>

enum class opr_t { conjunction, inclusive_disjunction, exclusive_disjunction, implication, equivalence, unk };
enum class mod_t { start_precedence, end_precedence, negation, unk };

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
	{"(", mod_t::start_precedence},
	{")", mod_t::end_precedence},
	{"!", mod_t::negation}
};

std::unordered_set<unsigned char> const whitespace {' ', '\t', '\n'};

std::vector<token_t> token_parse(std::string const&);
size_t token_validate(std::vector<token_t> const&, std::string const&);

std::unordered_map<opr_t, std::string> const opr_name {
	{opr_t::conjunction,           "conjunction"},
	{opr_t::inclusive_disjunction, "inclusive disjunction"},
	{opr_t::exclusive_disjunction, "exclusive disjunction"},
	{opr_t::implication,           "implication"},
	{opr_t::equivalence,           "equivalence"}
};

std::unordered_map<mod_t, std::string> const mod_name {
	{mod_t::start_precedence, "opening parentheses"},
	{mod_t::end_precedence  , "closing parentheses"},
	{mod_t::negation       , "negation"}
};

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::vector<std::string> args(argv+1, argv+argc);

	if (args.size() < 1) {
		std::cerr
			<< "usage:\n"
			<< "\t()  for precedence\n"
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
	if (token_validate(token, args.back()))
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

void error_draw(bool& is_drawn, size_t& error, std::string const& s, size_t loc) {
	if (!is_drawn) {
		std::cerr << s << '\n';
		for (size_t i {0}; i < loc; ++i)
			std::cerr << ' ';
		std::cerr << "\"\n";
		is_drawn = true;
	}
	++error;
}

size_t token_validate(std::vector<token_t> const& t, std::string const& s) {
	size_t error {0};

	for (size_t i {0}; i < t.size(); ++i) {
		bool is_drawn {false};
		switch (t[i]._class) {

		case token_c::unk:
			error_draw(is_drawn, error, s, t[i].loc);
			std::cerr << '[' << t[i].loc+1 << "] unknown operator or variable: " << t[i].str << '\n';
			break;

		case token_c::var:
			if (i > 0 && !(t[i-1]._class == token_c::opr || (t[i-1]._class == token_c::mod && t[i-1].mod != mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].loc);
				std::cerr << '[' << t[i].loc+1 << "] expected an operator before variable " << t[i].str << '\n';
			}
			if (i < t.size()-1 && !(t[i+1]._class == token_c::opr || (t[i+1]._class == token_c::mod && t[i+1].mod == mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].loc);
				std::cerr << '[' << t[i].loc+1 << "] expected an operator after variable " << t[i].str << '\n';
			}
			break;

		case token_c::opr:
			if (i == 0 || !(t[i-1]._class == token_c::var || (t[i-1]._class == token_c::mod && t[i-1].mod == mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].loc);
				std::cerr << '[' << t[i].loc+1 << "] expected a variable name before the operator " << opr_name.at(t[i].opr) << '\n';
			}
			if (i == t.size()-1 || !(t[i+1]._class == token_c::var || (t[i+1]._class == token_c::mod && t[i+1].mod != mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].loc);
				std::cerr << '[' << t[i].loc+1 << "] expected a variable name after the operator " << opr_name.at(t[i].opr) << '\n';
			}
			break;

		case token_c::mod:
			switch (t[i].mod) {

			case mod_t::start_precedence:
			case mod_t::negation:
				if (i > 0 && !(t[i-1]._class == token_c::opr || (t[i-1]._class == token_c::mod && t[i-1].mod != mod_t::end_precedence))) {
					error_draw(is_drawn, error, s, t[i].loc);
					std::cerr << '[' << t[i].loc+1 << "] expected an operator before " << mod_name.at(t[i].mod) << '\n';
				}
				if (i == t.size()-1 || !(t[i+1]._class == token_c::var || (t[i+1]._class == token_c::mod && t[i+1].mod != mod_t::end_precedence))) {
					error_draw(is_drawn, error, s, t[i].loc);
					std::cerr << '[' << t[i].loc+1 << "] expected a variable after " << mod_name.at(t[i].mod) << '\n';
				}
				break;

			case mod_t::end_precedence:
				if (i == 0 || !(t[i-1]._class == token_c::var || (t[i-1]._class == token_c::mod && t[i-1].mod == mod_t::end_precedence))) {
					error_draw(is_drawn, error, s, t[i].loc);
					std::cerr << '[' << t[i].loc+1 << "] expected a variable before " << mod_name.at(t[i].mod) << '\n';
				}
				if (i < t.size()-1 && !(t[i+1]._class == token_c::opr || (t[i+1]._class == token_c::mod && t[i+1].mod == mod_t::end_precedence))) {
					error_draw(is_drawn, error, s, t[i].loc);
					std::cerr << '[' << t[i].loc+1 << "] expected an operator after " << mod_name.at(t[i].mod) << '\n';
				}
				break;

			}
			break;

		}
		if (is_drawn)
			std::cerr << '\n';
	}

	if (error)
		std::cerr << error << " syntax error" << ((error == 1) ? " " : "s ") << "were encountered.\n";
	return error;
}
