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


CONTRACT vigorico : public contract
{
private:
	const symbol fund_token_symbol;
	const symbol dapp_token_symbol;
	const name token_contract_ac;

public:
	using contract::contract;

	vigorico(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				fund_token_symbol("EOS", 4),
				dapp_token_symbol("VIGOR", 4),
				token_contract_ac("vigor11token"_n) {}



	/**
	 * @brief - only accessed by contract owner
	 * @details - contract owner initializes the ICO rate for buy, ico_type. NOTE: not used for modifying ICO rate
	 * 
	 * @param buyorsell_type - buy/sell
	 * @param ico_type - A/B/C
	 * @param price_pereos - price per EOS token
	 * @param vector_admin - vector of admins who has the right to propose/approve the price proposal
	 * 
	 * @pre - the ico_type row (for buy/sell) must not exist
	 */
	ACTION initicorate( const name& buyorsell_type,
						const name& ico_type,
						float price_pereos,
						const vector<name> vector_admin );



	/**
	 * @brief - only accessed by admins
	 * @details - setter set ICO rate
	 * 
	 * @setter - one of admins
	 * @param buyorsell_type - buy/sell
	 * @param ico_type - A/B/C
	 * @param price_pereos - price per EOS token
	 * 
	 * 
	 * @pre - setter must be one of admins
	 */
	ACTION seticorate( const name& setter,
						const name& buyorsell_type,
						const name& ico_type,
						float price_pereos,
						uint32_t decision_timestamp );

	/**
	 * @brief - deposit EOS into vigor ICO fund to the contract
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
	 * @param ico_type - A/B/C
	 * @param disburse_qty - deposited quantity
	 * @param memo - purpose of disbursing money
	 */
	ACTION disburse( const name& receiver_ac,
					const name& ico_type,
					const asset& disburse_qty,
					const string& memo );

	/**
	 * @brief - send alert
	 * @details - send alert after the action is successfully done. e.g. after the disburse action is implemented
	 * 
	 * @param user - driver/commuter
	 * @param message - note depending on the action
	 */
	ACTION sendalert( const name& user,
						const string& message);



	using disburse_action  = action_wrapper<"disburse"_n, &vigorico::disburse>;

	static void check_quantity( const asset& quantity, const symbol& qty_sym ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == qty_sym, "symbol precision mismatch. Also, could be bcoz of sending some other tokens to this contract.");
	}

private:
	// `fund` table is for keeping the record of all buy in EOS, disburses in VIGOR
	// scope - user
	TABLE fund
	{
		name fund_type;			// buy or sell
		vector<pair<name, asset>> tot_fundtype_qty;			// E.g. [{"a": "5.0000 EOS"}, {"b": "10.0000 EOS"}]
		vector<pair<name, asset>> tot_disburse_qty;			// E.g. [{"a": "200.0000 VIGOR"}, {"b": "500.0000 VIGOR"}]	@ respective ICO rate

		auto primary_key() const { return fund_type.value; }
	};

	using fund_index = multi_index<"fund"_n, fund>;

	// -----------------------------------------------------------------------------------------------------------------------
	// scope - buy or sell
	TABLE icorate
	{
		name ico_type;
		float current_price_pereos;						// E.g. 1 EOS = 30 VIGOR. So, price_pereos = 30
		float proposed_price_pereos;					// E.g. 1 EOS = 40 VIGOR. So, price_pereos = 40
		vector<name> vector_admin;						// E.g. ["admin1", "admin2", "admin3", "admin4", "admin5"]
		vector<name, name> vector_admin_vote			// E.g. [{"admin1": "y"}, {"admin2": "n"}]
		uint32_t decision_timestamp;					// E.g. in next 15 mins, use the timestamp 15 mins from current timestamp

		auto primary_key() const { return ico_type.value; }
	};

	using icorate_index = multi_index<"icorates"_n, icorate>;

	// -----------------------------------------------------------------------------------------------------------------------
	// Adding inline action for `sendalert` action in the same contract 
	void send_alert(const name& user, const string& message);

	// Adding inline action for `disburse` action in the same contract 
	void disburse_inline(const name& receiver_ac,
						const name& ico_type,
						const asset& quantity,
						const string& memo );

	// get the current timestamp
	inline uint32_t now() const {
		return current_time_point().sec_since_epoch();
	}

};


