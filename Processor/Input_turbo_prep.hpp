// included in Input.hpp

#ifndef PROCESSOR_INPUT_HPP_
#define PROCESSOR_INPUT_HPP_

#include <typeinfo>

#include "Input.h"
#include "Processor.h"

#include "IntInput.h"
#include "FixInput.h"
#include "FloatInput.h"

#include "IntInput.hpp"

template<class T>
InputBase<T>::InputBase(ArithmeticProcessor* proc) :
        P(0), values_input(0)
{
    if (proc)
        buffer.setup(&proc->private_input, -1, proc->private_input_filename);
}

template<class T>
InputBase<T>::InputBase(SubProcessor<T>* proc) :
        InputBase(proc ? proc->Proc : 0)
{
}

template<class T>
Input<T>::Input(SubProcessor<T>& proc) :
        Input(proc, proc.MC)
{
}

template<class T>
Input<T>::Input(SubProcessor<T>& proc, MAC_Check& mc) :
        InputBase<T>(proc.Proc), proc(&proc), MC(mc), prep(proc.DataF), P(proc.P),
        shares(proc.P.num_players())
{
}

template<class T>
Input<T>::Input(SubProcessor<T>* proc, Player& P) :
        InputBase<T>(proc->Proc), proc(proc), MC(proc->MC), prep(proc->DataF), P(
                proc->P), shares(P.num_players())
{
    assert (proc != 0);
}

template<class T>
Input<T>::Input(MAC_Check& MC, Preprocessing<T>& prep, Player& P) :
        proc(0), MC(MC), prep(prep), P(P), shares(P.num_players())
{
}

template<class T>
InputBase<T>::~InputBase()
{
#ifdef VERBOSE
    if (timer.elapsed() > 0)
        cerr << T::type_string() << " inputs: " << timer.elapsed() << endl;
#endif
}

template<class T>
void Input<T>::reset(int player)
{
    InputBase<T>::reset(player);
    shares[player].clear();
    rand.clear(); //@TZ
}

template<class T>
void InputBase<T>::reset(int player)
{
    os.resize(max(os.size(), player + 1UL));
    os[player].reset_write_head();
}

template<class T>
void InputBase<T>::reset_all(Player& P)
{
    this->P = &P;
    os.resize(P.num_players());
    for (int i = 0; i < P.num_players(); i++)
        reset(i);
}

template<class T>
void Input<T>::add_mine(const open_type& input, int n_bits)
{
    (void) input;
    (void) n_bits;
    int player = P.my_num();
    // push back new shares and send them to preprocessing to be initialized
    shares[player].push_back({});
    rand.push_back({}); // @TZ can I do that?
    T& share = shares[player].back();
    open_type& ra = rand.back();
    DEBUG_IN("adding input: "<<input);
    prep.get_input(share, ra, player);
    DEBUG_IN("random val, share: "<<ra<<", "<<share);

    // uShare* temp_ptr1 = dynamic_cast<uShare*>(&share);
    // DEBUG_IN("generating random val: "<<ra);
    // DEBUG_IN("my share of random val val: "<<temp_ptr1->get_share());

    this->values_input++;
}

template<class T>
void Input<T>::add_other(int player)
{
    open_type t;
    shares.at(player).push_back({});
    // gets share of random value
    prep.get_input(shares[player].back(), t, player);
}

template<class T>
T Input<T>::finalize_mine()
{
    // returns last share and advances pointer
    return shares[P.my_num()].next();
}

template<class T>
typename T::open_type Input<T>::finalize_mine_rand()
{
    // returns last random open val and advances pointer
    return rand.next();
}

// compute own share of other's input
template<class T>
void Input<T>::finalize_other(int player, T& target,
        octetStream& o, int n_bits)
{
    (void) n_bits;(void) o; 
    // gets share [[r_i]] of other player
    target = shares[player].next();
    // uShare* temp_ptr1 = dynamic_cast<uShare*>(&target);
    // DEBUG_IN("share from other's rand val (finalize_other): "<<temp_ptr1->get_share());
}

