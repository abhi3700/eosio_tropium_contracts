# ICO Contract
This contract is used for auto/manual disbursement of dApp tokens in exchange of EOS tokens (which can be modified) at ICO rates (set for multiple phases). 

## Features
* Auto-disbursement of TRPM tokens from ICO contract after receiving EOS tokens from buyer.
* Also, Manual-disbursement of TRPM tokens from ICO contract is possible, if `"vigorico::disburse"` action is not added as inline action, but rather  in the `vigorico` contract.
* ICO can be conducted in multiple phases completely independent of each other.

## Workflow
### Pre-requisite
* Create TRPM token with attributes:
```md
"max_supply": "10 M",
"issuer": "vigor1issuer"
```
* Issue TRPM token to the issuer: `"vigor1issuer"`
* Issuer transfer TRPM tokens to the __ICO contract__ for auto-disbursement TRPM tokens only after receiving EOS tokens from buyer.
* ICO rate has to be set by the contract owner for respective phases - A, B, C.

### Real time
* Buyer send EOS tokens & receive TRPM tokens to/from contract respectively.
* Seller send TRPM tokens & receive EOS tokens to/from contract respectively.

> NOTE: Anyone can be buyer/seller, no authentication required from dApp side.

## Compile
```console
$ eosio-cpp tropiumico.cpp -o tropiumico.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <initicorate> does not have a ricardian contract
Warning, action <propoicorate> does not have a ricardian contract
Warning, action <voteicorate> does not have a ricardian contract
Warning, action <approicorate> does not have a ricardian contract
Warning, action <disburse> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
Warning, action <testdelico> does not have a ricardian contract
Warning, action <testdelfund> does not have a ricardian contract
Warning, action <initicorate> does not have a ricardian contract
Warning, action <propoicorate> does not have a ricardian contract
Warning, action <voteicorate> does not have a ricardian contract
Warning, action <approicorate> does not have a ricardian contract
Warning, action <disburse> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
```

## Deploy
* Deploy contract
```console
$ cleost set contract tropium11ico ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_tropium_contracts/tropiumico/tropiumico.wasm...
Skipping set abi because the new abi is the same as the existing abi
Publishing contract...
executed transaction: 861d72a4a2356aae6f5a73b82698043c409e2df91c6720eb56c128dbf1b33926  18752 bytes  1558 us
#         eosio <= eosio::setcode               {"account":"tropium11ico","vmtype":0,"vmversion":0,"code":"0061736d0100000001e7012460000060017e00600...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleost set account permission tropium11ico active --add-code
executed transaction: 83ae48d8378929ccd450639210ca64730699a88dba60283d775b239d991dae78  184 bytes  145 us
#         eosio <= eosio::updateauth            {"account":"tropium11ico","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `initicorate`
* set the ICO rate for phase A & gets error as the admins are not set in the stake contract
```console
$ cleost push action tropium11ico initicorate '["buy", "a", "40.0000 TRPM"]' -p tropium11ico@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: set admins list using action - 'setadmins' of stake contract.
pending console output:
```
* set the ICO rate for phase A
```console
$ cleost push action tropium11ico initicorate '["buy", "a", "40.0000 TRPM"]' -p tropium11ico@active
executed transaction: e3bbb12859b04e36bb83d287e1291e2d2e1b7b2aefc97524606dc6c967ebad91  128 bytes  295 us
#  tropium11ico <= tropium11ico::initicorate    {"buyorsell_type":"buy","phase_type":"a","current_price":"40.0000 TRPM"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "40.0000 TRPM",
        "proposed_price": "0.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 0
      },
      "payer": "tropium11ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* Again set the ICO rate again for phase A & get error:
