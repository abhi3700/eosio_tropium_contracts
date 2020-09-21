# ICO Contract
This contract is used for auto disbursement of dApp tokens in exchange of EOS tokens (which can be modified) at ICO rates (set for multiple phases). 

## Features
* Auto-disbursement of TRPM tokens from ICO contract after receiving EOS tokens from buyer.
* Also, Manual-disbursement of TRPM tokens from ICO contract is possible, if `"vigorico::disburse"` action is not added as inline action, but rather  in the `vigorico` contract.
* ICO can be conducted in multiple phases completely independent of each other.
* ICO rate is - 0.15 EOS/1 TRPM

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

> NOTE: Anyone can be buyer, no authentication required from dApp side.

## Compile
```console
$ eosio-cpp tropiumico.cpp -o tropiumico.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <seticorate> does not have a ricardian contract
Warning, action <disburse> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
Warning, action <testdelico> does not have a ricardian contract
Warning, action <testdelfund> does not have a ricardian contract
Warning, action <seticorate> does not have a ricardian contract
Warning, action <disburse> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
```

## Deploy
* Deploy contract
```console
$ cleost set contract trpm11111ico ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_tropium_contracts/tropiumico/tropiumico.wasm...
Publishing contract...
executed transaction: c68fffdd79a59a94c3ef364b3120a065d23e64be10336717d80feca0d624a4be  12512 bytes  994 us
#         eosio <= eosio::setcode               {"account":"trpm11111ico","vmtype":0,"vmversion":0,"code":"0061736d0100000001de012360000060017e00600...
#         eosio <= eosio::setabi                {"account":"trpm11111ico","abi":"0e656f73696f3a3a6162692f312e31000708646973627572736500040b726563656...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleost set account permission trpm11111ico active --add-code
executed transaction: a19c54ad8a97cf7475b8cd61f6a09273fa906d63f536ea4d7adf43b3f29a95c0  184 bytes  146 us
#         eosio <= eosio::updateauth            {"account":"trpm11111ico","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `seticorate`
* `tropiumchain` set the ICO rate for phase A
```console
$ cleost push action trpm11111ico seticorate '["a", "6.6700 TRPM"]' -p tropiumchain@active
executed transaction: c5b80350bcd1571637ebad4cee64a4626693df8dd57cb94f2c3e540d7214a751  120 bytes  163 us
#  trpm11111ico <= trpm11111ico::seticorate     {"phase_type":"a","current_price":"6.6700 TRPM"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table
```console
$ cleost get table trpm11111ico trpm11111ico icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "6.6700 TRPM"
      },
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* Again set the ICO rate again for phase A & get error:
```console
$ cleost push action trpm11111ico seticorate '["a", "6.6700 TRPM"]' -p tropiumchain@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the parsed current price is same as the set one.
pending console output:
```
* `tropiumchain` set the ICO rate for phase B
```console
$ cleost push action trpm11111ico seticorate '["b", "9.6700 TRPM"]' -p tropiumchain@active
executed transaction: 2dd8fd735a77c08c9bc891f707614a7a61badc3c4ec700930647de7abc20d241  120 bytes  168 us
#  trpm11111ico <= trpm11111ico::seticorate     {"phase_type":"b","current_price":"9.6700 TRPM"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table
```console
$ cleost get table trpm11111ico trpm11111ico icorates --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "current_price": "6.6700 TRPM"
      },
      "payer": "trpm11111ico"
    },{
      "data": {
        "phase_type": "b",
        "current_price": "9.6700 TRPM"
      },
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```


