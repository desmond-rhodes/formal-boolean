#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <utility>

enum class opr_t { conjunction, inclusive_disjunction, exclusive_disjunction, implication, equivalence, negation, unk };
enum class mod_t { start_precedence, end_precedence, negation, unk };

enum class token_c { var, opr, mod, unk };

struct token_t {
	token_c _class;
	size_t beg;
	size_t end;
	size_t var;
	opr_t opr;
	mod_t mod;
};

struct stack_t {
	token_c _class;
	size_t var;
	opr_t opr;
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

std::unordered_map<opr_t, int> const pre_opr {
	{opr_t::conjunction,           2},
	{opr_t::inclusive_disjunction, 2},
	{opr_t::exclusive_disjunction, 2},
	{opr_t::implication,           1},
	{opr_t::equivalence,           1},
	{opr_t::negation,              3}
};

std::pair<std::vector<token_t>, std::vector<std::string>> token_parse(std::string const&);
size_t token_validate(std::vector<token_t> const&, std::string const&);

std::vector<stack_t> stack_parse(std::vector<token_t> const&);

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

	auto const [token, var] {token_parse(args.back())};
	if (token_validate(token, args.back()))
		return -1;

	auto const stack {stack_parse(token)};

	for (auto i : stack) {
		switch (i._class) {
		case token_c::var:
			std::cout << var.at(i.var);
			break;
		case token_c::opr:
			switch (i.opr) {
			case opr_t::conjunction:           std::cout << "&";   break;
			case opr_t::inclusive_disjunction: std::cout << "|";   break;
			case opr_t::exclusive_disjunction: std::cout << "^";   break;
			case opr_t::implication:           std::cout << "=>";  break;
			case opr_t::equivalence:           std::cout << "<=>"; break;
			case opr_t::negation:              std::cout << "!";   break;
			}
			break;
		}
		std::cout << ' ';
	}
	std::cout << '\n';

	std::cout << "Hello, world!\n";
	return 0;
}

std::pair<std::vector<token_t>, std::vector<std::string>> token_parse(std::string const& s) {
	std::vector<token_t> token;
	std::vector<std::string> var;

	std::unordered_map<std::string, size_t> hash;
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
				if (t._class == token_c::var)
					if (hash.find(tmp) != hash.end())
						t.var = hash.at(tmp);
					else {
						t.var = var.size();
						hash[tmp] = t.var;
						var.push_back(tmp);
					}
				t.beg = i - tmp.size();
				t.end = i -1;
				token.push_back(t);
				tmp = "";
			}
			if (opr != opr_t::unk) {
				token_t t;
				t._class = token_c::opr;
				t.beg = i;
				t.end = opr_shift;
				t.opr = opr;
				token.push_back(t);
				i = opr_shift;
			}
			if (mod != mod_t::unk) {
				token_t t;
				t._class = token_c::mod;
				t.beg = i;
				t.end = mod_shift;
				t.mod = mod;
				token.push_back(t);
				i = mod_shift;
			}
		}
		else
			tmp += s[i];

	}

	return {token, var};
}

void error_draw(bool& is_drawn, size_t& error, std::string const& s, size_t beg, size_t end) {
	if (!is_drawn) {
		std::cerr << s << '\n';
		for (size_t i {0}; i < beg; ++i)
			std::cerr << ' ';
		for (size_t i {beg}; i <= end; ++i)
			std::cerr << '~';
		std::cerr << '\n';
		is_drawn = true;
	}
	++error;
}