```console
$ cleost push action tropium11ico initicorate '["buy", "a", "40.0000 TRPM"]' -p tropium11ico@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: The row for this phase_type: a is already initialized.
pending console output:
```
* set the ICO rate for phase B
```console
$ cleost push action tropium11ico initicorate '["buy", "b", "50.0000 TRPM"]' -p tropium11ico@active
executed transaction: c0916cd78c4340dab848f45d68b238df6c0358c864d3f6a5f435fe2f6960b342  128 bytes  220 us
#  tropium11ico <= tropium11ico::initicorate    {"buyorsell_type":"buy","phase_type":"b","current_price":"50.0000 TRPM"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "40.0000 TRPM",
        "proposed_price": "0.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 0
      },
      "payer": "tropium11ico"
    },{
      "data": {
        "phase_type": "b",
        "current_price": "50.0000 TRPM",
        "proposed_price": "0.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 0
      },
      "payer": "tropium11ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

#### For SELL
* set the ICO rate for phase A
```console
$ cleost push action tropium11ico initicorate '["sell", "a", "0.0500 EOS"]' -p tropium11ico@active
executed transaction: e013da31fa701f0d0c6b98d0126c23f0ad65c9d3b68c4f585ab622a6c04ef359  128 bytes  196 us
#  tropium11ico <= tropium11ico::initicorate    {"buyorsell_type":"sell","phase_type":"a","current_price":"0.0500 EOS"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table
```console
$ cleost get table tropium11ico sell icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "0.0500 EOS",
        "proposed_price": "0.0000 EOS",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 0
      },
      "payer": "tropium11ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `propoicorate`
* propose the ICO rate as "42.0" for phase A within 1 min
```console
$ cleost push action tropium11ico propoicorate '["trpmadmin111", "buy", "a", "42.0000 TRPM", "1599343200"]' -p trpmadmin111@active
executed transaction: 6196e99f6ad23a129b33e080d6b0c7c4eb2e5b17d07277445c749c2d57b1f5b4  136 bytes  140 us
#  tropium11ico <= tropium11ico::propoicorate   {"setter":"trpmadmin111","buyorsell_type":"buy","phase_type":"a","proposed_price":"42.0000 TRPM","de...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the `icorates` table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "40.0000 TRPM",
        "proposed_price": "42.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 1599343200
      },
      "payer": "trpmadmin111"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* parsing same rate as previous in the ICO rate for phase A
```console
$ cleost push action tropium11ico propoicorate '["trpmadmin111", "buy", "a", "42.0000 TRPM", "1599343200"]' -p trpmadmin111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: Either parsed proposed_price or decision_timestamp must be different.
pending console output:
```

### Action - `voteicorate`
* `trpmadmin111` votes as "y" for the ongoing proposal
```console
$ cleost push action tropium11ico voteicorate '["trpmadmin111", "buy", "a", "y"]' -p trpmadmin111@active
executed transaction: 23dafaeec094d43b5069c376bdf21dd35605104cf650018b3e611d0d437ffcd1  128 bytes  163 us
#  tropium11ico <= tropium11ico::voteicorate    {"admin":"trpmadmin111","buyorsell_type":"buy","phase_type":"a","vote":"y"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "40.0000 TRPM",
        "proposed_price": "42.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [{
            "first": "trpmadmin111",
            "second": "y"
          }
        ],
        "decision_timestamp": 1599343200
      },
      "payer": "trpmadmin111"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `trpmadmin112` votes as "n" for the ongoing proposal
```console
$ cleost push action tropium11ico voteicorate '["trpmadmin112", "buy", "a", "n"]' -p trpmadmin112@active
executed transaction: b14fba247e35d7846bf9fcb8d97fe0c1166640748d0859d9896852f0060cb567  128 bytes  193 us
#  tropium11ico <= tropium11ico::voteicorate    {"admin":"trpmadmin112","buyorsell_type":"buy","phase_type":"a","vote":"n"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "40.0000 TRPM",
        "proposed_price": "42.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [{
            "first": "trpmadmin111",
            "second": "y"
          },{
            "first": "trpmadmin112",
            "second": "n"
          }
        ],
        "decision_timestamp": 1599343200
      },
      "payer": "trpmadmin112"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `trpmadmin113` votes as "n" for the ongoing proposal
