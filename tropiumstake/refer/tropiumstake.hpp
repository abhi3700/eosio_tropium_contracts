#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/transaction.hpp>
#include <string>


using eosio::contract;
using eosio::print;
using eosio::name;
using eosio::multi_index;
using eosio::const_mem_fun;
using eosio::indexed_by;
using eosio::asset;
using eosio::check;
using eosio::permission_level;
using eosio::datastream;
using eosio::current_time_point;
using eosio::action;
using eosio::same_payer;
using eosio::symbol;
using eosio::require_recipient;
using eosio::checksum256;
using eosio::action_wrapper;

using std::string;


CONTRACT tropiumstake : public contract 
{
private:
	const symbol dapp_token_symbol;

public:
	using contract::contract;

	  //TODO: change ("TRPM",0) to ("TRPM",4)
	 tropiumstake(name receiver, name code, datastream<const char*> ds):
				contract(receiver, code, ds),
				dapp_token_symbol("TRPM", 4),
				_staker_list(receiver, receiver.value),
				_banned_list(receiver, receiver.value),
				_admin_list(receiver, receiver.value),
				// tropium_symb("TRPM",0){} //thre precision of the symbol is the decimal precision of
								  //the created token

	/**
	* Register a user in the list of stakers.
	* @param: username, the account name of the staker
	*/
	ACTION regstaker(name username);

	/**
	* Catch the transfer from eosio.token and register the funds staked
	* @param: receiver, the account owning this smart contract
	* @param: msg, define the action to be used. "start" to deposit the first funds, and 
	*         "increment" to increase the staked funds 
	*/
	[[eosio::on_notify("*::transfer")]]
	void stake (name username, name receiver, asset quantity, string msg);

	/**
	* Used to ban a staker that hasn't respected the regulations. Funds are not reimbursed 
	* this action is performed by the smart contract account
	* @param: username, the account that will be banned.
	* @param: admin, an administrator in the system
	*/
	ACTION banstaker (name username);

	/**
	* Used to reinstate a banned user. After this operation, user should re-register
	* the admin should also decide if the fund should be held or reimbursed 
	* @param: username, the account that was banned.
	* @param: admin, an administrator in the system
	* @param: is_given_back, if True, send back funds of username else keep them.
	*/
	ACTION letinstaker(name admin, name username, bool is_given_back);

	/*
	* Cancel the stake of a user and remove him from the table, refund the held amount
	* @param: username, the account wanting to unstake its funds. 
	*/
	ACTION unstake (name username);

	/**
	* Register an administrator. This function is only executable by the owner of the smart contract account
	* @param: username, the account that should be registered as an admin
	*/
	ACTION addadmin (name username);

	/**
	* Remove a user from the list of administrator. This function can only be executed by the
	* smart contract accoun  
	* @param: username, the account that should be removed from the admin_list 
	*/
	ACTION removeadmin(name username);

  private:
	  // table used to maintain the list of staking nodes
		TABLE staker{
		  name username; //name of the staking user
		  asset fund_staked; // funds to be staked
		  bool isstaked; //if the users has already staked funds or not
		  uint64_t primary_key() const {return username.value;}
		};
		typedef eosio::multi_index<name("stakerlist"), staker> stakers;
		stakers _staker_list;

		//the symbol of the tropium token (TRPM, 0.0000)
		const symbol tropium_symb;

		// table used to maintain banned people. Doctors approval needed to remove them from the list
		TABLE banned{
		  name username;
		  asset fund_held; //funds that were staked by the user
		  uint64_t primary_key() const {return username.value;}
		};
		typedef eosio::multi_index<name("bannedlist"), banned> banned_stakers;
		banned_stakers _banned_list;  


		// table used to maintain the list of administrators (doctors, physicians, etc.)
		// they can ban users or reinstate them
		TABLE admin{
		  name username;
		  uint64_t primary_key() const {return username.value;}
		};
		typedef eosio::multi_index<name("adminlist"), admin> admins;
		admins _admin_list;

		//check if a user is an admin
		void is_admin(name username);

		//called to transfer funds held by the smart contract to recipient
		void in_contract_transfer(name recipient, asset amount, string msg);
};
