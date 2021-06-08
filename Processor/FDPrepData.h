#include <iostream>
#include <string>
#include <unordered_map>


template<class T>
class FDPrepData{
  // hashtable, key = wire id (must be unique)
  std::unordered_map<int, int> ext_vals;
  std::unordered_map<int, T> perm_vals;

  public:
    T& get_perm(int w){return perm_vals[w];}
    T& get_ext(int w){return ext_vals[w];}
    
    T& set_perm(int w, T v){perm_vals[w]=v;}
    T& set_ext(int w, int v){ext_vals[w]=v;}
};