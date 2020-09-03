#include <tropiumstake.hpp>

ACTION tropiumstake::regstaker (name username){
  require_auth(username);
  
  auto itr_banned = _banned_list.find(username.value);
  check(itr_banned == _banned_list.end(), "You where banned, please see your administrator");

  auto itr = _staker_list.find(username.value);
  check(itr == _staker_list.end(), "You are already registered, you can stake your TRPM");

  asset temp_asset(0, tropium_symb);
  itr = _staker_list.emplace(username, [&](auto& row){
    row.username = username;
    row.fund_staked = temp_asset;
    row.isstaked = false;
  });

}

void tropiumstake::stake(name username, name receiver, asset quantity, string msg){
  require_auth(username);
  if (receiver != get_self() || username == get_self()) return;
  auto itr = _staker_list.find(username.value);

  check(itr != _staker_list.end(), "You cannot stake, you are not yet registered");
  check(quantity.symbol == tropium_symb, "wrong token used");
  //check( msg.size() <= 256, "msg has more than 256 bytes" );
  check(msg == "increment" || msg =="start", "Please use \"increment\" to increase your stake or \"start\" to deposit your first stake");
  
  if(msg == "start" && itr->isstaked == false){
  //TODO: Change amount from 5000 to 5000.0000
  check(quantity.amount >= 5000, "Staked amount not enough, stake at least 5000");
    _staker_list.modify(itr, username, [&](auto& row){
      row.fund_staked = quantity;
      row.isstaked = true;
    });
  }
  else if (msg == "increment" && itr->isstaked == true){
    _staker_list.modify(itr, username, [&](auto& row){
      row.fund_staked += quantity;
    });
  }
  else check(false, "Error with staking options, please check you status");

  require_recipient(username);
}


ACTION tropiumstake::unstake (name username){
  require_auth(username);
  auto itr = _staker_list.find(username.value);
  check(itr != _staker_list.end(), "Your are not a staker, please register first");
  print(get_self());

  in_contract_transfer(username,itr->fund_staked,string("Your funds have been transfered"));    

  _staker_list.erase(itr);
  require_recipient(username);
}

ACTION tropiumstake::banstaker(name username){
  require_auth(get_self());

  auto itr = _staker_list.find(username.value);
  check(itr != _staker_list.end(), "That user is not a staker");

  //registering user in the banned list
  _banned_list.emplace(get_self(), [&](auto& row){
  row.username = username;
  row.fund_held = itr->fund_staked;
  });

  //remove staker from staker list without reimbursment
  _staker_list.erase(itr);
}

ACTION tropiumstake::letinstaker(name admin, name username, bool is_given_back){
  require_auth(admin);

  is_admin(admin);

  auto itr = _banned_list.find(username.value);
  check(itr != _banned_list.end(), "This user was not banned");

  if(is_given_back){
    //return the funds of the user
    in_contract_transfer(username, itr->fund_held, string("Returned the funds"));
  
  }else{
    // keep the funds of the user
    //TODO change test2  to pool account
    in_contract_transfer("test2"_n, itr->fund_held, string("Kept funds"));
  }
  //remove user from banned list
  _banned_list.erase(itr);

}

ACTION tropiumstake::addadmin(name username){
  require_auth(get_self());

  check(is_account(username), "Given username is not an account on the network");

  auto itr_admin = _admin_list.find(username.value);
  check(itr_admin == _admin_list.end(), "That account is already an administrator in the system");

  _admin_list.emplace(get_self(),[&](auto& row){
    row.username = username;
  });
}

ACTION tropiumstake::removeadmin(name username){
  require_auth(get_self());

  auto itr_admin = _admin_list.find(username.value);
  check(itr_admin != _admin_list.end(), "That account is not an administrator");

  _admin_list.erase(itr_admin);

}

void tropiumstake::is_admin(name username){
  auto itr_admin = _admin_list.find(username.value);
  check(itr_admin != _admin_list.end(), "you are not an admin");
}

void tropiumstake::in_contract_transfer(name recipient, asset amount, string msg){

  //TODO: Chance contract name from test1 to eosio.token during deployment
  action{
        permission_level{get_self(), "active"_n},
        "test1"_n,
        "transfer"_n,
        std::make_tuple(get_self(), recipient, amount, msg)
      }.send();
}
