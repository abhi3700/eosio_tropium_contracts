# Stake Contract
* Patient stakes "5000 TRPM" tokens to get rehab from doctor.
* The assigned doctor `startrehab` & set the lock period of 30 days.
* The Dr. can `endrehab` by setting the status as "cured" or "caught".
* If cured, then Dr. gets 5% & remaining is returned back to the patient.
* Else if caught, then Dr. gets 100% tokens & the patient loses entirely.

## Compile
```console
$ eosio-cpp tropiumstake.cpp -o tropiumstake.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <setadmins> does not have a ricardian contract
Warning, action <addadmin> does not have a ricardian contract
Warning, action <remadmin> does not have a ricardian contract
Warning, action <clradmins> does not have a ricardian contract
Warning, action <startrehab> does not have a ricardian contract
Warning, action <endrehab> does not have a ricardian contract
Warning, action <unstake> does not have a ricardian contract
Warning, action <setadmins> does not have a ricardian contract
Warning, action <addadmin> does not have a ricardian contract
Warning, action <remadmin> does not have a ricardian contract
Warning, action <clradmins> does not have a ricardian contract
Warning, action <startrehab> does not have a ricardian contract
Warning, action <endrehab> does not have a ricardian contract
Warning, action <unstake> does not have a ricardian contract
```

## Deploy
* Deploy contract
```console
$ cleost set contract trpm111stake ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_tropium_contracts/tropiumstake/tropiumstake.wasm...
Publishing contract...
executed transaction: 140b1b3909e339087e107735210e912f34d915e92fea308277981bda2a831f41  11512 bytes  1027 us
#         eosio <= eosio::setcode               {"account":"trpm111stake","vmtype":0,"vmversion":0,"code":"0061736d0100000001c6011f60000060017e00600...
#         eosio <= eosio::setabi                {"account":"trpm111stake","abi":"0e656f73696f3a3a6162692f312e3100090861646461646d696e000306646f63746...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleost set account permission trpm111stake active --add-code
executed transaction: 2b070bd3086c310045e56ecae58674cc734d889aa5836c3ff1c381b21dcdc61b  184 bytes  149 us
#         eosio <= eosio::updateauth            {"account":"trpm111stake","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `setadmins`
* A doctor set admins list
```console
$ cleost push action trpm111stake setadmins '{"doctor": "trpmadmin111","type": "doctor","vector_admin": ["trpmadmin111", "trpmadmin112", "trpmadmin113", "trpmadmin114", "trpmadmin115"]}' -p trpmadmin111@active
executed transaction: 7b8f5d242655d39248695a20f7d377b1ad4dba6f366b1b1159689a21bb9ec0dd  152 bytes  272 us
#  trpm111stake <= trpm111stake::setadmins      {"doctor":"trpmadmin111","type":"doctor","vector_admin":["trpmadmin111","trpmadmin112","trpmadmin113...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "trpmadmin111",
          "trpmadmin112",
          "trpmadmin113",
          "trpmadmin114",
          "trpmadmin115"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* A doctor set same admins list & gets error:
```console
$ cleost push action trpm111stake setadmins '{"doctor": "trpmadmin111","type": "doctor","vector_admin": ["trpmadmin111", "trpmadmin112", "trpmadmin113", "trpmadmin114", "trpmadmin115"]}' -p trpmadmin111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the parsed admin list is same as the stored one.
pending console output:
```

### Action - `addadmin`
* add admin to the list
```console
cleost push action trpm111stake addadmin '["", ""]' -p 
```

### Action - `remadmin`
### Action - `clradmins`
### Action - `stake` (payable action)
### Action - `startrehab`
### Action - `endrehab`
### Action - `unstake`
