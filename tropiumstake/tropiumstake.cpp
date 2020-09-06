#include "tropiumstake.hpp"

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::setadmins(const name& type,
							const vector<name> vector_admin ) {
	require_auth(get_self());

	admin_index admin_table(get_self(), get_self().value);
	auto admin_it = admin_table.find(type.value);

	if(admin_it == admin_table.end()) {
		admin_table.emplace(get_self(), [&](auto& row){
			row.type = type;
			row.vector_admin = vector_admin;
		});
	} else {
		check(admin_it->vector_admin != vector_admin, "the parsed admin list is same as the stored one.");
		
		admin_table.modify(admin_it, get_self(), [&](auto& row){
			row.vector_admin = vector_admin;
		});
	}
}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::addadmin(const name& type, 
							const name& admin) {
	require_auth(get_self());

	admin_index admin_table(get_self(), get_self().value);
	auto admin_it = admin_table.find(type.value);

	check(admin_it != admin_table.end(), "set admins list using action - \'setadmins\'.");

	if (admin_it->vector_admin.size() != 0) {		// non-empty vector
		auto vec = admin_it->vector_admin;
		auto vec_it = std::find(vec.begin(), vec.end(), admin);

		check(vec_it == vec.end(), "the parsed admin is already in the list.");
		
		admin_table.modify(admin_it, get_self(), [&](auto& row){	// not found
			row.vector_admin.emplace_back(admin);
		});

	} else {		// empty vector
		admin_table.modify(admin_it, get_self(), [&](auto& row){
			row.vector_admin.emplace_back(admin);
		});
	}
}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::remadmin(const name& type, 
                    const name& admin) {
    require_auth(get_self());

    admin_index admin_table(get_self(), get_self().value);
    auto admin_it = admin_table.find(type.value);

    check(admin_it != admin_table.end(), "set admins list using action - \'setadmins\'.");
    check(admin_it->vector_admin.size() != 0, "empty admin list");
    
    auto vec_it = std::find(admin_it->vector_admin.begin(), admin_it->vector_admin.end(), admin);
    check(vec_it != admin_it->vector_admin.end(), "the parsed admin is not in the list."); 

    admin_table.modify(admin_it, get_self(), [&](auto& row){    // found & erase it
        row.vector_admin.erase(vec_it);
    });

}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::clradmins(const name& type) {
    require_auth(get_self());

	admin_index admin_table(get_self(), get_self().value);
	auto admin_it = admin_table.find(type.value);

	check(admin_it != admin_table.end(), "set admins list using action - \'setadmins\'.");

	admin_table.modify(admin_it, get_self(), [&](auto& row){	// found & erase it
		row.vector_admin.clear();
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

	check(quantity.amount == stake_amount , "the send quantity amount should be 5000 ");

	stakewallet_index stakewallet_table(get_self(), patient.value);
	auto stakewallet_it = stakewallet_table.find(quantity.symbol.raw());

	check(stakewallet_it == stakewallet_table.end(), "money can't be further transferred to stake as it already exists.");
	check(stakewallet_it->staked_qty.amount == 0, "the patient must have zero balance, otherwise the amount is still present in the stake wallet.");
	
	stakewallet_table.emplace(get_self(), [&](auto& row){
		row.staked_qty = quantity;
		row.patient_status = "unassigned"_n;
	});
}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::startrehab(const name& doctor,
							const name& patient,
							uint32_t lock_timestamp) {
	require_auth(doctor);

	// NOTE: this has been not set as it will be too accurate to push this action
	// check(lock_timestamp - now() == lock_period, "the lock_timestamp needs to be such that it is 30 days from now.");

	stakewallet_index stakewallet_table(get_self(), patient.value);
	auto stakewallet_it = stakewallet_table.find(dapp_token_symbol.raw());

	check(stakewallet_it != stakewallet_table.end(), "money hasn't been transferred to the wallet.");
	
	stakewallet_table.modify(stakewallet_it, get_self(), [&](auto& row){
		row.lock_timestamp = lock_timestamp;
		row.start_timestamp = now();
		row.doctor = doctor;
		row.patient_status = "assigned"_n;
	});

	// send alert to patient
	send_alert(patient, "Your rehab period is started by the doctor: \'" + stakewallet_it->doctor.to_string() + "\'.");
}

// --------------------------------------------------------------------------------------------------------------------
void tropiumstake::endrehab(const name& doctor,
						const name& patient,
						const name& patient_status) {
	
	check((patient_status == "caught"_n) || (patient_status == "cured"_n), "patient status can be \'caught\' or \'cured\'");

	stakewallet_index stakewallet_table(get_self(), patient.value);
	auto stakewallet_it = stakewallet_table.find(dapp_token_symbol.raw());

	check(stakewallet_it != stakewallet_table.end(), "money hasn't been transferred to the wallet.");
	
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

	check(stakewallet_it != stakewallet_table.end(), "money hasn't been transferred to the wallet.");

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
