# Token contract
## Brief
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
$ cleost set contract vigor11token ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_vigor_contracts/vigortoken/vigortoken.wasm...
Publishing contract...
executed transaction: ca5d82d3a2bb04a1b7e3b40cbda6def2f90f0643c53d04c7270da75c6347af82  7016 bytes  733 us
#         eosio <= eosio::setcode               {"account":"vigor11token","vmtype":0,"vmversion":0,"code":"0061736d0100000001a0011b60000060017e00600...
#         eosio <= eosio::setabi                {"account":"vigor11token","abi":"0e656f73696f3a3a6162692f312e310008076163636f756e7400010762616c616e6...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleost set account permission vigor11token active --add-code
executed transaction: b32e9611db31a58822c8e57a505a616382ac843b568c2487274858f744171c82  184 bytes  205 us
#         eosio <= eosio::updateauth            {"account":"vigor11token","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `create`
* create the token 1 M tokens for 10 years. Total 10 M tokens
```console
$ cleost push action vigor11token create '["vigor1issuer", "10000000.0000 VIGOR"]' -p vigor11token@active
executed transaction: d2522d99db139d48bf63dc69289e7bc69ace168d62b436a16fcce6401c28f1c4  120 bytes  194 us
#  vigor11token <= vigor11token::create         {"issuer":"vigor1issuer","maximum_supply":"10000000.0000 VIGOR"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table after this operation
```console
$ cleost get table vigor11token VIGOR stat
{
  "rows": [{
      "supply": "0.0000 VIGOR",
      "max_supply": "10000000.0000 VIGOR",
      "issuer": "vigor1issuer"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `issue`
* issue 1 M to issuer - `vigor1issuer`
```console
$ cleost push action vigor11token issue '["vigor1issuer", "1000000.0000 VIGOR", "issue 1M tokens"]' -p vigor1issuer@active
executed transaction: 4157609766fe0e45c61eabb52c1e4a9ddbbcf9af51010161001c3c5886a1d8c4  136 bytes  176 us
#  vigor11token <= vigor11token::issue          {"to":"vigor1issuer","quantity":"1000000.0000 VIGOR","memo":"issue 1M tokens"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table after this operation
```console
$ cleost get table vigor11token VIGOR stat
{
  "rows": [{
      "supply": "1000000.0000 VIGOR",
      "max_supply": "10000000.0000 VIGOR",
      "issuer": "vigor1issuer"
    }
  ],
  "more": false,
  "next_key": ""
}
```


### Action - `transfer`
* issuer - `vigor1issuer` transfer some 10% of total 1M tokens i.e. 100,000 for ICO distribution in phase A
```console
$ cleost push action vigor11token transfer '["vigor1issuer", "vigor1111ico", "100000.0000 VIGOR", "transfer VIGOR tokens for entire ICO phase A"]' -p vigor1issuer@active
executed transaction: 18614c0a30c462df9a823cfca6f201bad299ce72d78d02228490b9d19bbc318d  176 bytes  219 us
#  vigor11token <= vigor11token::transfer       {"from":"vigor1issuer","to":"vigor1111ico","quantity":"100000.0000 VIGOR","memo":"transfer VIGOR tok...
#  vigor1issuer <= vigor11token::transfer       {"from":"vigor1issuer","to":"vigor1111ico","quantity":"100000.0000 VIGOR","memo":"transfer VIGOR tok...
#  vigor1111ico <= vigor11token::transfer       {"from":"vigor1issuer","to":"vigor1111ico","quantity":"100000.0000 VIGOR","memo":"transfer VIGOR tok...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table of `vigor1issuer`
```console
$ cleost get table vigor11token vigor1issuer accounts
{
  "rows": [{
      "balance": "900000.0000 VIGOR"
    }
  ],
  "more": false,
  "next_key": ""
}
```

	- view the table of `vigor1111ico`
```console
$ cleost get table vigor11token vigor1111ico accounts
{
  "rows": [{
      "balance": "100000.0000 VIGOR"
    }
  ],
  "more": false,
  "next_key": ""
}
```
