#pragma once
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

#include <string>

namespace eosiosystem {
	class system_contract;
}

using eosio::contract;
using eosio::name;
using eosio::multi_index;
using eosio::asset;
using eosio::check;
using eosio::same_payer;
using eosio::symbol;
using eosio::require_recipient;
using eosio::action_wrapper;
using eosio::symbol_code;
using eosio::datastream;

using std::string;


CONTRACT vigortoken : public contract {

public:
	using contract::contract;

	// vigortoken(name receiver, name code, datastream<const char*> ds) : 
	// 			contract(receiver, code, ds) {}

	/**
	* Allows `issuer` account to create a token in supply of `maximum_supply`. If validation is successful a new entry in statstable for token symbol scope gets created.
	*
	* @param issuer - the account that creates the token,
	* @param maximum_supply - the maximum supply set for the token created.
	*
	* @pre Token symbol has to be valid,
	* @pre Token symbol must not be already created,
	* @pre maximum_supply has to be smaller than the maximum supply allowed by the system: 2^62 - 1.
	* @pre Maximum supply must be positive;
	*/
	ACTION create(const name& issuer, const asset& maximum_supply);

	/**
	 *  This action issues to `to` account a `quantity` of tokens.
	 *
	 * @param to - the account to issue tokens to, it must be the same as the issuer,
	 * @param quantity - the amount of tokens to be issued,
	 * @memo - the memo string that accompanies the token issue transaction.
	 */
	ACTION issue(const name& to, const asset& quantity, const string& memo);

	/**
	* The opposite for create action, if all validations succeed,
	* it debits the statstable.supply amount.
	*
	* @param quantity - the quantity of tokens to retire,
	* @param memo - the memo string to accompany the transaction.
	*/
	ACTION retire(const asset& quantity, const string& memo);

	/**
	* Allows `from` account to transfer to `to` account the `quantity` tokens.
	* One account is debited and the other is credited with quantity tokens.
	*
	* @param from - the account to transfer from,
	* @param to - the account to be transferred to,
	* @param quantity - the quantity of tokens to be transferred,
	* @param memo - the memo string to accompany the transaction.
	*/
	ACTION transfer(const name& from, const name& to, 
					const asset& quantity, const string& memo);

	/**
	* Allows `ram_payer` to create an account `owner` with zero balance for
	* token `symbol` at the expense of `ram_payer`.
	*
	* @param owner - the account to be created,
	* @param symbol - the token to be payed with by `ram_payer`,
	* @param ram_payer - the account that supports the cost of this action.
	*
	* More information can be read [here](https://github.com/EOSIO/eosio.contracts/issues/62)
	* and [here](https://github.com/EOSIO/eosio.contracts/issues/61).
	*/
	ACTION open(const name& owner, const symbol& symbol, const name& ram_payer);

	/**
	* This action is the opposite for open, it closes the account `owner`
	* for token `symbol`.
	*
	* @param owner - the owner account to execute the close action for,
	* @param symbol - the symbol of the token to execute the close action for.
	*
	* @pre The pair of owner plus symbol has to exist otherwise no action is executed,
	* @pre If the pair of owner plus symbol exists, the balance has to be zero.
	*/
	ACTION close(const name& owner, const symbol& symbol);

	// ACTION testdelstat(const name& token_contract_account, const symbol_code& sym_code ) {
	// 	stats_index statstable(token_contract_account, sym_code.raw());
	// 	auto stats_it = statstable.find(sym_code.raw());

	// 	check(stats_it != statstable.end(), "stat doesn't exist.");
	// 	statstable.erase(stats_it);
	// }

	// ACTION testdelaccnt(const name& token_contract_account, const name& owner, const symbol_code& sym_code ) {
	// 	accounts_index accountstable(token_contract_account, owner.value);
	// 	auto accounts_it = accountstable.find(sym_code.raw());

	// 	check(accounts_it != accountstable.end(), "account doesn't exist.");
	// 	accountstable.erase(accounts_it);
	// }

	static asset get_supply(const name& token_contract_account, const symbol_code& sym_code) {
		stats_index statstable(token_contract_account, sym_code.raw());
		const auto& st = statstable.get(sym_code.raw());	// get that row which contains the symbol as it is declared as primary_index
		return st.supply;							// now, return member i.e. 'supply' of struct 'st'
	}

	static asset get_balance(const name& token_contract_account, const name& owner, const symbol_code& sym_code) {
		accounts_index accountstable(token_contract_account, owner.value);
		const auto& ac = accountstable.get(sym_code.raw());	// get that row which contains the symbol as it is declared as primary_index
		return ac.balance;							// now, return member i.e. 'supply' of struct 'ac'
	}

	using create_action  = action_wrapper<"create"_n, &vigortoken::create>;
	using issuer_action  = action_wrapper<"issue"_n, &vigortoken::issue>;
	using retire_action  = action_wrapper<"retire"_n, &vigortoken::retire>;
	using transfer_action  = action_wrapper<"transfer"_n, &vigortoken::transfer>;
	using open_action  = action_wrapper<"open"_n, &vigortoken::open>;
	using close_action  = action_wrapper<"close"_n, &vigortoken::close>;

private:
	// ------------------------------------------------------------------
	TABLE account
	{
		asset balance;

		uint64_t primary_key() const { return balance.symbol.code().raw(); }
	};

	using accounts_index = eosio::multi_index< "accounts"_n, account >;

	// ------------------------------------------------------------------
	TABLE currency_stats
	{
		asset supply;
		asset max_supply;
		name issuer;
		
		uint64_t primary_key() const { return supply.symbol.code().raw(); }
	};

	using stats_index = eosio::multi_index< "stat"_n, currency_stats >;

	// ------------------------------------------------------------------
	void sub_balance(const name& owner, const asset& value);
	void add_balance(const name& owner, const asset& value, const name& ram_payer);


};
