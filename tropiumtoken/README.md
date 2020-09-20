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
Reading WASM from /mnt/f/Coding/github_repos/eosio_vigor_contracts/vigortoken/vigortoken.wasm...
Publishing contract...
executed transaction: ca5d82d3a2bb04a1b7e3b40cbda6def2f90f0643c53d04c7270da75c6347af82  7016 bytes  733 us
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
$ cleost push action trpm111token create '["trpiumissuer", "10000000.0000 TRPM"]' -p trpm111token@active
executed transaction: 36f0c0e1965e0b65d393fd59fd1a4200549049acbd1f2c7afa492a7454e27b5c  120 bytes  229 us
#  trpm111token <= trpm111token::create         {"issuer":"trpiumissuer","maximum_supply":"10000000.0000 TRPM"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table after this operation
```console
$ cleost get table trpm111token TRPM stat
{
  "rows": [{
      "supply": "0.0000 TRPM",
      "max_supply": "10000000.0000 TRPM",
      "issuer": "trpiumissuer"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `issue`
* issue 1 M to issuer - `trpiumissuer`
```console
$ cleost push action trpm111token issue '["trpiumissuer", "1000000.0000 TRPM", "issue 1M tokens"]' -p trpiumissuer@active
executed transaction: c79ae107a26e4593bad0a97f2d6d273ff56aad17c04f5ff195f77a49a6cd9fae  136 bytes  192 us
#  trpm111token <= trpm111token::issue          {"to":"trpiumissuer","quantity":"1000000.0000 TRPM","memo":"issue 1M tokens"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table after this operation
```console
$ cleost get table trpm111token TRPM stat
{
  "rows": [{
      "supply": "1000000.0000 TRPM",
      "max_supply": "10000000.0000 TRPM",
      "issuer": "trpiumissuer"
    }
  ],
  "more": false,
  "next_key": ""
}
```


### Action - `transfer`
* issuer - `trpiumissuer` transfer some 10% of total 1M tokens i.e. 100,000 for ICO distribution
```console
$ cleost push action trpm111token transfer '["trpiumissuer", "tropium11ico", "100000.0000 TRPM", "transfer TRPM tokens for entire ICO"]' -p trpiumissuer@active
executed transaction: 655949506d3e00af3ccde9ad7fe8af7aff06b1f7c840611cf4ee2e000aa5acb8  160 bytes  223 us
#  trpm111token <= trpm111token::transfer       {"from":"trpiumissuer","to":"tropium11ico","quantity":"100000.0000 TRPM","memo":"transfer TRPM token...
#  trpiumissuer <= trpm111token::transfer       {"from":"trpiumissuer","to":"tropium11ico","quantity":"100000.0000 TRPM","memo":"transfer TRPM token...
#  tropium11ico <= trpm111token::transfer       {"from":"trpiumissuer","to":"tropium11ico","quantity":"100000.0000 TRPM","memo":"transfer TRPM token...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table of `trpiumissuer`
```console
$ cleost get table trpm111token trpiumissuer accounts
{
  "rows": [{
      "balance": "900000.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```

	- view the table of `tropium11ico`
```console
$ cleost get table trpm111token tropium11ico accounts
{
  "rows": [{
      "balance": "100000.0000 TRPM"
    }
  ],
  "more": false,
  "next_key": ""
}
```
