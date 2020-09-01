#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>

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


CONTRACT vigorico : public contract
{
private:
	const symbol fund_token_symbol;
	const symbol dapp_token_symbol;
	const name token_contract_ac;
	const name dapp_token_issuer;

public:
	using contract::contract;

	vigorico(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				fund_token_symbol("EOS", 4),
				dapp_token_symbol("VIGOR", 4),
				token_contract_ac("vigor11token"_n),
				dapp_token_issuer("vigor1issuer"_n) {}



	/**
	 * @brief - only accessed by contract owner
	 * @details - contract owner initializes the ICO rate for buy, phase_type. NOTE: not used for modifying ICO rate
	 * 
	 * @param buyorsell_type - buy/sell
	 * @param phase_type - A/B/C
	 * @param current_price_pereos - price per EOS token
	 * @param vector_admin - vector of admins who has the right to propose/approve the price proposal
	 * 
	 * @pre - the phase_type row (for buy/sell) must not exist
	 */
	ACTION initicorate( const name& buyorsell_type,
						const name& phase_type,
						float current_price_pereos,
						const vector<name> vector_admin );



	/**
	 * @brief - only accessed by admins
	 * @details - setter set ICO rate
	 * 
	 * @setter - one of admins
	 * @param buyorsell_type - buy/sell
	 * @param phase_type - A/B/C
	 * @param proposed_price_pereos - price per EOS token
	 * 
	 * 
	 * @pre - setter must be one of admins
	 */
	ACTION propoicorate( const name& setter,
						const name& buyorsell_type,
						const name& phase_type,
						float proposed_price_pereos,
						uint32_t decision_timestamp );


	/**
	 * @brief - only accessed by admins
	 * @details - vote for the proposal
	 * 
	 * @admin - one of admins
	 * @param buyorsell_type - buy/sell
	 * @param phase_type - A/B/C
	 * @param vote - y/n
	 * 
	 * @pre - [current_timestamp <= decision_timestamp]
	 */
	ACTION voteicorate( const name& admin,
						const name& buyorsell_type,
						const name& phase_type,
						const name& vote );


	/**
	 * @brief - only accessed by the contract
	 * @details - count the votes & change the current_price as proposed_price
	 * 
	 * @param buyorsell_type - buy or sell
	 * @param phase_type - A/B/C
	 * 
	 */
	ACTION approicorate( const name& buyorsell_type,
						const name& phase_type );

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
	[[eosio::on_notify("*::transfer")]]
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
					const name& buyorsell_type,
					const name& phase_type,
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


	ACTION testdelico(const name& scope, const name& phase_type) {
		require_auth(get_self());

		icorate_index icorate_table(get_self(), scope.value);
		auto ico_it = icorate_table.find(phase_type.value);

		check(ico_it != icorate_table.end(), "The row for this phase_type doesn't exist" );
		icorate_table.erase(ico_it);
	}

	ACTION testdelfund(const name& user, const name& fund_type) {
		require_auth(get_self());
		
		fund_index fund_table(get_self(), user.value);
		auto fund_it = fund_table.find(fund_type.value);

		check(fund_it != fund_table.end(), "The row for this fund_type doesn't exist" );
		fund_table.erase(fund_it);
	}

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
		name phase_type;									// a/b/c
		float current_price_pereos;						// E.g. 1 EOS = 30 VIGOR. So, price_pereos = 30
		float proposed_price_pereos;					// E.g. 1 EOS = 40 VIGOR. So, price_pereos = 40
		vector<name> vector_admin;						// E.g. ["admin1", "admin2", "admin3", "admin4", "admin5"]
		vector<pair<name, name>> vector_admin_vote;			// E.g. [{"admin1": "y"}, {"admin2": "n"}]
		uint32_t decision_timestamp;					// E.g. in next 15 mins, use the timestamp 15 mins from current timestamp

		auto primary_key() const { return phase_type.value; }
	};

	using icorate_index = multi_index<"icorates"_n, icorate>;

	// -----------------------------------------------------------------------------------------------------------------------
	// struct account
	// {
	// 	asset balance;

	// 	uint64_t primary_key() const { return balance.symbol.code().raw(); }
	// };

	// using accounts_index = eosio::multi_index< "accounts"_n, account >;
	// -----------------------------------------------------------------------------------------------------------------------
	// Adding inline action for `sendalert` action in the same contract 
	void send_alert(const name& user, const string& message);

	// Adding inline action for `disburse` action in the same contract 
	void disburse_inline( const name& receiver_ac,
							const name& buyorsell_type,
							const name& phase_type,
							const asset& disburse_qty,
							const string& memo );

	// get the current timestamp
	inline uint32_t now() const {
		return current_time_point().sec_since_epoch();
	}

	// NOTE: vector arg can't be const as emplace_back is non-const method
	// asset values are added here in order to store the total value 
	inline void creatify_vector_pair_fund( vector<pair<name, asset>>& v, 
										const name& s, 
										const asset& val) {
		auto s_it = std::find_if(v.begin(), v.end(), [&](auto& vs){ return vs.first == s; });
		if(s_it != v.end()) {		// key found
			s_it->second += val;		// modify by adding values
		}
		else {						// key NOT found
			v.emplace_back(make_pair(s, val));
		}
	}

	// admin can change their votes repeatedly before the decision_timestamp
	inline void creatify_vector_pair_ico( vector<pair<name, name>>& v, 
										const name& s, 
										const name& val		// can be "y"_n or "n"_n
										) {
		auto s_it = std::find_if(v.begin(), v.end(), [&](auto& vs){ return vs.first == s; });
		if(s_it != v.end()) {		// key found
			s_it->second = val;		// modify by setting values
		}
		else {						// key NOT found
			v.emplace_back(make_pair(s, val));
		}
	}

	// check if the user is one of admins set in the vector
	inline void check_admin(const vector<name> vec, const name& s) {
		auto it = std::find(vec.begin(), vec.end(), s);
		check(it != vec.end(), "Sorry! the user: " + s.to_string() + " is not present in the admin list.");
	}

	inline bool approval_status(vector<pair<name, name>> vec) {
		auto yes_count = 0;
		auto voter_count = 0;

		for (auto itr = vec.begin(); itr != vec.end(); ++itr) {
			if(itr->second == "y"_n) {
				++yes_count;
			}
			++voter_count;
		}

		// if "yes" count is more than 50% of voter_count
		if(yes_count > voter_count/2) {
			return true;
		} else {
			return false;
		}

	}

	// set the precision of price_pereos (of float type) to precision 2. E.g. 37.7765757 to 37.78
	inline float round_float_2( float val ) {
		float rounded_up = ceilf(val * 100) / 100;
		return rounded_up;
	}

};


