#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <vector>
#include <algorithm>
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
using std::vector;
using std::pair;
using std::make_pair;


CONTRACT tropiumico : public contract
{
private:
	const symbol fund_token_symbol;
	const symbol dapp_token_symbol;
	const name token_contract_ac;
	const name stake_contract_ac;
	const name dapp_token_issuer;

public:
	using contract::contract;

	tropiumico(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				fund_token_symbol("EOS", 4),
				dapp_token_symbol("TRPM", 4),
				token_contract_ac("trpm111token"_n),
				stake_contract_ac("trpm111stake"_n),
				dapp_token_issuer("tropiumchain"_n) {}



	/**
	 * @brief - only accessed by contract owner
	 * @details - contract owner initializes the ICO rate for buy, phase_type. NOTE: not used for modifying ICO rate
	 * 
	 * @param buyorsell_type - buy/sell
	 * @param phase_type - A/B/C
	 * @param current_price - price per EOS token
	 * 
	 * @pre - the phase_type row (for buy/sell) must not exist
	 */
	ACTION seticorate( const name& phase_type,
						const asset& current_price );


	/**
	 * @brief - deposit EOS into ICO fund to the contract
	 * @details 
	 * 		- deposit the `amount` to the contract to recieve tokens at ICO rate.
	 * 		- this table `fund` acts as a record-keeper for amounts sent by the buyers
	 * @param buyer_ac - buyer account
	 * @param contract_ac - contract account
	 * @param quantity - the deposit amount
	 * @param memo - remarks
	 */
	[[eosio::on_notify("eosio.token::transfer")]]
	void deposit( const name& buyer_ac, 
					const name& contract_ac, 
					const asset& quantity, 
					const string& memo );


	/**
	 * @brief - only accessed by contract
	 * @details - disburse from contract a/c to user a/c. Also, update the fund table
	 * 
	 * @param receiver_ac - account to whom money is disbursed
	 * @param buyorsell_type - buy/sell
	 * @param phase_type - A/B/C
	 * @param disburse_qty - deposited quantity
	 * @param memo - purpose of disbursing money
	 */
	ACTION disburse( const name& receiver_ac,
					const name& phase_type,
					const asset& disburse_qty,
					const string& memo );

	/**
	 * @brief - send alert
	 * @details - send alert after the action is successfully done
	 * 
	 * @param user - user
	 * @param message - note depending on the action
	 */
	ACTION sendalert( const name& user,
						const string& message);


	// ACTION testdelico(const name& scope, const name& phase_type) {
	// 	require_auth(get_self());

	// 	icorate_index icorate_table(get_self(), scope.value);
	// 	auto ico_it = icorate_table.find(phase_type.value);

	// 	check(ico_it != icorate_table.end(), "The row for this phase_type doesn't exist" );
	// 	icorate_table.erase(ico_it);
	// }

	// ACTION testdelfund(const name& user, const name& fund_type) {
	// 	require_auth(get_self());
		
	// 	fund_index fund_table(get_self(), user.value);
	// 	auto fund_it = fund_table.find(fund_type.value);

	// 	check(fund_it != fund_table.end(), "The row for this fund_type doesn't exist" );
	// 	fund_table.erase(fund_it);
	// }

	static void check_quantity( const asset& quantity, const symbol& qty_sym ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == qty_sym, "symbol precision mismatch. Also, could be bcoz of sending some other tokens to this contract.");
	}

private:
	// `fund` table is for keeping the record of all buy in EOS, disburses in TRPM
	// scope - user
	TABLE fund
	{
		name phase_type;				// a or b or c
		asset tot_deposit_qty;			// 5.0000 EOS
		asset tot_disburse_qty;			// 200.0000 TRPM @ respective ICO rate

		auto primary_key() const { return phase_type.value; }
	};

	using fund_index = multi_index<"fund"_n, fund>;

	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE icorate
	{
		name phase_type;									// a/b/c
		asset current_price;						// E.g. 1 TRPM = 0.15 EOS => 1 EOS = 6.67 TRPM. So, price_pereos = 6.67 TRPM

		auto primary_key() const { return phase_type.value; }
	};

	using icorate_index = multi_index<"icorates"_n, icorate>;

	// -----------------------------------------------------------------------------------------------------------------------
	// Adding inline action for `sendalert` action in the same contract 
	void send_alert(const name& user, const string& message);


};


