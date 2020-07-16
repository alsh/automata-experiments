#include <vector>
#include <map>
#include <tuple>
#include <iostream>
#include <functional>
#include <numeric>
#include <string>

class rule
{
    typedef std::pair<char, rule*> transition_t;
    typedef std::map<transition_t::first_type, transition_t::second_type> transition_set_t;

    transition_set_t transition_set;

    rule(const transition_set_t& _init_set) : transition_set(_init_set) {}
    rule(transition_set_t&& _init_set) : transition_set(_init_set) {}
public:
    struct error_unaccept {};

    rule() : transition_set() {}
    rule(char _c) : transition_set{std::make_pair(_c, static_cast<class rule*>(nullptr))} {}
    rule(char _c, rule& _r) : transition_set{std::make_pair(_c, &_r)} {}

    rule(const rule& _c) = default;
    rule(rule&& _c) = default;
    rule& operator=(const rule& _rhs) = default;

    rule& operator+=(const rule& _rhs)
    {
        transition_set.insert(std::begin(_rhs.transition_set),
                              std::end(_rhs.transition_set));
        return *this;
    }

    rule* transit(const char _c)
    {
        transition_set_t::iterator i = transition_set.find(_c);
        if (i == std::end(transition_set))
            throw error_unaccept();
        else
            return i->second;
    }

    template<typename _T1, typename _T2>
    friend rule operator+(_T1 _a, _T2 _b);
};

template<typename _T1, typename _T2>
rule operator+(_T1 _a, _T2 _b)
{
    rule::transition_set_t _new_transition_set;
    if (std::is_rvalue_reference<_T1>::value)
    {
        _new_transition_set = _a.transition_set;
        _new_transition_set.insert(std::begin(_b.transition_set),
                                   std::end(_b.transition_set));
    }
    else
    {
        _new_transition_set = _b.transition_set;
        _new_transition_set.insert(std::begin(_a.transition_set),
                                   std::end(_a.transition_set));
    }
    return rule(std::move(_new_transition_set));
}

int main()
{
    rule S;
    S += rule('a', S);
    S += rule('b');

    rule* init = &S;

    std::string input = "aaab";

    auto final = std::accumulate(std::begin(input), std::end(input), init,
                                 [](rule* _r, char _c) { return _r->transit(_c); }
        );

    if (final == nullptr)
        std::cout << "Success!" << std::endl;

    return 0;
}
