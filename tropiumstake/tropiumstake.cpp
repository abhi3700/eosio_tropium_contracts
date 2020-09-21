#include "tropiumstake.hpp"


// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::regbydoctor( const name& doctor, 
								const string& profile_url)
{
	require_auth(doctor);

	auth_index auth_table(get_self(), doctor.value);
	auto auth_it = auth_table.find(doctor.value);

	check(auth_it == auth_table.end(), "the doctor\'s info is already added.");
	auth_table.emplace(doctor, [&](auto& row) {
		row.doctor = doctor;
		row.profile_url = profile_url;
		row.user_status = "added"_n;
		row.add_timestamp = now();
	});
	// else {
	// 	check((auth_it->type != type) || (auth_it->profile_url != profile_url), "Either type or profile_url has to be different than the stored one.");
	// 	auth_table.modify(auth_it, doctor, [&](auto& row) {
	// 		if(auth_it->type != type) {
	// 			row.type = type;
	// 		}
	// 		if(auth_it->profile_url != profile_url) {
	// 			row.profile_url = profile_url;
	// 		}

	// 		row.user_status = "updated"_n;
	// 		row.update_timestamp = now();
	// 	});
	// }
}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::verify( const name& verified_doctor,
							const name& new_doctor) {
	require_auth(verified_doctor);

	// check if the verified_doctor is one of the admins
	check_admin(get_self(), verified_doctor);

	auth_index auth_table(get_self(), new_doctor.value);
	auto auth_it = auth_table.find(new_doctor.value);

	check(auth_it != auth_table.end(), "Doctor doesn\'t exist in the auth table.");

	// check that doctor's status must not be "verified"
	check((auth_it->user_status != "verified"_n), "The doctor may already verified.");

	auth_table.modify(auth_it, get_self(), [&](auto& row) {
		row.user_status = "verified"_n;
		row.verify_timestamp = now();
		row.validator_verify = verified_doctor;
		if(auth_it->validators_blacklist.size() != 0) {
			row.validators_blacklist.clear();
		}
	});

	// add new_doctor to the admins list
	action(
		permission_level{get_self(), "active"_n},
		get_self(),
		"addadmin"_n,
		std::make_tuple(new_doctor)
	).send();


}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::blacklist( const name& verified_doctor,
								const name& doctor) {
	require_auth(verified_doctor);

	// check if the verified_doctor is one of the admins
	check_admin(get_self(), verified_doctor);

	auth_index auth_table(get_self(), doctor.value);
	auto auth_it = auth_table.find(doctor.value);

	check(auth_it != auth_table.end(), "Doctor doesn\'t exist in the auth table.");

	auto blacklist_vector_count = auth_it->validators_blacklist.size();

	auto search_it = std::find(auth_it->validators_blacklist.begin(), auth_it->validators_blacklist.end(), verified_doctor);
	check(search_it == auth_it->validators_blacklist.end(), "the parsed verified_doctor has already blacklisted this parsed doctor. So, other admins need to try.");

	auth_table.modify(auth_it, get_self(), [&](auto& row) {
		row.user_status = "blacklisted"_n;
		row.blist_timestamp = now();
		row.validators_blacklist.emplace_back(verified_doctor);
	});

	// after adding the blacklister to the vector, if the vector count is 5, then remove doctor from the admins list.
	if( (blacklist_vector_count + 1) == 5 ) {

		// remove doctor to the admins list
		action(
			permission_level{get_self(), "active"_n},
			get_self(),
			"remadmin"_n,
			std::make_tuple(doctor)
		).send();
	}

}


// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::compaddadmin() {
	require_auth(founder_ac);

	admin_index admin_table(get_self(), get_self().value);
	auto admin_it = admin_table.find("doctor"_n.value);

	check(admin_it == admin_table.end(), "admins list is already initialized by founder: \'eosaidchains\'. So, can\'t be redone.");
	
	admin_table.emplace(get_self(), [&](auto& row){	// not found
		row.type = "doctor"_n;
		row.vector_admin = {founder_ac};
	});

}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::addadmin(const name& doctor) {

	require_auth(get_self());

	admin_index admin_table(get_self(), get_self().value);
	auto admin_it = admin_table.find("doctor"_n.value);

	check(admin_it != admin_table.end(), "admins list must have min. 1 verified doctor i.e. \'eosaidchains\'");
	check(admin_it->vector_admin.size() != 0, "admins list size must be non-zero as it must have min. 1 verified doctor i.e. \'eosaidchains\'");

	auto vec = admin_it->vector_admin;
	auto vec_it = std::find(vec.begin(), vec.end(), doctor);

	check(vec_it == vec.end(), "the parsed doctor is already in the list.");
	
	admin_table.modify(admin_it, get_self(), [&](auto& row){	// not found
		row.vector_admin.emplace_back(doctor);
	});

}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::remadmin(const name& doctor) {
    require_auth(get_self());

    admin_index admin_table(get_self(), get_self().value);
    auto admin_it = admin_table.find("doctor"_n.value);

    check(admin_it != admin_table.end(), "set admins list using action - \'compaddadmin\' action.");
    check(admin_it->vector_admin.size() != 0, "empty admin list");
    
    auto vec_it = std::find(admin_it->vector_admin.begin(), admin_it->vector_admin.end(), doctor);
    check(vec_it != admin_it->vector_admin.end(), "the parsed doctor is not in the admins list."); 

    admin_table.modify(admin_it, get_self(), [&](auto& row){    // found & erase it
        row.vector_admin.erase(vec_it);
    });

}


// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::stake(	const name& patient,
							const name& contract_ac,
							const asset& quantity,
							const string& memo ) {
	// check for conditions if either or both of these are true for `from` & `to`
	if(contract_ac != get_self() ||  patient == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the buyer.");
		return;
	}

	check_quantity(quantity, dapp_token_symbol);

	check(quantity.amount == stake_amount , "the send quantity amount should be 500 TRPM.");

	stakewallet_index stakewallet_table(get_self(), patient.value);
	auto stakewallet_it = stakewallet_table.find(quantity.symbol.raw());

	if (stakewallet_it == stakewallet_table.end()) {
		stakewallet_table.emplace(get_self(), [&](auto& row){
			row.staked_qty = quantity;
			row.patient_status = "unassigned"_n;
		});		
	} else {
		stakewallet_table.modify(stakewallet_it, get_self(), [&](auto& row){
			row.staked_qty += quantity;
			row.patient_status = "unassigned"_n;
		});		
	}
	
}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::startrehab(const name& verified_doctor,
								const name& patient,
								uint32_t lock_timestamp) {
	require_auth(verified_doctor);

	// check if the verified_doctor is one of the admins
	check_admin(get_self(), verified_doctor);

	// NOTE: this has been not set as it will be too accurate to push this action
	// check(lock_timestamp - now() == lock_period, "the lock_timestamp needs to be such that it is 30 days from now.");

	stakewallet_index stakewallet_table(get_self(), patient.value);
	auto stakewallet_it = stakewallet_table.find(dapp_token_symbol.raw());

	check(stakewallet_it != stakewallet_table.end(), "money hasn't been transferred to the wallet.");
	check(stakewallet_it->staked_qty.amount == stake_amount, "patient must have staked qty as 500 TRPM.");
	check(stakewallet_it->patient_status == "unassigned"_n, "the patient status must be unassigned before starting of rehab.");
	
	stakewallet_table.modify(stakewallet_it, get_self(), [&](auto& row){
		row.lock_timestamp = lock_timestamp;
		row.start_timestamp = now();
		row.doctor = verified_doctor;
		row.patient_status = "assigned"_n;
	});

	// send alert to patient
	send_alert(patient, "Your rehab period is started by the doctor: \'" + stakewallet_it->doctor.to_string() + "\'.");
}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::endrehab(const name& doctor,
						const name& patient,
						const name& patient_status) {
	
	check((patient_status == "caught"_n) || (patient_status == "cured"_n), "patient status can either be \'caught\' or \'cured\'.");

	stakewallet_index stakewallet_table(get_self(), patient.value);
	auto stakewallet_it = stakewallet_table.find(dapp_token_symbol.raw());

	check(stakewallet_it != stakewallet_table.end(), "patient hasn\'t transferred any money to the wallet.");
	
	check(doctor == stakewallet_it->doctor, "parsed Doctor\'s name does not match with assigned Dr. name.");

	require_auth(stakewallet_it->doctor);

	stakewallet_table.modify(stakewallet_it, get_self(), [&](auto& row){
		row.end_timestamp = now();
		row.patient_status = patient_status;
	});

	// send alert to patient
	send_alert(patient, "Your rehab period is ended as the doctor: \'" + stakewallet_it->doctor.to_string() + "\' mentioned status as: \'" + 
							stakewallet_it->patient_status.to_string() + "\'.");
}
// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::unstake(const name& patient) {

	stakewallet_index stakewallet_table(get_self(), patient.value);
	auto stakewallet_it = stakewallet_table.find(dapp_token_symbol.raw());

	check(stakewallet_it != stakewallet_table.end(), "patient hasn\'t transferred any money to the wallet.");

	require_auth(patient);

	// if cured, then 5% to Dr. & 95% to patient 
	if((now() > stakewallet_it->lock_timestamp) && (stakewallet_it->patient_status == "cured"_n)) {
		auto dr_money = asset(0.05 * stake_amount, dapp_token_symbol);
		auto patient_recieve_money = asset(0.95 * stake_amount, dapp_token_symbol);

		action(
			permission_level{get_self(), "active"_n},
			token_contract_ac,
			"transfer"_n,
			std::make_tuple(get_self(), stakewallet_it->doctor, dr_money, 
								"TROPIUM Stake contract disburses " + dr_money.to_string() + " to \'" 
								+ stakewallet_it->doctor.to_string() + "\'. for treating \'" + patient.to_string() + "\'")
		).send();

		action(
			permission_level{get_self(), "active"_n},
			token_contract_ac,
			"transfer"_n,
			std::make_tuple(get_self(), patient, patient_recieve_money, 
								"TROPIUM Stake contract disburses " + patient_recieve_money.to_string() + " to \'" 
								+ patient.to_string() + "\'. for getting treatment from \'" + stakewallet_it->doctor.to_string() + "\'")
		).send();

		stakewallet_table.erase(stakewallet_it);

	} 
	// if caught, then 100% to Dr. 
	else if ((stakewallet_it->patient_status == "caught"_n)){
		action(
			permission_level{get_self(), "active"_n},
			token_contract_ac,
			"transfer"_n,
			std::make_tuple(get_self(), stakewallet_it->doctor, stakewallet_it->staked_qty, 
								"TROPIUM Stake contract disburses " + stakewallet_it->staked_qty.to_string() + " to \'" 
								+ stakewallet_it->doctor.to_string() + "\'. for treating \'" + patient.to_string() + "\'")
		).send();

		stakewallet_table.erase(stakewallet_it);

		// send alert to patient
		send_alert(patient, "You lose your staked tokens: \'" + stakewallet_it->staked_qty.to_string() + "\' for rehab");
	}



}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::sendalert(const name& user,
							const string& message) {
	require_auth(get_self());

	require_recipient(user);
}

void tropiumstake::send_alert(const name& user, 
							const string& message) {
	check(message.size() <= 256, "message has more than 256 bytes");
	
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"sendalert"_n,
		std::make_tuple(user, message)
	).send();
}
