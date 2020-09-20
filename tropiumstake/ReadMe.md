# Stake Contract
* A doctor registers by giving NPI no.
* An admin
* Patient stakes "5000 TRPM" tokens to get rehab from doctor.
* The assigned doctor `startrehab` & set the lock period of 30 days.
* The Dr. can `endrehab` by setting the status as "cured" or "caught".
* If cured, then Dr. gets 5% & remaining is returned back to the patient.
* Else if caught, then Dr. gets 100% tokens & the patient loses entirely.
* A doctor can be removed from the admins list when the no. is 5 

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
$ cleost push action trpm111stake setadmins '{"type": "doctor","vector_admin": ["trpmadmin111", "trpmadmin112", "trpmadmin113", "trpmadmin114", "trpmadmin115"]}' -p trpm111stake@active
executed transaction: f37ab7821fa82d7ef1472368c93b0c57963bbff118c78f3b4df11a5abcc289e8  144 bytes  185 us
#  trpm111stake <= trpm111stake::setadmins      {"type":"doctor","vector_admin":["trpmadmin111","trpmadmin112","trpmadmin113","trpmadmin114","trpmad...
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
$ cleost push action trpm111stake setadmins '{"type": "doctor","vector_admin": ["trpmadmin111", "trpmadmin112", "trpmadmin113", "trpmadmin114", "trpmadmin115"]}' -p trpm111stake@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the parsed admin list is same as the stored one.
pending console output:
```

### Action - `addadmin`
* `trpm111stake` add admin to the list
```console
$ cleost push action trpm111stake addadmin '["doctor", "trpmadmin121"]' -p trpm111stake@active
executed transaction: e5fc5dd51952b1c59ed94a9948c3ca9df9db5c0d102cfd516de692ca38e9536c  112 bytes  145 us
#  trpm111stake <= trpm111stake::addadmin       {"type":"doctor","admin":"trpmadmin121"}
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
          "trpmadmin115",
          "trpmadmin121"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* `trpm111stake` add same admin to the list & gets error:
```console
$ cleost push action trpm111stake addadmin '["doctor", "trpmadmin121"]' -p trpm111stake@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the parsed admin is already in the list.
pending console output:
```

### Action - `remadmin`
* `trpm111stake` remove admin - `trpmadmin121` from the list
```console
$ cleost push action trpm111stake remadmin '["doctor", "trpmadmin121"]' -p trpm111stake@active
executed transaction: a4b6b5d1b1cf13b6ecbf68c1f00b743e3c5f4de626b0c4f77fb04b6b1cd5f390  112 bytes  176 us
#  trpm111stake <= trpm111stake::remadmin       {"type":"doctor","admin":"trpmadmin121"}
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
* `trpm111stake` removes same admin - `trpmadmin121` from the list & gets error:
```console
$ cleost push action trpm111stake remadmin '["doctor", "trpmadmin121"]' -p trpm111stake@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the parsed admin is not in the list.
pending console output:
```

### Action - `clradmins`
```console
$ cleost push action trpm111stake clradmins '["doctor"]' -p trpm111stake@active
executed transaction: 6f3fbd9e31462acf6497c32d671511d96548d68120c990588445dfb594df514c  104 bytes  206 us
#  trpm111stake <= trpm111stake::clradmins      {"type":"doctor"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": []
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `stake` (payable action)
* Patient `trpmuser1112` wants to join the rehab centre. So, transfers "5000 TRPM" tokens
```console
$ cleost push action trpm111token transfer '["trpmuser1112", "trpm111stake", "5000.0000 TRPM", "phase A"]' -p trpmuser1112@active
executed transaction: d36cf18919c36600711e7ef938e09e291ef14d03b5cebc8a7ce344ff8867b54e  136 bytes  232 us
#  trpm111token <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm111stake","quantity":"5000.0000 TRPM","memo":"phase A"}
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm111stake","quantity":"5000.0000 TRPM","memo":"phase A"}
#  trpm111stake <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm111stake","quantity":"5000.0000 TRPM","memo":"phase A"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table
```console
$ cleost get table trpm111stake trpmuser1112 stakewallet --show-payer
{
  "rows": [{
      "data": {
        "amount": "5000.0000 TRPM",
        "lock_timestamp": 0,
        "start_timestamp": 0,
        "end_timestamp": 0,
        "doctor": "",
        "patient_status": "unassigned"
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `startrehab`
* `trpmadmin111` sets lock period of 30 days for the patient `trpmuser1112`
```console
$ cleost push action trpm111stake startrehab '["trpmadmin111", "trpmuser1112", "1601944920"]' -p trpmadmin111@active
executed transaction: f904007f3d39cf09efb40997c91ba71397aec14bb9d14a91ba9b49af2aa45e7f  112 bytes  230 us
#  trpm111stake <= trpm111stake::startrehab     {"doctor":"trpmadmin111","patient":"trpmuser1112","lock_timestamp":1601944920}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table
```console
$ cleost get table trpm111stake trpmuser1112 stakewallet --show-payer
{
  "rows": [{
      "data": {
        "staked_qty": "5000.0000 TRPM",
        "lock_timestamp": 1601944920,
        "start_timestamp": 1599353205,
        "end_timestamp": 0,
        "doctor": "trpmadmin111",
        "patient_status": "assigned"
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `endrehab`
* `trpmadmin111` caught & ends rehab.
```console
$ cleost push action trpm111stake endrehab '["trpmadmin111", "trpmuser1112", "caught"]' -p trpmadmin111@active
executed transaction: 945008a0850a9077bfcb9f36a2c87d7f4c419ed48afad37388cbc7e4b7c97af9  120 bytes  192 us
#  trpm111stake <= trpm111stake::endrehab       {"doctor":"trpmadmin111","patient":"trpmuser1112","patient_status":"caught"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table
```console
$ cleost get table trpm111stake trpmuser1112 stakewallet --show-payer
{
  "rows": [{
      "data": {
        "staked_qty": "5000.0000 TRPM",
        "lock_timestamp": 1601944920,
        "start_timestamp": 1599353205,
        "end_timestamp": 1599353381,
        "doctor": "trpmadmin111",
        "patient_status": "caught"
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```


### Action - `unstake`
* patient wants to unstake after rehab is ended with "caught" status
```console
$ cleost push action trpm111stake unstake '["trpmuser1112"]' -p trpmuser1112@active
executed transaction: c2d901ff053f0ee8536908d561ef0094677573732002f502ad49157d2fc85e06  104 bytes  199 us
#  trpm111stake <= trpm111stake::unstake        {"patient":"trpmuser1112"}
#  trpm111token <= trpm111token::transfer       {"from":"trpm111stake","to":"trpmadmin111","quantity":"5000.0000 TRPM","memo":"TROPIUM Stake contrac...
#  trpm111stake <= trpm111token::transfer       {"from":"trpm111stake","to":"trpmadmin111","quantity":"5000.0000 TRPM","memo":"TROPIUM Stake contrac...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmadmin111 <= trpm111token::transfer       {"from":"trpm111stake","to":"trpmadmin111","quantity":"5000.0000 TRPM","memo":"TROPIUM Stake contrac...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- Here, the patient loses all its staked tokens to Doctor.

## TODO
* [ ] the auth table data ownership is transferred from new doctor to contract itself. as the verification is done.
  - transfer back to the new doctor by an ACTION owndata(), only then the doctor is added to the admins list.