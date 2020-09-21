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
#### For BUY
* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error due to unfit memo for this action 
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "3.0000 EOS", "transfer EOS for ICO"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: For sending to this contract, parsed memo can either be 'buy for phase A' or 'phase B' or 'phase C'
pending console output:
```
* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase-A & gets error because the contract account was not transferred any dapp tokens by the issuer - `dcieosissuer`
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "3.0000 EOS", "phase A"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: no balance object found
pending console output:
``` 
* `trpmuser1112` successfully transfer some "3.0000 EOS" to ICO funding in phase-A
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "3.0000 EOS", "phase A"]' -p trpmuser1112@active
executed transaction: eaf47b27efcd8a12b416710badc32dc30cfa88d101980d23108144782be3879b  136 bytes  368 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  trpm11111ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase A"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1112","buyorsell_type":"buy","phase_type":"a","disburse_qty":"126.0000 TRPM"...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '126.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
#  trpm111token <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"126.0000 TRPM","memo":"TROPIUM ICO contract d...
#  trpm11111ico <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"126.0000 TRPM","memo":"TROPIUM ICO contract d...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"126.0000 TRPM","memo":"TROPIUM ICO contract d...
#  trpmuser1112 <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '126.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
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
$ cleost get table trpm11111ico trpmuser1112 fund --show-payer
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
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```

	- So, basically, buyer sends `3.0000 EOS` & receives `126.0000 TRPM` tokens instead

* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase B & gets error: ICO rate for phase B is not set
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "3.0000 EOS", "phase B"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: ICO rate for phase b is not set. Please set using 'seticorate'
pending console output:
```
* `trpmuser1112` transfer some "3.0000 EOS" to ICO funding in phase B
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "3.0000 EOS", "phase B"]' -p trpmuser1112@active
executed transaction: 09c0e161da782bd189d786f33faa43dcb05ab77d888616d8e2e9aa40f5c94707  136 bytes  323 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase B"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase B"}
#  trpm11111ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"3.0000 EOS","memo":"phase B"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1112","buyorsell_type":"buy","phase_type":"b","disburse_qty":"150.0000 TRPM"...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '150.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
#  trpm111token <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"150.0000 TRPM","memo":"TROPIUM ICO contract d...
#  trpm11111ico <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"150.0000 TRPM","memo":"TROPIUM ICO contract d...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"150.0000 TRPM","memo":"TROPIUM ICO contract d...
#  trpmuser1112 <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '150.0000 TRPM' for depositing '3.0000 EOS' to ICO con...
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
$ cleost get table trpm11111ico trpmuser1112 fund --show-payer
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
      "payer": "trpm11111ico"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `trpmuser1112` wants to buy "5000 TRPM" to join rehab centre. So, he transfers "119.0476 EOS" @ phas_type A rate
```console
$ cleost push action eosio.token transfer '["trpmuser1112", "trpm11111ico", "119.0476 EOS", "phase A"]' -p trpmuser1112@active
executed transaction: 83f00f9fab72fb34daa355ed00d8267b8a9dcf796360ded5d73fae191a301ddf  136 bytes  314 us
#   eosio.token <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"119.0476 EOS","memo":"phase A"}
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"119.0476 EOS","memo":"phase A"}
#  trpm11111ico <= eosio.token::transfer        {"from":"trpmuser1112","to":"trpm11111ico","quantity":"119.0476 EOS","memo":"phase A"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1112","buyorsell_type":"buy","phase_type":"a","disburse_qty":"4999.9992 TRPM...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '4999.9992 TRPM' for depositing '119.0476 EOS' to ICO ...
#  trpm111token <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"4999.9992 TRPM","memo":"TROPIUM ICO contract ...
#  trpm11111ico <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"4999.9992 TRPM","memo":"TROPIUM ICO contract ...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpm11111ico","to":"trpmuser1112","quantity":"4999.9992 TRPM","memo":"TROPIUM ICO contract ...
#  trpmuser1112 <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '4999.9992 TRPM' for depositing '119.0476 EOS' to ICO ...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
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
$ cleost push action trpm111token transfer '["trpmuser1112", "trpm11111ico", "50.0000 TRPM", "phase A"]' -p trpmuser1112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: ICO rate for 'sell' in phase a is not set. Contract owner must set using 'initicorate'
pending console output:
```
* Now, `trpmuser1112` wants to sell "5 TRPM" tokens at the rate of phase A
```console
$ cleost push action trpm111token transfer '["trpmuser1112", "trpm11111ico", "5.0000 TRPM", "phase A"]' -p trpmuser1112@active
executed transaction: b329d0f0a97717a6ab0078d6024c18b4ac5f64021aca3b82a9b9ae99ac64e302  136 bytes  284 us
#  trpm111token <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm11111ico","quantity":"5.0000 TRPM","memo":"phase A"}
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm11111ico","quantity":"5.0000 TRPM","memo":"phase A"}
#  trpm11111ico <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm11111ico","quantity":"5.0000 TRPM","memo":"phase A"}
#  trpm11111ico <= trpm11111ico::disburse       {"receiver_ac":"trpmuser1112","buyorsell_type":"sell","phase_type":"a","disburse_qty":"0.2500 EOS","...
#  trpm11111ico <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '0.2500 EOS' for depositing '5.0000 TRPM' to ICO contr...
#   eosio.token <= eosio.token::transfer        {"from":"trpm11111ico","to":"trpmuser1112","quantity":"0.2500 EOS","memo":"TROPIUM ICO contract disb...
#  trpm11111ico <= eosio.token::transfer        {"from":"trpm11111ico","to":"trpmuser1112","quantity":"0.2500 EOS","memo":"TROPIUM ICO contract disb...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmuser1112 <= eosio.token::transfer        {"from":"trpm11111ico","to":"trpmuser1112","quantity":"0.2500 EOS","memo":"TROPIUM ICO contract disb...
#  trpmuser1112 <= trpm11111ico::sendalert      {"user":"trpmuser1112","message":"You receive '0.2500 EOS' for depositing '5.0000 TRPM' to ICO contr...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the `fund` table
```console
$ cleost get table trpm11111ico trpmuser1112 fund --show-payer
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
      "payer": "trpm11111ico"
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
      "payer": "trpm11111ico"
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

