#include "tropiumico.hpp"
#include <cmath>

using std::pow;

// --------------------------------------------------------------------------------------------------------------------
void tropiumico::seticorate( const name& phase_type,
								const asset& current_price )
{
	require_auth(dapp_token_issuer);

	check((phase_type == "a"_n) || (phase_type == "b"_n) || (phase_type == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	check_quantity(current_price, dapp_token_symbol);

	// admin_index admin_table(stake_contract_ac, stake_contract_ac.value);
	// auto admin_it = admin_table.find("doctor"_n.value);

	// check(admin_it != admin_table.end(), "set admins list using action - \'setadmins\' of stake contract.");

	icorate_index icorate_table(get_self(), get_self().value);
	auto ico_it = icorate_table.find(phase_type.value);

	if(ico_it == icorate_table.end()) {
		icorate_table.emplace(get_self(), [&](auto& row){
			row.phase_type = phase_type;
			row.current_price = current_price;
		});
	} else {
		check(ico_it->current_price != current_price, "the parsed current price is same as the set one.");

		icorate_table.modify(ico_it, get_self(), [&](auto& row){
			row.current_price = current_price;
		});
	}
}


// --------------------------------------------------------------------------------------------------------------------
void tropiumico::deposit( const name& buyer_ac, 
						const name& contract_ac, 
						const asset& quantity, 
						const string& memo ) {


	// check for conditions if either or both of these are true for `from` & `to`
	if(contract_ac != get_self() ||  buyer_ac == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the buyer.");
		return;
	}

	if(buyer_ac == dapp_token_issuer) {		// include the case where, additional ICO money with TROPIUM tokens needed after the contract is deployed on this a/c 
		return;
	} else {		

		check((memo == "phase A") || (memo == "phase B") || (memo == "phase C"), 
			"For sending to this contract, parsed memo can either be \'phase A' or \'phase B' or \'phase C'");

		name phase_type = ""_n;
		if(memo == "phase A") {
			phase_type = "a"_n;
		} else 	if(memo == "phase B") {
			phase_type = "b"_n;
		} else 	if(memo == "phase C") {
			phase_type = "c"_n;
		}

		// instantiate the `fund` table
		fund_index fund_table(get_self(), buyer_ac.value);
		auto fund_it = fund_table.find(phase_type.value);

		// update (add/modify) the deposit_qty
		if(fund_it == fund_table.end()) {
			fund_table.emplace(get_self(), [&](auto& row) {
				row.phase_type = phase_type;
				row.tot_deposit_qty = quantity;
				row.tot_disburse_qty.symbol = dapp_token_symbol;				
			});
		} else {
			fund_table.modify(fund_it, get_self(), [&](auto& row) {
				row.tot_deposit_qty += quantity;
			});
		}


		asset disburse_qty;

		// prepare for disbursement of dapp tokens as per ICO rate
		disburse_qty = asset(0, dapp_token_symbol);

		icorate_index icorate_table(get_self(), get_self().value);

		auto ico_it = icorate_table.find(phase_type.value);

		check(ico_it != icorate_table.end(), "ICO rate for phase " + phase_type.to_string() + " is not set.\'" + dapp_token_issuer.to_string() + "\' must set using \'seticorate\'");

		auto prec = ico_it->current_price.symbol.precision();
		disburse_qty.amount = quantity.amount * ico_it->current_price.amount/pow(10, prec);

		// inline disburse of dapp token based on the amount of EOS sent
		action(
			permission_level{get_self(), "active"_n},
			get_self(),
			"disburse"_n,
			std::make_tuple(buyer_ac, phase_type, disburse_qty, string("disburse for " + memo))
		).send();

		// send alert to buyer for receiving dapp token
		send_alert(buyer_ac, "You receive \'" + disburse_qty.to_string() + "\' for depositing \'" + 
									quantity.to_string() + "\' to ICO contract for " + memo );

	}

}


// --------------------------------------------------------------------------------------------------------------------
void tropiumico::disburse(const name& receiver_ac,
						const name& phase_type,
						const asset& disburse_qty,
						const string& memo )
{
	require_auth(get_self());

	check(is_account(receiver_ac), "receiver account doesn't exist");
	check( receiver_ac != get_self(), "amount can't be disbursed to contract itself");

	check((phase_type == "a"_n) || (phase_type == "b"_n) || (phase_type == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	// check quantity is valid for all conditions as 'asset'
	check_quantity(disburse_qty, dapp_token_symbol);

	// instantiate the `fund` table
	fund_index fund_table(get_self(), receiver_ac.value);
	auto fund_it = fund_table.find(phase_type.value);

	// Make sure that the receiver_ac has deposited amount in the table
	check( fund_it != fund_table.end(), "Sorry! There is no amount deposited by " + receiver_ac.to_string() + "in the fund.");

	action(
		permission_level{get_self(), "active"_n},
		token_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), receiver_ac, disburse_qty, 
							"TROPIUM ICO contract disburses " + disburse_qty.to_string() + " to \'" + receiver_ac.to_string() + "\'. for " + memo)
	).send();

	fund_table.modify(fund_it, get_self(), [&](auto& row) {
		row.tot_disburse_qty += disburse_qty;
	});

}

// --------------------------------------------------------------------------------------------------------------------
void tropiumico::sendalert(const name& user,
							const string& message) {
	require_auth(get_self());

	require_recipient(user);
}

void tropiumico::send_alert(const name& user, 
							const string& message) {
	check(message.size() <= 256, "message has more than 256 bytes");
	
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"sendalert"_n,
		std::make_tuple(user, message)
	).send();
}