size_t token_validate(std::vector<token_t> const& t, std::string const& s) {
	size_t error {0};

	size_t opening_bracket {0};
	size_t closing_bracket {0};

	for (size_t i {0}; i < t.size(); ++i) {
		bool is_drawn {false};

		if (t[i]._class == token_c::unk) {
			error_draw(is_drawn, error, s, t[i].beg, t[i].end);
			std::cerr << '[' << t[i].beg+1 << "] unknown operator or variable.\n";
		}

		if (t[i]._class == token_c::var || (t[i]._class == token_c::mod && t[i].mod != mod_t::end_precedence))
			if (i > 0 && !(t[i-1]._class == token_c::opr || (t[i-1]._class == token_c::mod && t[i-1].mod != mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].beg, t[i].end);
				std::cerr << '[' << t[i].beg+1 << "] expected an operator beforehand.\n";
			}

		if (t[i]._class == token_c::var || (t[i]._class == token_c::mod && t[i].mod == mod_t::end_precedence))
			if (i < t.size()-1 && !(t[i+1]._class == token_c::opr || (t[i+1]._class == token_c::mod && t[i+1].mod == mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].beg, t[i].end);
				std::cerr << '[' << t[i].beg+1 << "] expected an operator afterwards.\n";
			}

		if (t[i]._class == token_c::opr || (t[i]._class == token_c::mod && t[i].mod == mod_t::end_precedence))
			if (i == 0 || !(t[i-1]._class == token_c::var || (t[i-1]._class == token_c::mod && t[i-1].mod == mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].beg, t[i].end);
				std::cerr << '[' << t[i].beg+1 << "] expected a variable name beforehand.\n";
			}

		if (t[i]._class == token_c::opr || (t[i]._class == token_c::mod && t[i].mod != mod_t::end_precedence))
			if (i == t.size()-1 || !(t[i+1]._class == token_c::var || (t[i+1]._class == token_c::mod && t[i+1].mod != mod_t::end_precedence))) {
				error_draw(is_drawn, error, s, t[i].beg, t[i].end);
				std::cerr << '[' << t[i].beg+1 << "] expected a variable name afterwards.\n";
			}

		if (t[i]._class == token_c::mod) {
			if (t[i].mod == mod_t::start_precedence)
				++opening_bracket;
			if (t[i].mod == mod_t::end_precedence)
				++closing_bracket;
		}

		if (is_drawn)
			std::cerr << '\n';
	}

	if (closing_bracket > opening_bracket) {
		auto const bracket {closing_bracket-opening_bracket};
		++error;
		for (size_t i {0}; i < bracket; ++i) std::cerr << ' '; std::cerr << s << '\n';
		for (size_t i {0}; i < bracket; ++i) std::cerr << '~'; std::cerr << '\n';
		std::cerr << "[0] missing " << bracket << " matching opening bracket" << ((bracket == 1) ? "\n" : "s\n") << '\n';
	}

	if (opening_bracket > closing_bracket) {
		auto const bracket {opening_bracket-closing_bracket};
		++error;
		std::cerr << s << '\n';
		for (size_t i {0}; i < s.size(); ++i) std::cerr << ' ';
		for (size_t i {0}; i < bracket; ++i) std::cerr << '~';
		std::cerr << '\n';
		std::cerr << '[' << s.size()+1 << "] missing " << bracket << " matching closing bracket" << ((bracket == 1) ? "\n" : "s\n") << '\n';
	}

	if (error)
		std::cerr << error << " syntax error" << ((error == 1) ? " " : "s ") << "were encountered.\n";
	return error;
}

std::vector<stack_t> stack_parse(std::vector<token_t> const& token) {
	std::vector<stack_t> stack;

	std::vector<stack_t> o;
	std::vector<size_t> p;
	size_t n {0};
	for (auto t : token) {
		stack_t s;
		s._class = t._class;
		switch (s._class) {
		case token_c::var:
			s.var = t.var;
			stack.push_back(s);
			break;
		case token_c::opr:
			s.opr = t.opr;
			break;
		case token_c::mod:
			switch (t.mod) {
			case mod_t::start_precedence:
				++n;
				break;
			case mod_t::end_precedence:
				--n;
				break;
			case mod_t::negation:
				s._class = token_c::opr;
				s.opr = opr_t::negation;
				break;
			}
			break;
		}
		if (s._class == token_c::opr) {
			/* Why are things from token_c::mod always badly behaved? */
			if (!(o.size() && s.opr == opr_t::negation && o.back().opr == opr_t::negation))
				while (o.size() && (p.back() > n || (p.back() == n && pre_opr.at(o.back().opr) >= pre_opr.at(s.opr)))) {
					stack.push_back(o.back());
					o.pop_back();
					p.pop_back();
				}
			o.push_back(s);
			p.push_back(n);
		}
	}
	while (o.size()) {
		stack.push_back(o.back());
		o.pop_back();
	}

	return stack;
}
