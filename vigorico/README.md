# ICO Contract
This contract is used for auto/manual disbursement of dApp tokens in exchange of EOS tokens (which can be modified) at ICO rates (set for multiple phases). 

## Features
* Auto-disbursement of VIGOR tokens from ICO contract after receiving EOS tokens from buyer.
* Also, Manual-disbursement of VIGOR tokens from ICO contract is possible, if `"vigorico::disburse"` action is not added as inline action, but rather  in the `vigorico` contract.
* ICO can be conducted in multiple phases completely independent of each other.

## Workflow
### Pre-requisite
* Create VIGOR token with attributes:
```md
"max_supply": "10 M",
"issuer": "vigor1issuer"
```
* Issue VIGOR token to the issuer: `"vigor1issuer"`
* Issuer transfer VIGOR tokens to the __ICO contract__ for auto-disbursement VIGOR tokens only after receiving EOS tokens from buyer.
* ICO rate has to be set by the contract owner for respective phases - A, B, C.

### Real time
* Buyer send EOS tokens & receive VIGOR tokens to/from contract respectively.
* Seller send VIGOR tokens & receive EOS tokens to/from contract respectively.

> NOTE: Anyone can be buyer/seller, no authentication required from dApp side.

## Compile
```console
$ eosio-cpp vigorico.cpp -o vigorico.wasm
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
$ cleost set contract vigor1111ico ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_vigor_contracts/vigorico/vigorico.wasm...
Skipping set abi because the new abi is the same as the existing abi
Publishing contract...
executed transaction: 4557cd4ab9cccc37c0414d06e29a5cf227abe5a2b2517505f909bc5e7e4fba72  16208 bytes  1449 us
#         eosio <= eosio::setcode               {"account":"vigor1111ico","vmtype":0,"vmversion":0,"code":"0061736d0100000001e7012360000060017e00600...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleost set account permission vigor1111ico active --add-code
executed transaction: 99ab0e7cff1339e4187a6ac8159e095c7d9f2050876ab6c8c4846b52dd7f26d7  184 bytes  195 us
#         eosio <= eosio::updateauth            {"account":"vigor1111ico","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `initicorate`
* set the ICO rate for phase A
```console
$ cleost push action vigor1111ico initicorate '{"buyorsell_type": "buy","phase_type": "a","current_price_pereos": 40,"vector_admin": ["vigoradmin11", "vigoradmin12", "vigoradmin13", "vigoradmin14", "vigoradmin15"]}' -p vigor1111ico@active
executed transaction: b8a284cd4e8a7f14f955f690ab7a679b8a091bae0ee622695679ca1ef2f5c526  160 bytes  227 us
#  vigor1111ico <= vigor1111ico::initicorate    {"buyorsell_type":"buy","phase_type":"a","current_price_pereos":"40.00000000000000000","vector_admin...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table
```console
$ cleost get table vigor1111ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price_pereos": "40.00000000000000000",
        "proposed_price_pereos": "0.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 0
      },
      "payer": "vigor1111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* Again set the ICO rate for phase A & get error:
```console
$ cleost push action vigor1111ico initicorate '{"buyorsell_type": "buy","phase_type": "a","current_price_pereos": 40,"vector_admin": ["vigoradmin11", "vigoradmin12", "vigoradmin13", "vigoradmin14", "vigoradmin15"]}' -p vigor1111ico@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: The row for this phase_type: a is already initialized.
pending console output:
```
* set the ICO rate for phase B
```console
$ cleost push action vigor1111ico initicorate '{"buyorsell_type": "buy","phase_type": "b","current_price_pereos": 50,"vector_admin": ["vigoradmin11", "vigoradmin12", "vigoradmin13", "vigoradmin14", "vigoradmin15"]}' -p vigor1111ico@active
executed transaction: 948c456a9504d283539d9aadd107cf551d8c8fe8d42d5252c5ca93314bb3ad90  160 bytes  171 us
#  vigor1111ico <= vigor1111ico::initicorate    {"buyorsell_type":"buy","phase_type":"b","current_price_pereos":"50.00000000000000000","vector_admin...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table
```console
$ cleost get table vigor1111ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price_pereos": "40.00000000000000000",
        "proposed_price_pereos": "0.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 0
      },
      "payer": "vigor1111ico"
    },{
      "data": {
        "phase_type": "b",
        "current_price_pereos": "50.00000000000000000",
        "proposed_price_pereos": "0.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 0
      },
      "payer": "vigor1111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `propoicorate`
* propose the ICO rate as "42.0" for phase A within 1 min
```console
$ cleost push action vigor1111ico propoicorate '["vigoradmin11", "buy", "a", "42.0", "1598997614"]' -p vigoradmin11@active
executed transaction: 55b9cbdc2eba319a398747802345ecb37bedb424360db74f0ac40c5c75556609  128 bytes  240 us
#  vigor1111ico <= vigor1111ico::propoicorate   {"setter":"vigoradmin11","buyorsell_type":"buy","phase_type":"a","proposed_price_pereos":"42.0000000...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the `icorates` table
```console
$ cleost get table vigor1111ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price_pereos": "40.00000000000000000",
        "proposed_price_pereos": "42.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 1598997614
      },
      "payer": "vigoradmin11"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* parsing same rate as previous in the ICO rate for phase A
```console
$ cleost push action vigor1111ico propoicorate '["vigoradmin11", "buy", "a", "42.0", "1598997614"]' -p vigoradmin11@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: The proposed price parsed is same as stored.
pending console output:
```

### Action - `voteicorate`
* `vigoradmin11` votes as "y" for the ongoing proposal
```console
$ cleost push action vigor1111ico voteicorate '["vigoradmin11", "buy", "a", "y"]' -p vigoradmin11@active
executed transaction: c6305ec073ff1261a7bb69f539f0e04fac3d0eb98a4ce6dbe0b68a278ee61af3  128 bytes  163 us
#  vigor1111ico <= vigor1111ico::voteicorate    {"admin":"vigoradmin11","buyorsell_type":"buy","phase_type":"a","vote":"y"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table
```console
$ cleost get table vigor1111ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price_pereos": "40.00000000000000000",
        "proposed_price_pereos": "42.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [{
            "first": "vigoradmin11",
            "second": "y"
          }
        ],
        "decision_timestamp": 1598999414
      },
      "payer": "vigoradmin11"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `vigoradmin12` votes as "n" for the ongoing proposal
```console
$ cleost push action vigor1111ico voteicorate '["vigoradmin12", "buy", "a", "n"]' -p vigoradmin12@active
executed transaction: a2f1e67108f20747a9a35f2fe7ce9a92d12e3e025b8b6da457cb5ec3f89b492e  128 bytes  191 us
#  vigor1111ico <= vigor1111ico::voteicorate    {"admin":"vigoradmin12","buyorsell_type":"buy","phase_type":"a","vote":"n"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table
```console
$ cleost get table vigor1111ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price_pereos": "40.00000000000000000",
        "proposed_price_pereos": "42.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [{
            "first": "vigoradmin11",
            "second": "y"
          },{
            "first": "vigoradmin12",
            "second": "n"
          }
        ],
        "decision_timestamp": 1598999414
      },
      "payer": "vigoradmin12"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `vigoradmin13` votes as "n" for the ongoing proposal
```console
$ cleost push action vigor1111ico voteicorate '["vigoradmin13", "buy", "a", "n"]' -p vigoradmin13@
active
executed transaction: b0ca6e6340bda8bbfc8a5f8bb811562b4f2d85b2d3f1e4ae8e47ddfe8c2f3781  128 bytes  142 us
#  vigor1111ico <= vigor1111ico::voteicorate    {"admin":"vigoradmin13","buyorsell_type":"buy","phase_type":"a","vote":"n"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table
```console
$ cleost get table vigor1111ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price_pereos": "40.00000000000000000",
        "proposed_price_pereos": "42.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [{
            "first": "vigoradmin11",
            "second": "y"
          },{
            "first": "vigoradmin12",
            "second": "n"
          },{
            "first": "vigoradmin13",
            "second": "n"
          }
        ],
        "decision_timestamp": 1598999414
      },
      "payer": "vigoradmin13"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `vigoradmin14` votes as "y" for the ongoing proposal, but can't vote as the time exceeded the voting time limit
```console
$ cleost push action vigor1111ico voteicorate '["vigoradmin14", "buy", "a", "y"]' -p vigoradmin14@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: 'voteicorate' action can be accessed within the 'decision_timestamp'.
pending console output:
```
* So, total 3 out of 5 admins voted. Now, let's see the result

### Action - `approicorate`
* contract approves the proposed ico rate based on the votes done
```console
cleost push action vigor1111ico approicorate '["buy", "a"]' -p vigor1111ico@active
```
  - view the `icorates` table
```console
$ cleost get table vigor1111ico buy icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price_pereos": "40.00000000000000000",
        "proposed_price_pereos": "42.00000000000000000",
        "vector_admin": [
          "vigoradmin11",
          "vigoradmin12",
          "vigoradmin13",
          "vigoradmin14",
          "vigoradmin15"
        ],
        "vector_admin_vote": [],
        "decision_timestamp": 1598999414
      },
      "payer": "vigor1111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - Observations:
    + current_price is not changed as proposed
    + the votes vector has been cleared. So, that the old votes are not considered for new proposals especially, if the admin didn't vote for the new proposal.

