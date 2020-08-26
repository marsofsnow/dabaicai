#include <learn.hpp>
#include<eosio/system.hpp>
ACTION learn::hi( name nm ) {
   /* fill in action body */
   print_f("time : %\n",111);
   print_f("Name : %\n",nm);
   print_f("account_name:%\n",_self);
  
   
}