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
	const name founder_ac;

public:
	using contract::contract;

	tropiumstake(name receiver, name code, datastream<const char*> ds):
				contract(receiver, code, ds),
				dapp_token_symbol("TRPM", 4),
				lock_period(2'592'000),
				stake_amount(500'0000),
				token_contract_ac("trpm111token"_n),
				founder_ac("eosaidchains"_n) {}

	/**
	 * @brief - doctor register
	 * @details - doctor register
	 * 
	 * @param doctor - doctor name
	 * @param profile_url - profile link: e.g. "https://www.hipaaspace.com/medical_billing/coding/npi/codes/npi_1891894531.aspx"
	 * 
	 */
	ACTION regbydoctor( const name& doctor, 
						const string& profile_url);

	/**
	 * @brief - contract verifies a doctor 
	 * @details - contract verifies a doctor
	 * 
	 * @param verified_doctor - verified doctor name
	 * @param new_doctor - new doctor name
	 * 
	 * @pre doctor must exist in the auth table & should have status as "added"
	 * 
	 */
	ACTION verify( const name& verified_doctor,
					const name& new_doctor);


	/**
	 * @brief - verified doctor blacklists the doctor
	 * @details - verified doctor blacklists the doctor
	 * 			- if the blacklist_vector is == 5, then rem doctor from admin list.
	 * 
	 * @param verified_doctor - verified doctor
	 * @param doctor - doctor
	 */
	ACTION blacklist( const name& verified_doctor,
						const name& doctor);

	/**
	 * @brief - founder adds itself to the admin
	 * @details - founder adds itself to the admin
	 * 
	 */
	ACTION compaddadmin();


	/**
	 * @brief - verified_doctor add admin
	 * @details - verified_doctor add admin
	 * 			- Inline action
	 * 
	 * @param doctor - doctor
	 * 
	 */
	ACTION addadmin(const name& doctor);

	/**
	 * @brief - contract remove admin
	 * @details - contract remove admin
	 * 			- inline action
	 * 
	 * @param doctor - doctor
	 * 
	 */
	ACTION remadmin(const name& doctor);

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


	/**
	 * @brief - verified doctor starts the rehab
	 * @details - verified doctor starts the rehab
	 * 
	 * @param verified_doctor - verified doctor
	 * @param patient - patient
	 * @param lock_timestamp - lock timestamp
	 */
	ACTION startrehab(const name& verified_doctor,
						const name& patient,
						uint32_t lock_timestamp);


	/**
	 * @brief - doctor
	 * @details - doctor assigned ends the rehab
	 * 
	 * @param doctor - assigned doctor
	 * @param patient - patient
	 * @param patient_status - caught/cured
	 * 
	 * @pre - doctor must be the assigned one
	 */
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
	 * @pre - execute if (current_time > lock_timestamp && "cure"_n) OR
	 * @pre - execute if patient_status == "caught"_n or 
	 */
	ACTION unstake(const name& patient);


	/**
	 * @brief - send alert
	 * @details - send alert after the action is successfully done
	 * 
	 * @param user - user
	 * @param message - note depending on the action
	 */
	ACTION sendalert( const name& user,
						const string& message);

	// ACTION testdelstake(const name& patient) {
	// 	stakewallet_index stakewallet_table(get_self(), patient.value);
	// 	auto stakewallet_it = stakewallet_table.find(dapp_token_symbol.raw());

	// 	check(stakewallet_it != stakewallet_table.end(), "money hasn't been transferred to the wallet.");
	// 	stakewallet_table.erase(stakewallet_it);
	// }

	// ACTION testdeladmin() {
	// 	admin_index admin_table(get_self(), get_self().value);
	// 	auto admin_it = admin_table.find("doctor"_n.value);

	// 	check(admin_it != admin_table.end(), "set admins list using action - \'compaddadmin\'.");

	// 	admin_table.erase(admin_it);
	// }

	// ACTION testdelauth(const name& doctor) {
	// 	auth_index auth_table(get_self(), doctor.value);
	// 	auto auth_it = auth_table.find(doctor.value);

	// 	check(auth_it != auth_table.end(), "doctor is not present in the auth table.");
	// 	auth_table.erase(auth_it);
	// }

	static void check_quantity( const asset& quantity, const symbol& qty_sym ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == qty_sym, "symbol precision mismatch. Also, could be bcoz of sending some other tokens to this contract.");
	}


	static void check_admin(const name& stake_contract_ac,
		 						const name& doctor) {
		admin_index admin_table(stake_contract_ac, stake_contract_ac.value);
		auto admin_it = admin_table.find("doctor"_n.value);

		check(admin_it != admin_table.end(), "there is no admins list created so far.");
		auto search_it = std::find(admin_it->vector_admin.begin(), admin_it->vector_admin.end(), doctor);
		check(search_it != admin_it->vector_admin.end(), "the parsed verified doctor is not in the admins list.");
	}

private:
	// scope - patient
	TABLE stakewallet {
		asset staked_qty;						// staked amount
		uint32_t lock_timestamp;			// normally 30 days
		uint32_t start_timestamp;			// start time of rehab. shall be set when the Dr. sets the lock period
		uint32_t end_timestamp;				// end time of rehab
		name doctor;
		name patient_status;				// unassigned/assigned/caught/cured

		auto primary_key() const { return staked_qty.symbol.raw(); }
	};

	using stakewallet_index = multi_index<"stakewallet"_n, stakewallet>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE admin {
		name type;								// only one type i.e. doctor
		vector<name> vector_admin;						// E.g. ["admin1", "admin2", "admin3", "admin4", "admin5"]

		auto primary_key() const { return type.value; }
	};

	using admin_index = multi_index<"admins"_n, admin>;

	// -----------------------------------------------------------------------------------------------------------------------
	// scope - doctor name
	TABLE auth {
		name doctor;					// doctor name
		string profile_url;				// doctor's url
		name user_status;				// added/updated/verified
		uint32_t add_timestamp;			// timestamp at which the user details is added
		uint32_t verify_timestamp;		// timestamp at which the user details is verified
		uint32_t blist_timestamp;		// timestamp at which the user is blacklisted
		name validator_verify;			// validator who verifies the user
		vector<name> validators_blacklist;		// validator list who blacklists the user. the doctor will be marked as blacklisted only if the length is 5.


		auto primary_key() const { return doctor.value; }
	};

	using auth_index = multi_index<"auth"_n, auth>;

	// -----------------------------------------------------------------------------------------------------------------------
	// Adding inline action for `sendalert` action in the same contract 
	void send_alert(const name& user, const string& message);

	// get the current timestamp
	inline uint32_t now() const {
		return current_time_point().sec_since_epoch();
	}


};
