#include "tropiumico.hpp"
#include <cmath>

using std::pow;

// --------------------------------------------------------------------------------------------------------------------
void tropiumico::seticorate( const name& phase_type,
								const asset& current_price )
{
	require_auth(founder_ac);

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
void tropiumico::deposit( const name& user, 
						const name& contract_ac, 
						const asset& quantity, 
						const string& memo ) {


	// check for conditions if either or both of these are true for `from` & `to`
	if(contract_ac != get_self() ||  user == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the buyer.");
		return;
	}

	// founder_ac is the tokenissuer
	if(user == founder_ac) {		// include the case where, additional ICO money with TROPIUM tokens needed after the contract is deployed on this a/c 
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
		fund_index fund_table(get_self(), user.value);
		auto fund_it = fund_table.find(phase_type.value);

		// update (add/modify) the deposit_qty
		if(fund_it == fund_table.end()) {
			fund_table.emplace(get_self(), [&](auto& row) {
				row.fund_type = phase_type;
				row.tot_fundtype_qty = quantity;
				row.tot_disburse_qty.symbol = dapp_token_symbol;				
			});
		} else {
			fund_table.modify(fund_it, get_self(), [&](auto& row) {
				row.tot_fundtype_qty += quantity;
			});
		}


		asset disburse_asset;

		// prepare for disbursement of dapp tokens as per ICO rate
		disburse_asset = asset(0, dapp_token_symbol);

		icorate_index icorate_table(get_self(), get_self().value);

		auto ico_it = icorate_table.find(phase_type.value);

		check(ico_it != icorate_table.end(), "ICO rate for \'" + buyorsell_type.to_string() + "\' in phase " + phase_type.to_string() + " is not set. Contract owner must set using \'initicorate\'");

		auto prec = ico_it->current_price.symbol.precision();
		disburse_asset.amount = quantity.amount * ico_it->current_price.amount/pow(10, prec);

		// inline disburse of dapp token based on the amount of EOS sent
		disburse_inline(user, buyorsell_type, phase_type, disburse_asset, memo);

		// send alert to buyer for receiving dapp token
		send_alert(user, "You receive \'" + disburse_asset.to_string() + "\' for depositing \'" + 
									quantity.to_string() + "\' to ICO contract for " + buyorsell_type.to_string() + " in " + memo );

	}



}


// --------------------------------------------------------------------------------------------------------------------
void tropiumico::disburse(const name& receiver_ac,
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
								"TROPIUM ICO contract disburses " + disburse_qty.to_string() + " to \'" + receiver_ac.to_string() + "\'. for " + buyorsell_type.to_string() + " in " + memo)
		).send();
	} else if(buyorsell_type == "sell"_n) {
		action(
			permission_level{get_self(), "active"_n},
			"eosio.token"_n,
			"transfer"_n,
			std::make_tuple(get_self(), receiver_ac, disburse_qty, 
								"TROPIUM ICO contract disburses " + disburse_qty.to_string() + " to \'" + receiver_ac.to_string() + "\'. for " + buyorsell_type.to_string() + " in " + memo)
		).send();
	}

	fund_table.modify(fund_it, get_self(), [&](auto& row) {
		creatify_vector_pair_fund(row.tot_disburse_qty, phase_type, disburse_qty);
	});

}

void tropiumico::disburse_inline(const name& receiver_ac,
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