### Action - `deposit` (Payable action)
* `vigoruser111` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error due to unfit memo for this action 
```console
$ cleost push action eosio.token transfer '["vigoruser111", "vigor1111ico", "3.0000 EOS", "transfer EOS for ICO"]' -p vigoruser111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: For sending to this contract, parsed memo can either be 'buy for phase A' or 'phase B' or 'phase C'
pending console output:
```
* `vigoruser111` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error because the contract account was not transferred any dapp tokens by the issuer - `dcieosissuer`
```console
$ cleost push action eosio.token transfer '["vigoruser111", "vigor1111ico", "3.0000 EOS", "phase A"]' -p vigoruser111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: no balance object found
pending console output:
``` 
* `vigoruser111` successfully transfer some "3.0000 EOS" to ICO funding in phase-A
```console
$ cleost push action eosio.token transfer '["vigoruser111", "vigor1111ico", "3.0000 EOS", "phase A"]' -p vigoruser111@active
executed transaction: 854589a4c12e56f1c5eae702e86ab9ebce4b18c260793f6bdf1c5c7d3dbf369a  136 bytes  311 us
#   eosio.token <= eosio.token::transfer        {"from":"vigoruser111","to":"vigor1111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  vigoruser111 <= eosio.token::transfer        {"from":"vigoruser111","to":"vigor1111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  vigor1111ico <= eosio.token::transfer        {"from":"vigoruser111","to":"vigor1111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  vigor1111ico <= vigor1111ico::disburse       {"receiver_ac":"vigoruser111","buyorsell_type":"buy","phase_type":"a","disburse_qty":"120.0000 VIGOR...
#  vigor1111ico <= vigor1111ico::sendalert      {"user":"vigoruser111","message":"You receive '120.0000 VIGOR' for depositing '3.0000 EOS' to vigor ...
#  vigor11token <= vigor11token::transfer       {"from":"vigor1111ico","to":"vigoruser111","quantity":"120.0000 VIGOR","memo":"VIGOR ICO contract di...
#  vigor1111ico <= vigor11token::transfer       {"from":"vigor1111ico","to":"vigoruser111","quantity":"120.0000 VIGOR","memo":"VIGOR ICO contract di...
>> Either money is not sent to the contract or contract itself is the buyer.
#  vigoruser111 <= vigor11token::transfer       {"from":"vigor1111ico","to":"vigoruser111","quantity":"120.0000 VIGOR","memo":"VIGOR ICO contract di...
#  vigoruser111 <= vigor1111ico::sendalert      {"user":"vigoruser111","message":"You receive '120.0000 VIGOR' for depositing '3.0000 EOS' to vigor ...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the VIGOR balance of `vigoruser111` buyer
```console
$ cleost get table vigor11token vigoruser111 accounts
{
  "rows": [{
      "balance": "120.0000 VIGOR"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `vigoruser111` buyer
```console
$ cleost get table vigor1111ico vigoruser111 fund --show-payer
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
            "second": "120.0000 VIGOR"
          }
        ]
      },
      "payer": "vigor1111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

	- So, basically, buyer sends `3 EOS` & receives `120 VIGOR` tokens instead

* `vigoruser111` transfer some "3.0000 EOS" to ICO funding in phase B & gets error: ICO rate for phase B is not set
```console
$ cleost push action eosio.token transfer '["vigoruser111", "vigor1111ico", "3.0000 EOS", "phase B"]' -p vigoruser111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: ICO rate for phase b is not set. Please set using 'seticorate'
pending console output:
```
* `vigoruser111` transfer some "3.0000 EOS" to ICO funding in phase B
```console
$ cleost push action eosio.token transfer '["vigoruser111", "vigor1111ico", "3.0000 EOS", "phase B"]' -p vigoruser111@active
executed transaction: fd11a094dfa65b97b7bb718dc4cd876c932d3995a60b3f0d7c2848566b05b250  136 bytes  250 us
#   eosio.token <= eosio.token::transfer        {"from":"vigoruser111","to":"vigor1111ico","quantity":"3.0000 EOS","memo":"phase B"}
#  vigoruser111 <= eosio.token::transfer        {"from":"vigoruser111","to":"vigor1111ico","quantity":"3.0000 EOS","memo":"phase B"}
#  vigor1111ico <= eosio.token::transfer        {"from":"vigoruser111","to":"vigor1111ico","quantity":"3.0000 EOS","memo":"phase B"}
#  vigor1111ico <= vigor1111ico::disburse       {"receiver_ac":"vigoruser111","buyorsell_type":"buy","phase_type":"b","disburse_qty":"150.0000 VIGOR...
#  vigor1111ico <= vigor1111ico::sendalert      {"user":"vigoruser111","message":"You receive '150.0000 VIGOR' for depositing '3.0000 EOS' to vigor ...
#  vigor11token <= vigor11token::transfer       {"from":"vigor1111ico","to":"vigoruser111","quantity":"150.0000 VIGOR","memo":"VIGOR ICO contract di...
#  vigor1111ico <= vigor11token::transfer       {"from":"vigor1111ico","to":"vigoruser111","quantity":"150.0000 VIGOR","memo":"VIGOR ICO contract di...
>> Either money is not sent to the contract or contract itself is the buyer.
#  vigoruser111 <= vigor11token::transfer       {"from":"vigor1111ico","to":"vigoruser111","quantity":"150.0000 VIGOR","memo":"VIGOR ICO contract di...
#  vigoruser111 <= vigor1111ico::sendalert      {"user":"vigoruser111","message":"You receive '150.0000 VIGOR' for depositing '3.0000 EOS' to vigor ...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - View the VIGOR balance of `vigoruser111` buyer
```console
$ cleost get table vigor11token vigoruser111 accounts
{
  "rows": [{
      "balance": "270.0000 VIGOR"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `vigoruser111` buyer
```console
$ cleost get table vigor1111ico vigoruser111 fund --show-payer
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
            "second": "120.0000 VIGOR"
          },{
            "first": "b",
            "second": "150.0000 VIGOR"
          }
        ]
      },
      "payer": "vigor1111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```


## TODO
* [ ] Phase from_date, to_date for ICO

## Transaction History
1. For Jungle 3 Testnet, view [here](https://jungle3history.cryptolions.io/v2/history/get_actions?account=vigoruser111)
1. Copy the JSON response from above and paste it into [here](http://jsonviewer.stack.hu/) to see in JSON format.