// need this function as caller function cannot access P
template<class T>
tuple<T, typename T::open_type, bool> InputBase<T>::finalize_tzprep(int player, int n_bits)
{

    if (player == P->my_num())
        return {finalize_mine(), finalize_mine_rand(), true};
    else
    {
        T res;
        finalize_other(player, res, os[player], n_bits);
        return {res, NULL,false}; // bad programming
    }
}


template<class T>
template<class U>
void InputBase<T>::prepare(SubProcessor<T>& Proc, int player, const int* params,
        int size)
{
    auto& input = Proc.input;
    assert(Proc.Proc != 0);
    if (player == Proc.P.my_num())
    {
        for (int j = 0; j < size; j++)
        {
            U tuple = Proc.Proc->template get_input<U>(Proc.Proc->use_stdin(),
                    params);
            for (auto x : tuple.items)
                input.add_mine(x);
        }
    }
    else
    {
        for (int j = 0; j < U::N_DEST * size; j++)
            input.add_other(player);
    }
}


template<class T>
template<class U>
void InputBase<T>::finalize(SubProcessor<T>& Proc, int player, const int* dest,
        int size)
{
    auto& input = Proc.input;
    for (int k = 0; k < size; k++)
        for (int j = 0; j < U::N_DEST; j++){
            // put shares of the input in their place
            tuple<T, typename T::open_type, bool> so = input.finalize_tzprep(player);
            if (get<2>(so) == true){ // if my share save the open random element
                Proc.get_E_ref(dest[j] + k) =  get<1>(so);
                DEBUG_IN("E("<<dest[j] + k<<") <-"<<get<1>(so));
            }
            Proc.get_S_ref(dest[j] + k) =  get<0>(so);// save share
            DEBUG_IN("S("<<dest[j] + k<<") <-"<<get<0>(so));

            // T& svshr = get<0>(so);
            // uShare* temp_ptr1 = dynamic_cast<uShare*>(&svshr);
            // DEBUG_IN("saving to S register: "<<temp_ptr1->get_share());
        }            
}


template<class T>
int InputBase<T>::get_player(SubProcessor<T>& Proc, int arg, bool player_from_reg)
{
   if (player_from_reg)
   {
       assert(Proc.Proc);
       auto res = Proc.Proc->read_Ci(arg);
       if (res >= Proc.P.num_players())
           throw runtime_error("player id too large: " + to_string(res));
       return res;
   }
   else
       return arg;
}
template<class T>
void InputBase<T>::input_mixed(SubProcessor<T>& Proc, const vector<int>& args,
    int size, bool player_from_reg)
{
    DEBUG_IN("getting input (input_mixed)");
    // Proc.print_registers();
    auto& input = Proc.input;
    input.reset_all(Proc.P);
    int last_type = -1;

    for (size_t i = 0; i < args.size();)
    {
        int n_arg_tuple;
        int type = args[i];
        int player;
        switch (type)
        {
#undef X
// prepare activates add_mine and add_other for me and other players resp.
#define X(U) \
        case U::TYPE: \
            n_arg_tuple = U::N_DEST + U::N_PARAM + 2; \
            player = get_player(Proc, args[i + n_arg_tuple - 1], player_from_reg); \
            if (type != last_type and Proc.Proc and Proc.Proc->use_stdin()) \
                cout << "Please input " << U::NAME << "s:" << endl; \
            prepare<U>(Proc, player, &args[i + U::N_DEST + 1], size); \
            break;
        X(IntInput<typename T::clear>) X(FixInput) X(FloatInput)
#undef X
        default:
            throw unknown_input_type(type);
        }
        i += n_arg_tuple;
        last_type = type;
    }
    
    // @TZ prep deprecated
    // input.exchange();

    for (size_t i = 0; i < args.size();)
    {
        int n_arg_tuple;
        int type = args[i];
        int player;
        switch (type)
        {
#define X(U) \
        case U::TYPE: \
            n_arg_tuple = U::N_DEST + U::N_PARAM + 2; \
            player = get_player(Proc, args[i + n_arg_tuple - 1], player_from_reg); \
            finalize<U>(Proc, player, &args[i + 1], size); \
            break;
        X(IntInput<typename T::clear>) X(FixInput) X(FloatInput)
#undef X
        default:
            throw unknown_input_type(type);
        }
        i += n_arg_tuple;
    }
#ifdef TZDEBUG
    Proc.print_registers();
#endif
    DEBUG_IN("finished getting input (input_mixed)");
}

