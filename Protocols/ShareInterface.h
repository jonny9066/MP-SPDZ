/*
 * ShareInterface.h
 *
 */

#ifndef PROTOCOLS_SHAREINTERFACE_H_
#define PROTOCOLS_SHAREINTERFACE_H_

#include <vector>
#include <typeinfo>

using namespace std;

class Player;

namespace GC
{
class NoShare;
}

class ShareInterface
{
public:
    virtual ~ShareInterface(){};//@TZ virtual destrutor for casting, needed for a test

    typedef GC::NoShare part_type;
    typedef GC::NoShare bit_type;

    static const bool needs_ot = false;
    static const bool expensive = false;
    static const bool expensive_triples = false;

    static const int default_length = 1;

    static string type_short() { return "undef"; }

    template<class T, class U>
    static void split(vector<U>, vector<int>, int, T*, int,
            typename U::Protocol&)
    { throw runtime_error("split not implemented"); }

    template<class T>
    static void shrsi(T&, const Instruction&)
    { throw runtime_error("shrsi not implemented"); }

    static bool get_rec_factor(int, int) { return false; }

    template<class T>
    static void read_or_generate_mac_key(const string&, const Player&, T&) {}

    template<class T, class U>
    static void generate_mac_key(T&, U&) {}

    // template<class T>
    // const T& get_share_tz(T* dummy) const  { (void)dummy; throw runtime_error("get_share_tz not implemented, T is "<<typeid(T).name()); }//@TZ for testing
};

#endif /* PROTOCOLS_SHAREINTERFACE_H_ */
