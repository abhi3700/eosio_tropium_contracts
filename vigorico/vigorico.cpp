#include "vigorico.hpp"

// --------------------------------------------------------------------------------------------------------------------
void vigorico::initicorate( const name& buyorsell_type,
							const name& phase_type,
							float current_price_pereos,
							const vector<name> vector_admin )
{
	require_auth(get_self());

	check((buyorsell_type == "buy"_n) || (buyorsell_type == "sell"_n), "Type can either be \'buy\' or \'sell\'.");

	check((phase_type == "a"_n) || (phase_type == "b"_n) || (phase_type == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	icorate_index icorate_table(get_self(), buyorsell_type.value);
	auto ico_it = icorate_table.find(phase_type.value);

	check(ico_it == icorate_table.end(), "The row for this phase_type: " + phase_type.to_string() + " is already initialized." );

	icorate_table.emplace(get_self(), [&](auto& row){
		row.phase_type = phase_type;
		row.current_price_pereos = current_price_pereos;
		row.vector_admin = vector_admin;
	});
}

// --------------------------------------------------------------------------------------------------------------------
void vigorico::propoicorate( const name& setter,
							const name& buyorsell_type,
							const name& phase_type,
							float proposed_price_pereos,
							uint32_t decision_timestamp )
{


	check((buyorsell_type == "buy"_n) || (buyorsell_type == "sell"_n), "Type can either be \'buy\' or \'sell\'.");

	check((phase_type == "a"_n) || (phase_type == "b"_n) || (phase_type == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	icorate_index icorate_table(get_self(), buyorsell_type.value);
	auto ico_it = icorate_table.find(phase_type.value);

	check(ico_it != icorate_table.end(), "The row for this phase_type: " + phase_type.to_string() + " is not set. Contract should initiate using \'initicorate\' action." );

	check_admin(ico_it->vector_admin, setter);
	require_auth(setter);

	check((proposed_price_pereos != ico_it->proposed_price_pereos) || (decision_timestamp != ico_it->decision_timestamp), "Either parsed proposed_price or decision_timestamp must be different.");

	// setter can set a price only if the current_timestamp > decision_timestamp. Otherwise, it might happen that the price is set again while already a proposal running
	check(now() > ico_it->decision_timestamp, "Current timestamp must be greated than decision_timestamp. There is already a proposal running.");

	// TODO: how does setter gets the RAM back. Here, the row won't be deleted, but it will remain forever.

	icorate_table.modify(ico_it, setter, [&](auto& row){
		row.proposed_price_pereos = proposed_price_pereos;
		row.decision_timestamp = decision_timestamp;
	});
}

// --------------------------------------------------------------------------------------------------------------------
void vigorico::voteicorate( const name& admin,
							const name& buyorsell_type,
							const name& phase_type,
							const name& vote )
{
	check((buyorsell_type == "buy"_n) || (buyorsell_type == "sell"_n), "Type can either be \'buy\' or \'sell\'.");

	check((phase_type == "a"_n) || (phase_type == "b"_n) || (phase_type == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	icorate_index icorate_table(get_self(), buyorsell_type.value);
	auto ico_it = icorate_table.find(phase_type.value);

	check(ico_it != icorate_table.end(), "The row for this phase_type: " + phase_type.to_string() + " is not set. Contract should initiate using \'initicorate\' action." );

	check_admin(ico_it->vector_admin, admin);
	require_auth(admin);

	// vote can be "y" or "n"
	check((vote == "y"_n) || (vote == "n"_n), "admin's vote must be \'y\' or \'n\'.");

	auto v1 = ico_it->vector_admin_vote;
	auto admin_search_it = std::find_if(v1.begin(), v1.end(), [&](auto& vs){ return vs.first == admin; });

	// if admin is found in the voting list, then check if the parsed vote is already same as stored.
	if(admin_search_it != v1.end()) {
		check(vote != admin_search_it->second, "this vote already exists in the table.");
	}

	check(now() <= ico_it->decision_timestamp, "\'voteicorate\' action can be accessed within the \'decision_timestamp\'.");

	icorate_table.modify(ico_it, admin, [&](auto& row){
		creatify_vector_pair_ico(row.vector_admin_vote, admin, vote);
	});
}

// --------------------------------------------------------------------------------------------------------------------

void vigorico::approicorate( const name& buyorsell_type,
							const name& phase_type )
{
	require_auth(get_self());

	check((buyorsell_type == "buy"_n) || (buyorsell_type == "sell"_n), "Type can either be \'buy\' or \'sell\'.");

	check((phase_type == "a"_n) || (phase_type == "b"_n) || (phase_type == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	icorate_index icorate_table(get_self(), buyorsell_type.value);
	auto ico_it = icorate_table.find(phase_type.value);

	check(ico_it != icorate_table.end(), "The row for this phase_type: " + phase_type.to_string() + " is not set. Contract should initiate using \'initicorate\' action." );

	check(now() > ico_it->decision_timestamp, "\'approicorate\' action can be accessed only after the \'decision_timestamp\' gets elapsed.");

	icorate_table.modify(ico_it, get_self(), [&](auto& row){
		if(approval_status(ico_it->vector_admin_vote)) {
			row.current_price_pereos = ico_it->proposed_price_pereos;
		}
		row.vector_admin_vote.clear();		// clear all the votes after the decision_timestamp & the approval decision is taken. This is to ensure that for a new proposal old votes are not counted.
	});

}

// --------------------------------------------------------------------------------------------------------------------
void vigorico::deposit( const name& user, 
						const name& contract_ac, 
						const asset& quantity, 
						const string& memo ) {


	// check for conditions if either or both of these are true for `from` & `to`
	if(contract_ac != get_self() ||  user == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the buyer.");
		return;
	}

	if(user == dapp_token_issuer) {		// include the case where, additional ICO money with VIGOR tokens needed after the contract is deployed on this a/c 
		return;
	} else {		
		check((get_first_receiver() == "eosio.token"_n) || (get_first_receiver() == token_contract_ac), 
			"Acceptable token contracts: \'eosio.token\' or \'" + token_contract_ac.to_string() +" \'.");

		name buyorsell_type = ""_n;
		if(get_first_receiver() == "eosio.token"_n) {
			buyorsell_type = "buy"_n;
			
			// Although this is checked in "vigortoken::transfer" action, but fund_token_symbol check is pending. 
			// So, in addition the entire asset check is done using static func defined in "vigorico.hpp" file.
			// check quantity is valid for all conditions as 'asset'
			check_quantity(quantity, fund_token_symbol);

		} else if(get_first_receiver() == token_contract_ac) {
			buyorsell_type = "sell"_n;

			// Although this is checked in "vigortoken::transfer" action, but fund_token_symbol check is pending. 
			// So, in addition the entire asset check is done using static func defined in "vigorico.hpp" file.
			// check quantity is valid for all conditions as 'asset'
			check_quantity(quantity, dapp_token_symbol);
		}

		check((memo == "phase A") || (memo == "phase B") || (memo == "phase C"), 
			"For sending to this contract, parsed memo can either be \'buy for phase A' or \'phase B' or \'phase C'");

		name phase_type = ""_n;
		if(memo == "phase A") {
			phase_type = "a"_n;
		} else 	if(memo == "phase B") {
			phase_type = "b"_n;
		} else 	if(memo == "phase C") {
			phase_type = "c"_n;
		}

		// instantiate the `fund` table
		fund_index fund_table(get_self(), user.value);
		auto fund_it = fund_table.find(buyorsell_type.value);

		// update (add/modify) the deposit_qty
		if(fund_it == fund_table.end()) {
			fund_table.emplace(get_self(), [&](auto& row) {
				row.fund_type = buyorsell_type;
				row.tot_fundtype_qty.emplace_back(make_pair(phase_type, quantity));
				
				if(buyorsell_type == "buy"_n) {
					row.tot_disburse_qty.emplace_back(make_pair(phase_type, asset(0, dapp_token_symbol))) ;		// initialize the asset with symbol "VIGOR" & '0' amount 
				} else if(buyorsell_type == "sell"_n) {
					row.tot_disburse_qty.emplace_back(make_pair(phase_type, asset(0, fund_token_symbol))) ;		// initialize the asset with symbol "EOS" & '0' amount 
				}
			});
		} else {
			fund_table.modify(fund_it, get_self(), [&](auto& row) {
				creatify_vector_pair_fund(row.tot_fundtype_qty, phase_type, quantity);
			});
		}

		// prepare for disbursement of dapp tokens as per ICO rate
		if(buyorsell_type == "buy"_n) {
			auto disburse_asset = asset(0, dapp_token_symbol);

			icorate_index icorate_table(get_self(), buyorsell_type.value);

			auto ico_it = icorate_table.find(phase_type.value);

			check(ico_it != icorate_table.end(), "ICO rate for \'" + buyorsell_type.to_string() + "\' in phase " + phase_type.to_string() + " is not set. Contract owner must set using \'initicorate\'");
			check(ico_it->current_price_pereos != 0, "current price_pereos must be non-zero.");

			disburse_asset.amount = quantity.amount * ico_it->current_price_pereos;

			// // inline disburse of dapp token based on the amount of EOS sent
			disburse_inline(user, buyorsell_type, phase_type, disburse_asset, memo);

			// send alert to buyer for receiving dapp token
			send_alert(user, "You receive \'" + disburse_asset.to_string() + "\' for depositing \'" + 
										quantity.to_string() + "\' to vigor ICO contract for " + buyorsell_type.to_string() + " in " + memo );
		} 
		else if(buyorsell_type == "sell"_n) {
			auto disburse_asset = asset(0, fund_token_symbol);

			icorate_index icorate_table(get_self(), buyorsell_type.value);

			auto ico_it = icorate_table.find(phase_type.value);

			check(ico_it != icorate_table.end(), "ICO rate for \'" + buyorsell_type.to_string() + "\' in phase " + phase_type.to_string() + " is not set. Contract owner must set using \'initicorate\'");
			check(ico_it->current_price_pereos != 0, "current price_pereos must be non-zero.");

			disburse_asset.amount = quantity.amount * ico_it->current_price_pereos;

			// // inline disburse of dapp token based on the amount of EOS sent
			disburse_inline(user, buyorsell_type, phase_type, disburse_asset, memo);

			// send alert to buyer for receiving dapp token
			send_alert(user, "You receive \'" + disburse_asset.to_string() + "\' for depositing \'" + 
										quantity.to_string() + "\' to vigor ICO contract for " + buyorsell_type.to_string() + " in " + memo );
		}

	}



}


// --------------------------------------------------------------------------------------------------------------------
void vigorico::disburse(const name& receiver_ac,
						const name& buyorsell_type,
						const name& phase_type,
						const asset& disburse_qty,
						const string& memo )
{
	require_auth(get_self());

	// require_recipient(receiver_ac);

	check(is_account(receiver_ac), "receiver account doesn't exist");
	check( receiver_ac != get_self(), "amount can't be disbursed to contract itself");

	check((buyorsell_type == "buy"_n) || (buyorsell_type == "sell"_n), "buyorsell_type can either be \'buy\' or \'sell\'.");
	check((phase_type == "a"_n) || (phase_type == "b"_n) || (phase_type == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	if(buyorsell_type == "buy"_n) {
		// check quantity is valid for all conditions as 'asset'
		check_quantity(disburse_qty, dapp_token_symbol);
	}
	else if(buyorsell_type == "sell"_n) {
		// check quantity is valid for all conditions as 'asset'
		check_quantity(disburse_qty, fund_token_symbol);
	}

	// instantiate the `fund` table
	fund_index fund_table(get_self(), receiver_ac.value);
	auto fund_it = fund_table.find(buyorsell_type.value);

	// Make sure that the receiver_ac has deposited amount in the table
	check( fund_it != fund_table.end(), "Sorry! There is no amount deposited by " + receiver_ac.to_string() + "in the fund.");

	if(buyorsell_type == "buy"_n) {
		action(
			permission_level{get_self(), "active"_n},
			token_contract_ac,
			"transfer"_n,
			std::make_tuple(get_self(), receiver_ac, disburse_qty, 
								"VIGOR ICO contract disburses " + disburse_qty.to_string() + " to \'" + receiver_ac.to_string() + "\'. for " + buyorsell_type.to_string() + " in " + memo)
		).send();
	} else if(buyorsell_type == "sell"_n) {
		action(
			permission_level{get_self(), "active"_n},
			"eosio.token"_n,
			"transfer"_n,
			std::make_tuple(get_self(), receiver_ac, disburse_qty, 
								"VIGOR ICO contract disburses " + disburse_qty.to_string() + " to \'" + receiver_ac.to_string() + "\'. for " + buyorsell_type.to_string() + " in " + memo)
		).send();
	}

	fund_table.modify(fund_it, get_self(), [&](auto& row) {
		creatify_vector_pair_fund(row.tot_disburse_qty, phase_type, disburse_qty);
	});

}

void vigorico::disburse_inline(const name& receiver_ac,
						const name& buyorsell_type,
						const name& phase_type,
						const asset& disburse_qty,
						const string& memo )
{
	action(
		permission_level{get_self(), "active"_n},
		get_self(),
		"disburse"_n,
		std::make_tuple(receiver_ac, buyorsell_type, phase_type, disburse_qty, memo)
	).send();
}
// --------------------------------------------------------------------------------------------------------------------
void vigorico::sendalert(const name& user,
							const string& message) {
	require_auth(get_self());

	require_recipient(user);
}

void vigorico::send_alert(const name& user, 
							const string& message) {
	check(message.size() <= 256, "message has more than 256 bytes");
	
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"sendalert"_n,
		std::make_tuple(user, message)
	).send();
}