//@TZ not used in TZ prep cleanup later



template<class T>
T InputBase<T>::finalize(int player, int n_bits)
{
    throw runtime_error("finalize deprecated in TZ prep");
    if (player == P->my_num())
        return finalize_mine();
    else
    {
        T res;
        finalize_other(player, res, os[player], n_bits);
        return res;
    }
}

template<class T>
void InputBase<T>::add_from_all(const clear& input)
{
    
    throw runtime_error("add_from_all deprecated TZ prep");
    for (int i = 0; i < P->num_players(); i++)
        if (i == P->my_num())
            add_mine(input);
        else
            add_other(i);
}


template<class T>
void Input<T>::send_mine()
{
    throw runtime_error("send_mine deprecated TZ prep");
    // P.send_all(this->os[P.my_num()]); //@RZ not used in prep
}

template<class T>
void InputBase<T>::exchange()
{
    throw runtime_error("exchange deprecated TZ prep");
    // for (int i = 0; i < P->num_players(); i++)
    //     if (i == P->my_num())
    //         send_mine();
    //     else
    //         P->receive_player(i, os[i]);
}


template<class T>
template<class U>
void InputBase<T>::input(SubProcessor<T>& Proc,
        const vector<int>& args, int size)
{
    throw runtime_error("input deprecated TZ prep");

    auto& input = Proc.input;
    input.reset_all(Proc.P);
    int n_arg_tuple = U::N_DEST + U::N_PARAM + 1;
    assert(args.size() % n_arg_tuple == 0);

    int n_from_me = 0;

    if (Proc.Proc and Proc.Proc->use_stdin())
    {
        for (size_t i = n_arg_tuple - 1; i < args.size(); i += n_arg_tuple)
            n_from_me += (args[i] == Proc.P.my_num()) * size;
        if (n_from_me > 0)
            cout << "Please input " << n_from_me << " " << U::NAME << "(s):" << endl;
    }

    for (size_t i = U::N_DEST; i < args.size(); i += n_arg_tuple)
    {
        int n = args[i + U::N_PARAM];
        InputBase<T>::prepare<U>(Proc, n, &args[i], size);
    }

    if (n_from_me > 0)
        cout << "Thank you" << endl;

    // exchange shares of x_i-r_i
    input.exchange();

    for (size_t i = 0; i < args.size(); i += n_arg_tuple)
    {
        int player = args[i + n_arg_tuple - 1];
        finalize<U>(Proc, player, &args[i], size);
    }
}


template<class T>
void InputBase<T>::raw_input(SubProcessor<T>& proc, const vector<int>& args,
        int size)
{
    throw runtime_error("raw_input deprecated TZ prep");
    auto& P = proc.P;
    reset_all(P);

    for (auto it = args.begin(); it != args.end();)
    {
        int player = *it++;
        it++;
        if (player == P.my_num())
        {
            for (int i = 0; i < size; i++)
            {
                clear t;
                try
                {
                    this->buffer.input(t);
                }
                catch (not_enough_to_buffer& e)
                {
                    throw runtime_error("Insufficient input data to buffer");
                }
                add_mine(t);
            }
        }
        else
        {
            for (int i = 0; i < size; i++)
                add_other(player);
        }
    }

    timer.start();
    exchange();
    timer.stop();

    for (auto it = args.begin(); it != args.end();)
    {
        int player = *it++;
        int base = *it++;
        for (int i = 0; i < size; i++)
            proc.get_S_ref(base + i) = finalize(player);
    }
}

#endif 