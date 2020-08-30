#include "dciico.hpp"

// --------------------------------------------------------------------------------------------------------------------
void dciico::deposit( const name& buyer_ac, 
						const name& contract_ac, 
						const asset& quantity, 
						const string& memo ) {

	// check for conditions if either or both of these are true for `from` & `to`
	if(contract_ac != get_self() ||  buyer_ac == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the buyer.");
		return;
	}

	// Although this is checked in "dcitoken::transfer" action, but fund_token_symbol check is pending. 
	// So, in addition the entire asset check is done using static func defined in "dciico.hpp" file.
	// check quantity is valid for all conditions as 'asset'
	check_quantity(quantity, fund_token_symbol);

	// instantiate the `fund` table
	fund_index fund_table(get_self(), buyer_ac.value);
	auto fund_it = fund_table.find(fund_token_symbol.raw());

	// update (add/modify) the deposit_qty
	if(fund_it == fund_table.end()) {
		fund_table.emplace(get_self(), [&](auto& row) {
			row.deposit_qty = quantity;
			row.disburse_qty.symbol = dapp_token_symbol;		// initialize the asset with symbol "DCI" & '0' amount 
		});
	} else {
		fund_table.modify(fund_it, get_self(), [&](auto& row) {
			row.deposit_qty += quantity;
		});
	}

	// prepare for disbursement of dapp tokens as per ICO rate
	auto disburse_asset = asset(0, dapp_token_symbol);
	

	icorate_index icorate_table(get_self(), get_self().value);

	check((memo == "phase A") || (memo == "phase B") || (memo == "phase C"), 
		"For sending to this contract, parsed memo can either be \'phase A' or \'phase B' or \'phase C'");
	
	if(memo == "phase A") {
		auto ico_it = icorate_table.find("a"_n.value);

		check(ico_it != icorate_table.end(), "ICO rate for this phase is not set. Please set using \'seticorate\'");

		disburse_asset.amount = quantity.amount * ico_it->price_pereos;

		// inline disburse of dapp token based on the amount of EOS sent
		disburse_inline(buyer_ac, "a"_n, disburse_asset, memo);
	} else if(memo == "phase B") {
		auto ico_it = icorate_table.find("b"_n.value);

		check(ico_it != icorate_table.end(), "ICO rate for this phase is not set. Please set using \'seticorate\'");

		disburse_asset.amount = quantity.amount * ico_it->price_pereos;

		// inline disburse of dapp token based on the amount of EOS sent
		disburse_inline(buyer_ac, "b"_n, disburse_asset, memo);
	} else if(memo == "phase C") {
		auto ico_it = icorate_table.find("c"_n.value);

		check(ico_it != icorate_table.end(), "ICO rate for this phase is not set. Please set using \'seticorate\'");

		disburse_asset.amount = quantity.amount * ico_it->price_pereos;

		// inline disburse of dapp token based on the amount of EOS sent
		disburse_inline(buyer_ac, "c"_n, disburse_asset, memo);
	}

	// send alert to buyer for receiving dapp token
	send_alert(buyer_ac, "You receive \'" + disburse_asset.to_string() + "\' for depositing \'" + 
								quantity.to_string() + "\' to DCI ICO fund in " + memo );
}


// --------------------------------------------------------------------------------------------------------------------
void dciico::seticorate( const name& phase,
						float price_pereos )
{
	require_auth(get_self());

	check((phase == "a"_n) || (phase == "b"_n) || (phase == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	icorate_index icorate_table(get_self(), get_self().value);
	auto ico_it = icorate_table.find(phase.value);

	if(ico_it == icorate_table.end()) {
		icorate_table.emplace(get_self(), [&](auto& row) {
			row.phase = phase;
			row.price_pereos = price_pereos;
		});
	} else {
		check(price_pereos != ico_it->price_pereos, "The price parsed is same as stored.");
		icorate_table.modify(ico_it, same_payer, [&](auto& row){
			row.price_pereos = price_pereos;
		});
	}
}

// --------------------------------------------------------------------------------------------------------------------
void dciico::disburse(const name& receiver_ac,
						const name& phase,
						const asset& disburse_qty,
						const string& memo )
{
	require_auth(get_self());

	// require_recipient(receiver_ac);

	check(is_account(receiver_ac), "receiver account doesn't exist");
	check( receiver_ac != get_self(), "amount can't be disbursed to contract itself");

	check((phase == "a"_n) || (phase == "b"_n) || (phase == "c"_n), "Phases can either be \'a\' or \'b\' or \'c\'.");

	// check quantity is valid for all conditions as 'asset'
	check_quantity(disburse_qty, dapp_token_symbol);

	// instantiate the `fund` table
	fund_index fund_table(get_self(), receiver_ac.value);
	auto fund_it = fund_table.find(fund_token_symbol.raw());

	// Make sure that the receiver_ac has deposited amount in the table
	check( fund_it != fund_table.end(), "Sorry! There is no amount deposited by " + receiver_ac.to_string() + "in the fund.");

	action(
		permission_level{get_self(), "active"_n},
		token_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), receiver_ac, disburse_qty, 
							"DCI ICO contract disburses " + disburse_qty.to_string() + " to \'" + receiver_ac.to_string() + "\'. for contribution in ICO " + memo)
	).send();

	fund_table.modify(fund_it, get_self(), [&](auto& row) {
		row.disburse_qty += disburse_qty; 
	});

}

void dciico::disburse_inline(const name& receiver_ac,
						const name& phase,
						const asset& disburse_qty,
						const string& memo )
{
	action(
		permission_level{get_self(), "active"_n},
		get_self(),
		"disburse"_n,
		std::make_tuple(receiver_ac, phase, disburse_qty, memo)
	).send();
}
// --------------------------------------------------------------------------------------------------------------------
void dciico::sendalert(const name& user,
							const string& message) {
	require_auth(get_self());

	require_recipient(user);
}

void dciico::send_alert(const name& user, 
							const string& message) {
	check(message.size() <= 256, "message has more than 256 bytes");
	
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"sendalert"_n,
		std::make_tuple(user, message)
		).send();
}

