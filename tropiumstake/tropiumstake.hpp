#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/transaction.hpp>
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

CONTRACT tropiumstake : public contract 
{
private:
	const symbol dapp_token_symbol;
	const uint32_t lock_period;
	const uint32_t stake_amount;
	const name token_contract_ac;

public:
	using contract::contract;

	tropiumstake(name receiver, name code, datastream<const char*> ds):
				contract(receiver, code, ds),
				dapp_token_symbol("TRPM", 4),
				lock_period(2'592'000),
				stake_amount(5000'0000),
				token_contract_ac("trpm111token"_n) {}

	/**
	 * @brief - set admins list
	 * @details - set admins list to a type
	 * 
	 * @param type - type
	 * @param vector_admin - list of admins
	 * 
	 * @pre - check the row has no data
	 */
	ACTION setadmins(const name& doctor,
					const name& type,
					const vector<name> vector_admin );

	/**
	 * @brief - add admins
	 * @details - add admins
	 * 
	 * @param type [description]
	 * @param admin [description]
	 * 
	 * @return [description]
	 */
	ACTION addadmin(const name& doctor,
					const name& type, 
					const name& admin);

	ACTION remadmin(const name& doctor,
					const name& type, 
					const name& admin);

	ACTION clradmins(const name& doctor,
					const name& type);

	/**
	 * @brief - stake by patient
	 * @details - patient stakes "5000 TRPM" amount, can only be transferred once by any patient
	 * 
	 * @param patient - patient
	 * @param contract - contract
	 * @param quantity - amount fixed -- 5000.0000 TRPM
	 * @param memo - any string
	 */
	[[eosio::on_notify("trpm111token::transfer")]]
	void stake(	const name& patient,
				const name& contract_ac,
				const asset& quantity,
				const string& memo );


	ACTION startrehab(const name& doctor,
						const name& patient,
						uint32_t lock_timestamp);

	ACTION endrehab(const name& doctor,
						const name& patient,
						const name& patient_status);


	/**
	 * @brief - unstake entirely
	 * @details - unstake entirely, also transfers 5% to the assigned doctor.
	 * 			- execute in 2 cases.
	 * 
	 * @param patient - patient
	 * 
	 * @pre - execute if (current_time > lock_timestamp && "cure"_n)
	 * @pre - execute if patient_status == "caught"_n or 
	 */
	ACTION unstake(const name& patient);

	static void check_quantity( const asset& quantity, const symbol& qty_sym ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == qty_sym, "symbol precision mismatch. Also, could be bcoz of sending some other tokens to this contract.");
	}

private:
	// scope - patient
	TABLE stakewallet {
		asset amount;						// staked amount
		uint32_t lock_timestamp;			// normally 30 days
		uint32_t start_timestamp;			// start time of rehab. shall be set when the Dr. sets the lock period
		uint32_t end_timestamp;				// end time of rehab
		name doctor;
		name patient_status;				// unassigned/assigned/caught/cured

		auto primary_key() const { return amount.symbol.raw(); }
	};

	using stakewallet_index = multi_index<"stakewallet"_n, stakewallet>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE admin {
		name type;								// psychiatrist, therapist
		vector<name> vector_admin;						// E.g. ["admin1", "admin2", "admin3", "admin4", "admin5"]

		auto primary_key() const { return type.value; }
	};

	using admin_index = multi_index<"admins"_n, admin>;

	// -----------------------------------------------------------------------------------------------------------------------
	// get the current timestamp
	inline uint32_t now() const {
		return current_time_point().sec_since_epoch();
	}




};