### Action - `deposit` (Payable action)
* `trpmuser1111` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error due to unfit memo for this action 
```console
$ cleost push action eosio.token transfer '["trpmuser1111", "trpm11111ico", "3.0000 EOS", "transfer EOS for ICO"]' -p trpmuser1111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: For sending to this contract, parsed memo can either be 'phase A' or 'phase B' or 'phase C'
pending console output:
```
* `trpmuser1111` transfer some "3.0000 EOS" to ICO funding in phase-A
```console
$ cleost push action eosio.token transfer '["trpmuser1111", "trpm11111ico", "3.0000 EOS", "phase A"]' -p trpmuser1111@active
executed transaction: 4db8a386f077e00c08b7c28442fa662ad6b4ae3df65a99af8d9f62c4a09f4ba4  136 bytes  402 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1111","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  trpmuser1111 <= eosio.token::transfer        {"from":"trpmuser1111","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  trpm11111ico <= eosio.token::transfer        {"from":"trpmuser1111","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1111","phase_type":"a","disburse_qty":"20.0100 TRPM","memo":"disburse for ph...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1111","message":"You receive '20.0100 TRPM' for depositing '3.0000 EOS' to ICO cont...
#  trpm111token <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1111","quantity":"20.0100 TRPM","memo":"TROPIUM ICO contract di...
#  trpm11111ico <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1111","quantity":"20.0100 TRPM","memo":"TROPIUM ICO contract di...
#  trpmuser1111 <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1111","quantity":"20.0100 TRPM","memo":"TROPIUM ICO contract di...
#  trpmuser1111 <= trpm11111ico::sendalert      {"user":"trpmuser1111","message":"You receive '20.0100 TRPM' for depositing '3.0000 EOS' to ICO cont...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
``` 
  - View the TRPM balance of `trpmuser1111` buyer
```console
$ cleost get table trpm111token trpmuser1111 accounts
{
  "rows": [{
      "balance": "20.0100 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `trpmuser1112` buyer
```console
$ cleost get table trpm11111ico trpmuser1111 fund --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "tot_deposit_qty": "3.0000 EOS",
        "tot_disburse_qty": "20.0100 TRPM"
      },
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - So, basically, buyer sends `3.0000 EOS` & receives `20.0100 TRPM` tokens instead

* `trpmuser1112` successfully transfer some "5.0000 EOS" to ICO funding in phase-A
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "5.0000 EOS", "phase A"]' -p trpmuser1112@active
executed transaction: d0e1035419466302143090d4c02992d3185e64f5d9bf278f3b6f08c265b84fa5  136 bytes  360 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"5.0000 EOS","memo":"phase A"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"5.0000 EOS","memo":"phase A"}
#  trpm11111ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"5.0000 EOS","memo":"phase A"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1112","phase_type":"a","disburse_qty":"33.3500 TRPM","memo":"disburse for ph...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '33.3500 TRPM' for depositing '5.0000 EOS' to ICO cont...
#  trpm111token <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"33.3500 TRPM","memo":"TROPIUM ICO contract di...
#  trpm11111ico <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"33.3500 TRPM","memo":"TROPIUM ICO contract di...
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"33.3500 TRPM","memo":"TROPIUM ICO contract di...
#  trpmuser1112 <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '33.3500 TRPM' for depositing '5.0000 EOS' to ICO cont...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - View the TRPM balance of `trpmuser1112` buyer
```console
$ cleost get table trpm111token trpmuser1112 accounts
{
  "rows": [{
      "balance": "33.3500 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `trpmuser1112` buyer
```console
$ cleost get table trpm11111ico trpmuser1112 fund --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "tot_deposit_qty": "5.0000 EOS",
        "tot_disburse_qty": "33.3500 TRPM"
      },
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - So, basically, buyer sends `5.0000 EOS` & receives `33.3500 TRPM` tokens instead