```console
$ cleost push action tropium11ico voteicorate '["trpmadmin113", "buy", "a", "n"]' -p trpmadmin113@active
executed transaction: 4f60d63391466b00b157b875842257e7c4ac0c60a950ecbdaf29b92a981e255b  128 bytes  153 us
#  tropium11ico <= tropium11ico::voteicorate    {"admin":"trpmadmin113","buyorsell_type":"buy","phase_type":"a","vote":"n"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "40.0000 TRPM",
        "proposed_price": "42.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [{
            "first": "trpmadmin111",
            "second": "y"
          },{
            "first": "trpmadmin112",
            "second": "n"
          },{
            "first": "trpmadmin113",
            "second": "n"
          }
        ],
        "decision_timestamp": 1599343200
      },
      "payer": "trpmadmin113"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `trpmadmin114` votes as "y" for the ongoing proposal
```console
$ cleost push action tropium11ico voteicorate '["trpmadmin114", "buy", "a", "y"]' -p trpmadmin114@active
executed transaction: 54b376ef5836439aa4274a1e07a3a3afe4a00bef8ad2846c65a30fed979e4cec  128 bytes  204 us
#  tropium11ico <= tropium11ico::voteicorate    {"admin":"trpmadmin114","buyorsell_type":"buy","phase_type":"a","vote":"y"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* `trpmadmin114` can't vote again for the ongoing proposal
```console
$ cleost push action tropium11ico voteicorate '["trpmadmin114", "buy", "a", "y"]' -p trpmadmin114@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: this vote already exists in the table.
pending console output:
```
* `trpmadmin114` votes as "y" for the ongoing proposal, but can't vote as the time exceeded the voting time limit
```console
$ cleost push action tropium11ico voteicorate '["trpmadmin114", "buy", "a", "y"]' -p trpmadmin114@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: 'voteicorate' action can be accessed within the 'decision_timestamp'.
pending console output:
```
  - view the `icorates` table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "40.0000 TRPM",
        "proposed_price": "42.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [{
            "first": "trpmadmin111",
            "second": "y"
          },{
            "first": "trpmadmin112",
            "second": "n"
          },{
            "first": "trpmadmin113",
            "second": "n"
          },{
            "first": "trpmadmin114",
            "second": "y"
          }
        ],
        "decision_timestamp": 1599343200
      },
      "payer": "trpmadmin114"
    }
  ],
  "more": false,
  "next_key": ""
}
```

* So, total 4 out of 5 admins voted. Now, let's see the result

### Action - `approicorate`
* contract approves the proposed ico rate based on the votes done
```console
$ cleost push action tropium11ico approicorate '["buy", "a"]' -p tropium11ico@active
executed transaction: 29ec5e8c6312035c39c97fe2be507f5ebaec11e3284b2707bb7d2d41dbdf3c21  112 bytes  189 us
#  tropium11ico <= tropium11ico::approicorate   {"buyorsell_type":"buy","phase_type":"a"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table
```console
$ cleost get table tropium11ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "42.0000 TRPM",
        "proposed_price": "42.0000 TRPM",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 1599343200
      },
      "payer": "tropium11ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - Observations:
    + current_price is changed as proposed
    + the votes vector has been cleared. So, that the old votes are not considered for new proposals especially, if the admin didn't vote for the new proposal.

### Action - `deposit` (Payable action)
#### For BUY
* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error due to unfit memo for this action 
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "tropium11ico", "3.0000 EOS", "transfer EOS for ICO"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: For sending to this contract, parsed memo can either be 'buy for phase A' or 'phase B' or 'phase C'
pending console output:
```
* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error because the contract account was not transferred any dapp tokens by the issuer - `dcieosissuer`
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "tropium11ico", "3.0000 EOS", "phase A"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: no balance object found
pending console output:
``` 
* `trpmuser1112` successfully transfer some "3.0000 EOS" to ICO funding in phase-A
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "tropium11ico", "3.0000 EOS", "phase A"]' -p trpmuser1112@active
executed transaction: eaf47b27efcd8a12b416710badc32dc30cfa88d101980d23108144782be3879b  136 bytes  368 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"tropium11ico","quantity":"3.0000 EOS","memo":"phase A"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"tropium11ico","quantity":"3.0000 EOS","memo":"phase A"}
#  tropium11ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"tropium11ico","quantity":"3.0000 EOS","memo":"phase A"}
#  tropium11ico <= tropium11ico::disburse       {"receiver_ac":"trpmuser1112","buyorsell_type":"buy","phase_type":"a","disburse_qty":"126.0000 TRPM"...
#  tropium11ico <= tropium11ico::sendalert      {"user":"trpmuser1112","message":"You receive '126.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
#  trpm111token <= trpm111token::transfer       {"from":"tropium11ico","to":"trpmuser1112","quantity":"126.0000 TRPM","memo":"TROPIUM ICO contract d...
#  tropium11ico <= trpm111token::transfer       {"from":"tropium11ico","to":"trpmuser1112","quantity":"126.0000 TRPM","memo":"TROPIUM ICO contract d...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmuser1112 <= trpm111token::transfer       {"from":"tropium11ico","to":"trpmuser1112","quantity":"126.0000 TRPM","memo":"TROPIUM ICO contract d...
#  trpmuser1112 <= tropium11ico::sendalert      {"user":"trpmuser1112","message":"You receive '126.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the TRPM balance of `trpmuser1112` buyer
```console
$ cleost get table trpm111token trpmuser1112 accounts
{
  "rows": [{
      "balance": "126.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `trpmuser1112` buyer
```console
$ cleost get table tropium11ico trpmuser1112 fund --show-payer
{
  "rows": [{
      "data": {
        "fund_type": "buy",
        "tot_fundtype_qty": [{
            "first": "a",
            "second": "3.0000 EOS"
          }
        ],
        "tot_disburse_qty": [{
            "first": "a",
            "second": "126.0000 TRPM"
          }
        ]
      },
      "payer": "tropium11ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

	- So, basically, buyer sends `3.0000 EOS` & receives `126.0000 TRPM` tokens instead

* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase B & gets error: ICO rate for phase B is not set
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "tropium11ico", "3.0000 EOS", "phase B"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: ICO rate for phase b is not set. Please set using 'seticorate'
pending console output:
```
* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase B
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "tropium11ico", "3.0000 EOS", "phase B"]' -p trpmuser1112@active
executed transaction: 09c0e161da782bd189d786f33faa43dcb05ab77d888616d8e2e9aa40f5c94707  136 bytes  323 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"tropium11ico","quantity":"3.0000 EOS","memo":"phase B"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"tropium11ico","quantity":"3.0000 EOS","memo":"phase B"}
#  tropium11ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"tropium11ico","quantity":"3.0000 EOS","memo":"phase B"}
#  tropium11ico <= tropium11ico::disburse       {"receiver_ac":"trpmuser1112","buyorsell_type":"buy","phase_type":"b","disburse_qty":"150.0000 TRPM"...
#  tropium11ico <= tropium11ico::sendalert      {"user":"trpmuser1112","message":"You receive '150.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
#  trpm111token <= trpm111token::transfer       {"from":"tropium11ico","to":"trpmuser1112","quantity":"150.0000 TRPM","memo":"TROPIUM ICO contract d...
#  tropium11ico <= trpm111token::transfer       {"from":"tropium11ico","to":"trpmuser1112","quantity":"150.0000 TRPM","memo":"TROPIUM ICO contract d...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmuser1112 <= trpm111token::transfer       {"from":"tropium11ico","to":"trpmuser1112","quantity":"150.0000 TRPM","memo":"TROPIUM ICO contract d...
#  trpmuser1112 <= tropium11ico::sendalert      {"user":"trpmuser1112","message":"You receive '150.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - View the TRPM balance of `trpmuser1112` buyer
```console
$ cleost get table trpm111token trpmuser1112 accounts
{
  "rows": [{
      "balance": "276.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `trpmuser1112` buyer
```console
$ cleost get table tropium11ico trpmuser1112 fund --show-payer
{
  "rows": [{
      "data": {
        "fund_type": "buy",
        "tot_fundtype_qty": [{
            "first": "a",
            "second": "3.0000 EOS"
          },{
            "first": "b",
            "second": "3.0000 EOS"
          }
        ],
        "tot_disburse_qty": [{
            "first": "a",
            "second": "126.0000 TRPM"
          },{
            "first": "b",
            "second": "150.0000 TRPM"
          }
        ]
      },
      "payer": "tropium11ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

#### For SELL
* `trpmuser1112` wants to sell some TRPM tokens.
  - let's see the TRPM balance
```console
$ cleost get table trpm111token trpmuser1112 accounts
{
  "rows": [{
      "balance": "276.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* Now, `trpmuser1112` wants to sell "50 TRPM" tokens at the rate of phase A & gets error as the ICO rate for phase A, sell is not set
```console
$ cleost push action trpm111token transfer '["trpmuser1112", "tropium11ico", "50.0000 TRPM", "phase A"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: ICO rate for 'sell' in phase a is not set. Contract owner must set using 'initicorate'
pending console output:
```
* Now, `trpmuser1112` wants to sell "5 TRPM" tokens at the rate of phase A
```console
$ cleost push action trpm111token transfer '["trpmuser1112", "tropium11ico", "5.0000 TRPM", "phase A"]' -p trpmuser1112@active
executed transaction: b329d0f0a97717a6ab0078d6024c18b4ac5f64021aca3b82a9b9ae99ac64e302  136 bytes  284 us
#  trpm111token <= trpm111token::transfer       {"from":"trpmuser1112","to":"tropium11ico","quantity":"5.0000 TRPM","memo":"phase A"}
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpmuser1112","to":"tropium11ico","quantity":"5.0000 TRPM","memo":"phase A"}
#  tropium11ico <= trpm111token::transfer       {"from":"trpmuser1112","to":"tropium11ico","quantity":"5.0000 TRPM","memo":"phase A"}
#  tropium11ico <= tropium11ico::disburse       {"receiver_ac":"trpmuser1112","buyorsell_type":"sell","phase_type":"a","disburse_qty":"0.2500 EOS","...
#  tropium11ico <= tropium11ico::sendalert      {"user":"trpmuser1112","message":"You receive '0.2500 EOS' for depositing '5.0000 TRPM' to ICO contr...
#   eosio.token <= eosio.token::transfer        {"from":"tropium11ico","to":"trpmuser1112","quantity":"0.2500 EOS","memo":"TROPIUM ICO contract disb...
#  tropium11ico <= eosio.token::transfer        {"from":"tropium11ico","to":"trpmuser1112","quantity":"0.2500 EOS","memo":"TROPIUM ICO contract disb...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmuser1112 <= eosio.token::transfer        {"from":"tropium11ico","to":"trpmuser1112","quantity":"0.2500 EOS","memo":"TROPIUM ICO contract disb...
#  trpmuser1112 <= tropium11ico::sendalert      {"user":"trpmuser1112","message":"You receive '0.2500 EOS' for depositing '5.0000 TRPM' to ICO contr...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `fund` table
```console
$ cleost get table tropium11ico trpmuser1112 fund --show-payer
{
  "rows": [{
      "data": {
        "fund_type": "buy",
        "tot_fundtype_qty": [{
            "first": "a",
            "second": "3.0000 EOS"
          },{
            "first": "b",
            "second": "3.0000 EOS"
          }
        ],
        "tot_disburse_qty": [{
            "first": "a",
            "second": "126.0000 TRPM"
          },{
            "first": "b",
            "second": "150.0000 TRPM"
          }
        ]
      },
      "payer": "tropium11ico"
    },{
      "data": {
        "fund_type": "sell",
        "tot_fundtype_qty": [{
            "first": "a",
            "second": "5.0000 TRPM"
          }
        ],
        "tot_disburse_qty": [{
            "first": "a",
            "second": "0.2500 EOS"
          }
        ]
      },
      "payer": "tropium11ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

## TODO
* [ ] Phase from_date, to_date for ICO

## Transaction History
1. For Jungle 3 Testnet, view [here](https://jungle3history.cryptolions.io/v2/history/get_actions?account=trpmuser1112)
1. Copy the JSON response from above and paste it into [here](http://jsonviewer.stack.hu/) to see in JSON format.

