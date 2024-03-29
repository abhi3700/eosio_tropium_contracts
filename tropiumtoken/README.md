# Token contract
## Brief
* the total_supply is 5 billion, max_supply is 15 billion.
* It is a token contract which is to 
	- [x] create tokens with issuer & max_supply
	- [x] issue tokens to issuer only
	- [x] retire tokens from supply
	- [x] transfer tokens from one account to another
	- [x] open account with even zero balance
	- [x] close account with zero balance

## About
* contract name - `toetoken`
* contract's account name - `toe1111token`
* action
	- `create`
	- `issue`
	- `retire`
	- `transfer`
	- `open`
	- `close`
* table
	- `stats`
	- `accounts`

## Compile
```console
$ eosio-cpp vigortoken.cpp -o vigortoken.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <create> does not have a ricardian contract
Warning, action <issue> does not have a ricardian contract
Warning, action <retire> does not have a ricardian contract
Warning, action <transfer> does not have a ricardian contract
Warning, action <open> does not have a ricardian contract
Warning, action <close> does not have a ricardian contract
Warning, action <create> does not have a ricardian contract
Warning, action <issue> does not have a ricardian contract
Warning, action <retire> does not have a ricardian contract
Warning, action <transfer> does not have a ricardian contract
Warning, action <open> does not have a ricardian contract
Warning, action <close> does not have a ricardian contract
```

## Deploy
* deploy contract
```console
$ cleost set contract trpm111token ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_tropium_contracts/tropiumtoken/tropiumtoken.wasm...
Publishing contract...
executed transaction: 3ad34ba072a42c1dc881f99f1458a4da07b8cfba2f3f4737300661da444a7525  7016 bytes  767 us
#         eosio <= eosio::setcode               {"account":"trpm111token","vmtype":0,"vmversion":0,"code":"0061736d0100000001a0011b60000060017e00600...
#         eosio <= eosio::setabi                {"account":"trpm111token","abi":"0e656f73696f3a3a6162692f312e310008076163636f756e7400010762616c616e6...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleost set account permission trpm111token active --add-code
executed transaction: be0f656bff17c776eb3ab851fe9d3eb42b41a35732681e4b762f733c0099c279  184 bytes  172 us
#         eosio <= eosio::updateauth            {"account":"trpm111token","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `create`
* create the token 1 M tokens for 10 years. Total 10 M tokens
```console
$ cleost push action trpm111token create '["tropiumchain", "15000000000.0000 TRPM"]' -p trpm
111token@active
executed transaction: 977c3910058acbc23a3d9c85292b75ab05078c0ab495e57f557b3b3bcde8a063  120 bytes  152 us
#  trpm111token <= trpm111token::create         {"issuer":"tropiumchain","maximum_supply":"15000000000.0000 TRPM"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table `stat` after this operation
```console
$ cleost get table trpm111token TRPM stat
{
  "rows": [{
      "supply": "0.0000 TRPM",
      "max_supply": "15000000000.0000 TRPM",
      "issuer": "tropiumchain"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `issue`
* issue 5 B to issuer - `tropiumchain`
```console
$ cleost push action trpm111token issue '["tropiumchain", "5000000000.0000 TRPM", "issue 5 Billions tokens"]' -p tropiumchain@active
executed transaction: c79ae107a26e4593bad0a97f2d6d273ff56aad17c04f5ff195f77a49a6cd9fae  136 bytes  192 us
#  trpm111token <= trpm111token::issue          {"to":"tropiumchain","quantity":"1000000.0000 TRPM","memo":"issue 1M tokens"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table `stat` after this operation
```console
$ cleost get table trpm111token TRPM stat
{
  "rows": [{
      "supply": "5000000000.0000 TRPM",
      "max_supply": "15000000000.0000 TRPM",
      "issuer": "tropiumchain"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - view the table `accounts`
```console
$ cleost get table trpm111token tropiumchain accounts
{
  "rows": [{
      "balance": "5000000000.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```


### Action - `transfer`
* issuer - `tropiumchain` transfer some 10% of total 5B tokens i.e. 500,000,000 for ICO distribution to contract ac - `trpm11111ico` (after the contract was deployed)
```console
$ cleost push action trpm111token transfer '["tropiumchain", "trpm11111ico", "500000000.0000 TRPM", "transfer TRPM tokens for entire ICO"]' -p tropiumchain@active
executed transaction: 98c43779a97bf785565d2cd4ff72eb33b869dc3ef2806ac60e7461e21187fd1b  160 bytes  218 us
#  trpm111token <= trpm111token::transfer       {"from":"tropiumchain","to":"trpm11111ico","quantity":"500000000.0000 TRPM","memo":"transfer TRPM to...
#  tropiumchain <= trpm111token::transfer       {"from":"tropiumchain","to":"trpm11111ico","quantity":"500000000.0000 TRPM","memo":"transfer TRPM to...
#  trpm11111ico <= trpm111token::transfer       {"from":"tropiumchain","to":"trpm11111ico","quantity":"500000000.0000 TRPM","memo":"transfer TRPM to...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table of `tropiumchain`
```console
$ cleost get table trpm111token tropiumchain accounts
{
  "rows": [{
      "balance": "4500000000.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```

	- view the table of `tropium11ico`
```console
$ cleost get table trpm111token trpm11111ico accounts
{
  "rows": [{
      "balance": "500000000.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