* `trpmuser1112` transfer some "8.0000 EOS" to ICO funding in phase B
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "8.0000 EOS", "phase B"]' -p trpmuser1112@active
executed transaction: f946f128e4cb250e5af213e8275b9d9e940ee0a6fc062d6347989e06713b3dcd  136 bytes  288 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"8.0000 EOS","memo":"phase B"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"8.0000 EOS","memo":"phase B"}
#  trpm11111ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"8.0000 EOS","memo":"phase B"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1112","phase_type":"b","disburse_qty":"77.3600 TRPM","memo":"disburse for ph...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '77.3600 TRPM' for depositing '8.0000 EOS' to ICO cont...
#  trpm111token <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"77.3600 TRPM","memo":"TROPIUM ICO contract di...
#  trpm11111ico <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"77.3600 TRPM","memo":"TROPIUM ICO contract di...
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"77.3600 TRPM","memo":"TROPIUM ICO contract di...
#  trpmuser1112 <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '77.3600 TRPM' for depositing '8.0000 EOS' to ICO cont...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - View the TRPM balance of `trpmuser1112` buyer
```console
$ cleost get table trpm111token trpmuser1112 accounts
{
  "rows": [{
      "balance": "110.7100 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `trpmuser1112` buyer
```console
$ cleost get table trpm11111ico trpmuser1112 fund --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "tot_deposit_qty": "5.0000 EOS",
        "tot_disburse_qty": "33.3500 TRPM"
      },
      "payer": "trpm11111ico"
    },{
      "data": {
        "phase_type": "b",
        "tot_deposit_qty": "8.0000 EOS",
        "tot_disburse_qty": "77.3600 TRPM"
      },
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `trpmuser1112` wants to buy "500 TRPM" to join rehab centre. So, he transfers "75.0000 EOS" @ phase_type A rate
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "75.0000 EOS", "phase A"]' -p trpmuser1112@active
executed transaction: a89c5827370a153130e20abe0fba13bec0f09e1aaed2d8d0fddc5a3f85504e59  136 bytes  353 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"75.0000 EOS","memo":"phase A"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"75.0000 EOS","memo":"phase A"}
#  trpm11111ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"75.0000 EOS","memo":"phase A"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1112","phase_type":"a","disburse_qty":"500.2500 TRPM","memo":"disburse for p...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '500.2500 TRPM' for depositing '75.0000 EOS' to ICO co...
#  trpm111token <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"500.2500 TRPM","memo":"TROPIUM ICO contract d...
#  trpm11111ico <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"500.2500 TRPM","memo":"TROPIUM ICO contract d...
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"500.2500 TRPM","memo":"TROPIUM ICO contract d...
#  trpmuser1112 <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '500.2500 TRPM' for depositing '75.0000 EOS' to ICO co...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - View the TRPM balance of `trpmuser1112` buyer
```console
$ cleost get table trpm111token trpmuser1112 accounts
{
  "rows": [{
      "balance": "610.9600 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the fund table of `trpmuser1112` buyer
```console
$ cleost get table trpm11111ico trpmuser1112 fund --show-payer
{
  "rows": [{
      "data": {
        "phase_type": "a",
        "tot_deposit_qty": "80.0000 EOS",
        "tot_disburse_qty": "533.6000 TRPM"
      },
      "payer": "trpm11111ico"
    },{
      "data": {
        "phase_type": "b",
        "tot_deposit_qty": "8.0000 EOS",
        "tot_disburse_qty": "77.3600 TRPM"
      },
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - Here, note that the total balance in TRPM matches with the sum of disbursement of phase A & B till now.
  - This situation is only possible, if the user doesn't receive TRPM tokens from any other source.

* `trpmuser1112` transfer some "10.0000 EOS" to ICO funding in phase C
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "10.0000 EOS", "phase C"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: ICO rate for phase c is not set.'tropiumchain' must set using 'seticorate'
pending console output:
```


## TODO
* [ ] Phase from_date, to_date for ICO

## Transaction History
1. For Jungle 3 Testnet, view [here](https://jungle3history.cryptolions.io/v2/history/get_actions?account=trpmuser1112)
1. Copy the JSON response from above and paste it into [here](http://jsonviewer.stack.hu/) to see in JSON format.

