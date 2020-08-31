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
Publishing contract...
executed transaction: e1dba7df60f6d00b0e11a34c185205690fc4236ffe8ea48182cf94998a641c5b  16352 bytes  1388 us
#         eosio <= eosio::setcode               {"account":"vigor1111ico","vmtype":0,"vmversion":0,"code":"0061736d0100000001e7012360000060017e00600...
#         eosio <= eosio::setabi                {"account":"vigor1111ico","abi":"0e656f73696f3a3a6162692f312e31000c0c617070726f69636f7261746500020e6...
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

### Action - `seticorate`
* set the ICO rate for phase A
```console
$ cleost push action dci111111ico seticorate '["a", "30.00"]' -p dci111111ico@active
executed transaction: 22890b024612c833b57bf65a58a2f29bb6152f2adbd5af6212b7e70b8a45f20b  104 bytes  277 us
#  dci111111ico <= dci111111ico::seticorate     {"phase":"a","price_pereos":"30.00000000000000000"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the `icorates` table
```console
$ cleost get table dci111111ico dci111111ico icorates
{
  "rows": [{
      "phase": "a",
      "price_pereos": "30.00000000000000000"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* update the ICO rate for phase A
```console
$ cleost push action dci111111ico seticorate '["a", "40.00"]' -p dci111111ico@active
executed transaction: 59e23528e5455a813c629160194443fb064c1fb013a369a879be0dfeebfbca0f  104 bytes  186 us
#  dci111111ico <= dci111111ico::seticorate     {"phase":"a","price_pereos":"40.00000000000000000"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the `icorates` table now,
```console
$ cleost get table dci111111ico dci111111ico icorates
{
  "rows": [{
      "phase": "a",
      "price_pereos": "40.00000000000000000"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* set the ICO rate for phase B
```console
$ cleost push action dci111111ico seticorate '["b", "53.00"]' -p dci111111ico@active
executed transaction: f29ccff99f8e39f9091931173652b5a1a11dbf73f54ed1047cf9c796cefbfb96  104 bytes  224 us
#  dci111111ico <= dci111111ico::seticorate     {"phase":"b","price_pereos":"53.00000000000000000"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `icorates` table now,
```console
$ cleost get table dci111111ico dci111111ico icorates
{
  "rows": [{
      "phase": "a",
      "price_pereos": "40.00000000000000000"
    },{
      "phase": "b",
      "price_pereos": "53.00000000000000000"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `deposit` (Payable action)
* `dciuser11111` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error due to unfit memo for this action 
```console
$ cleost push action eosio.token transfer '["dciuser11111", "dci111111ico", "3.0000 EOS", "transfer EOS for
ICO"]' -p dciuser11111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: For sending to this contract, parsed memo can either be 'phase A' or 'phase B' or 'phase C'
pending console output:
```
* `dciuser11111` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error because the contract account was not transferred any dapp tokens by the issuer - `dcieosissuer`
```console
$ cleost push action eosio.token transfer '["dciuser11111", "dci111111ico", "3.0000 EOS", "phase A"]' -p dciuser11111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: no balance object found
pending console output:
``` 
	- First, issuer - `dcieosissuer` transfer some 10% of total 1M tokens i.e. 1,00,000 for ICO distribution in phase A
```console
$ cleost push action dci1111token transfer '["dcieosissuer", "dci111111ico", "100000.0000 VIGOR", "transfer DC
I tokens for ICO phase A"]' -p dcieosissuer@active
executed transaction: e4334c81c4f2894c8de88c8e7561ebcb6e7be4705156e1e71a0d0483cb58686d  160 bytes  214 us
#  dci1111token <= dci1111token::transfer       {"from":"dcieosissuer","to":"dci111111ico","quantity":"100000.0000 VIGOR","memo":"transfer VIGOR tokens ...
#  dcieosissuer <= dci1111token::transfer       {"from":"dcieosissuer","to":"dci111111ico","quantity":"100000.0000 VIGOR","memo":"transfer VIGOR tokens ...
#  dci111111ico <= dci1111token::transfer       {"from":"dcieosissuer","to":"dci111111ico","quantity":"100000.0000 VIGOR","memo":"transfer VIGOR tokens ...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* `dciuser11111` successfully transfer some "3.0000 EOS" to ICO funding in phase-A
```console
$ cleost push action eosio.token transfer '["dciuser11111", "dci111111ico", "3.0000 EOS", "phase A"]' -p dciuser11111@active
executed transaction: 120b1b2bd3ba4a49601c6aa080c6f0ae7df3bed9c84bd4172e6fa06a9f5dd712  136 bytes  358 us
#   eosio.token <= eosio.token::transfer        {"from":"dciuser11111","to":"dci111111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  dciuser11111 <= eosio.token::transfer        {"from":"dciuser11111","to":"dci111111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  dci111111ico <= eosio.token::transfer        {"from":"dciuser11111","to":"dci111111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  dci111111ico <= dci111111ico::disburse       {"receiver_ac":"dciuser11111","phase":"a","disburse_qty":"120.0000 VIGOR","memo":"phase A"}
#  dci111111ico <= dci111111ico::sendalert      {"user":"dciuser11111","message":"You receive '120.0000 VIGOR' for depositing '3.0000 EOS' to VIGOR ICO ...
#  dci1111token <= dci1111token::transfer       {"from":"dci111111ico","to":"dciuser11111","quantity":"120.0000 VIGOR","memo":"VIGOR ICO contract disbur...
#  dci111111ico <= dci1111token::transfer       {"from":"dci111111ico","to":"dciuser11111","quantity":"120.0000 VIGOR","memo":"VIGOR ICO contract disbur...
#  dciuser11111 <= dci1111token::transfer       {"from":"dci111111ico","to":"dciuser11111","quantity":"120.0000 VIGOR","memo":"VIGOR ICO contract disbur...
#  dciuser11111 <= dci111111ico::sendalert      {"user":"dciuser11111","message":"You receive '120.0000 VIGOR' for depositing '3.0000 EOS' to VIGOR ICO ...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the VIGOR balance of `dciuser11111` buyer
```console
$ cleost get table dci1111token dciuser11111 accounts
{
  "rows": [{
      "balance": "120.0000 VIGOR"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- So, basically, buyer sends `3 EOS` & receives `120 VIGOR` tokens instead
* `dciuser11112` successfully transfer some "5.0000 EOS" to ICO funding in phase-B
```console
$ cleost push action eosio.token transfer '["dciuser11112", "dci111111ico", "5.0000 EOS", "phase B"]' -p dci
user11112@active
executed transaction: 524005b2059de939c79fc923394d543c7bd2d888fe3bdaabda562aa18f07949f  136 bytes  417 us
#   eosio.token <= eosio.token::transfer        {"from":"dciuser11112","to":"dci111111ico","quantity":"5.0000 EOS","memo":"phase B"}
#  dciuser11112 <= eosio.token::transfer        {"from":"dciuser11112","to":"dci111111ico","quantity":"5.0000 EOS","memo":"phase B"}
#  dci111111ico <= eosio.token::transfer        {"from":"dciuser11112","to":"dci111111ico","quantity":"5.0000 EOS","memo":"phase B"}
#  dci111111ico <= dci111111ico::disburse       {"receiver_ac":"dciuser11112","phase":"b","disburse_qty":"350.0000 VIGOR","memo":"phase B"}
#  dci111111ico <= dci111111ico::sendalert      {"user":"dciuser11112","message":"You receive '350.0000 VIGOR' for depositing '5.0000 EOS' to VIGOR ICO ...
#  dci1111token <= dci1111token::transfer       {"from":"dci111111ico","to":"dciuser11112","quantity":"350.0000 VIGOR","memo":"VIGOR ICO contract disbur...
#  dci111111ico <= dci1111token::transfer       {"from":"dci111111ico","to":"dciuser11112","quantity":"350.0000 VIGOR","memo":"VIGOR ICO contract disbur...
#  dciuser11112 <= dci1111token::transfer       {"from":"dci111111ico","to":"dciuser11112","quantity":"350.0000 VIGOR","memo":"VIGOR ICO contract disbur...
#  dciuser11112 <= dci111111ico::sendalert      {"user":"dciuser11112","message":"You receive '350.0000 VIGOR' for depositing '5.0000 EOS' to VIGOR ICO ...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - View the VIGOR balance of `dciuser11112` buyer
```console
$ cleost get table dci1111token dciuser11112 accounts
{
  "rows": [{
      "balance": "350.0000 VIGOR"
    }
  ],
  "more": false,
  "next_key": ""
}
```


## TODO
* [ ] Phase from_date, to_date
* [ ] Phase-wise total amount disbursement in form of vector of pairs. E.g. 
```md
"deposit_qty": ["a", "1.0000 EOS", "b": "30.0000 EOS"]
"disburse_qty": ["a", "14.0000 VIGOR", "b": "200.0000 VIGOR"]
```
* [ ] Store all the fund record into different scope of same table as per phases. As of now, all the fund_transferred are added up & cumulative is shown.

## Transaction History
1. For Jungle 3 Testnet, view [here](https://jungle3history.cryptolions.io/v2/history/get_actions?account=dciuser11112)
1. Copy the JSON response from above and paste it into [here](http://jsonviewer.stack.hu/) to see in JSON format.

