# Stake Contract
* A doctor registers by giving NPI no. as a profile_url in action - `regbydoctor`
* `eosaidchains` self adds into admins list by action - `compaddadmin`
* `eosaidchains` then as a admin verifies new doctors
* Patient stakes "500 TRPM" tokens to get rehab from doctor.
* The assigned doctor `startrehab` & set the lock period of 30 days.
* The Dr. can `endrehab` by setting the status as "cured" or "caught".
* If cured, then Dr. gets 5% & remaining is returned back to the patient.
* Else if caught, then Dr. gets 100% tokens & the patient loses entirely.
* A doctor can be removed from the admins list when the no. is 5 

## About
* contract name - `tropiumstake`
* contract's account name - `trpm111stake`
* action
  - `regbydoctor`
  - `verify`
  - `blacklist`
  - `compaddadmin`
  - `addadmin`  [Inline action]
  - `remadmin`  [Inline action]
  - `stake` [notify action]
  - `startrehab`
  - `endrehab`
  - `unstake`
  - `sendalert` [Inline action]
* table
  - `stakewallet`
  - `admins`
  - `auth`



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
### Action - `regbydoctor`
* A doctor `trpmdoc11111` registers
```console
$ cleost push action trpm111stake regbydoctor '["trpmdoc11111", "http://www.hipaaspace.com/medical_billing/coding/npi/codes/npi_1891894531.aspx"]' -p trpmdoc11111@active
executed transaction: 64d0e1fca08b9048f7515c22fb5b619fb5976e2c21c4e79e89e7756cb731b10d  184 bytes  340 us
#  trpm111stake <= trpm111stake::regbydoctor    {"doctor":"trpmdoc11111","profile_url":"http://www.hipaaspace.com/medical_billing/coding/npi/codes/n...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table
```console
$ cleost get table trpm111stake trpm111stake auth --show-payer --lower trpmdoc11111 --limit 1
{
  "rows": [{
      "data": {
        "doctor": "trpmdoc11111",
        "profile_url": "http://www.hipaaspace.com/medical_billing/coding/npi/codes/npi_1891894531.aspx",
        "user_status": "added",
        "add_timestamp": 1601401696,
        "verify_timestamp": 0,
        "blist_timestamp": 0,
        "validator_verify": "",
        "validators_blacklist": []
      },
      "payer": "trpmdoc11111"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* A doctor `trpmdoc11111` registers again & gets error
```console
$ cleost push action trpm111stake regbydoctor '["trpmdoc11111", "http://www.hipaaspace.com/medical_billing/coding/npi/codes/npi_1891894531.aspx"]' -p trpmdoc11111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the doctor's info is already added.
pending console output:
```
* Likewise, `trpmdoc11112`, `trpmdoc11113`, `trpmdoc11114`, `trpmdoc11115`, `trpmdoc11121` doctors register
  - for doctor `trpmdoc11112`
```console
$ cleost push action trpm111stake regbydoctor '["trpmdoc11112", "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx"]' -p trpmdoc11112@active
executed transaction: 81481fc1473c14a58c78bd7058008d1aa2f7e7951e7cc5b5877475b0d1ef2a58  208 bytes  141 us
#  trpm111stake <= trpm111stake::regbydoctor    {"doctor":"trpmdoc11112","profile_url":"https://www.hipaaspace.com/medical_billing/coding/national_p...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - for doctor `trpmdoc11113`
```console
$ cleost push action trpm111stake regbydoctor '["trpmdoc11113", "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx"]' -p trpmdoc11113@active
executed transaction: f7c1042c15e56013946bce177b7203b05d3ed3497f332846b07e45a427e1f04b  208 bytes  207 us
#  trpm111stake <= trpm111stake::regbydoctor    {"doctor":"trpmdoc11113","profile_url":"https://www.hipaaspace.com/medical_billing/coding/national_p...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - for doctor `trpmdoc11114`
```console
$ cleost push action trpm111stake regbydoctor '["trpmdoc11114", "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx"]' -p trpmdoc11114@active
executed transaction: 7a6249cb37c5e333ebfbd822130d7cc1b90865542cd09f8cd8b1008f87b84889  208 bytes  224 us
#  trpm111stake <= trpm111stake::regbydoctor    {"doctor":"trpmdoc11114","profile_url":"https://www.hipaaspace.com/medical_billing/coding/national_p...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - for doctor `trpmdoc11115`
```console
$ cleost push action trpm111stake regbydoctor '["trpmdoc11115", "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx"]' -p trpmdoc11115@active
executed transaction: d9239c166b3eacc24dd7dc93c113cef7c65705edad1d9a9f900d978cbedeaef0  208 bytes  158 us
#  trpm111stake <= trpm111stake::regbydoctor    {"doctor":"trpmdoc11115","profile_url":"https://www.hipaaspace.com/medical_billing/coding/national_p...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - for doctor `trpmdoc11121`
```console
$ cleost push action trpm111stake regbydoctor '["trpmdoc11121", "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx"]' -p trpmdoc11121@active
executed transaction: 534722a2cc2dd3f80cf077e7bb7e9deab3738ee4e57a85ab1a9538e2c4c1dddb  208 bytes  200 us
#  trpm111stake <= trpm111stake::regbydoctor    {"doctor":"trpmdoc11121","profile_url":"https://www.hipaaspace.com/medical_billing/coding/national_p...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

### Action - `compaddadmin`
* founder `eosaidchains` add itself as admin to the list
```console
$ cleost push action trpm111stake compaddadmin '[]' -p eosaidchains@active
executed transaction: acdcccec97547ccb7bf877fe419d8e233dffd8cbf0893d18621ed94691dd6864  96 bytes  196 us
#  trpm111stake <= trpm111stake::compaddadmin   ""
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
          "eosaidchains"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* founder `eosaidchains` add itself as admin to the list again & gets error
```console
$ cleost push action trpm111stake compaddadmin '[]' -p eosaidchains@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: admins list is already initialized by founder: 'eosaidchains'. So, can't be redone.
pending console output:
```

### Action - `verify`
* founder `eosaidchains` verifies the doctor `trpmdoc11111`
```console
$ cleost push action trpm111stake verify '["eosaidchains", "trpmdoc11111"]' -p eosaidchains@active
executed transaction: 9e7f02eabd0648c73eb6321fc4a0ca63694edfb3c7aa96fd4c22257c7ee5c22f  112 bytes  223 us
#  trpm111stake <= trpm111stake::verify         {"verified_doctor":"eosaidchains","new_doctor":"trpmdoc11111"}
#  trpm111stake <= trpm111stake::addadmin       {"doctor":"trpmdoc11111"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table `auth`
```console
$ cleost get table trpm111stake trpm111stake auth --show-payer --lower trpmdoc11111 --limit 1
{
  "rows": [{
      "data": {
        "doctor": "trpmdoc11111",
        "profile_url": "http://www.hipaaspace.com/medical_billing/coding/npi/codes/npi_1891894531.aspx",
        "user_status": "verified",
        "add_timestamp": 1601401696,
        "verify_timestamp": 1601402013,
        "blist_timestamp": 0,
        "validator_verify": "eosaidchains",
        "validators_blacklist": []
      },
      "payer": "trpmdoc11111"
    }
  ],
  "more": true,
  "next_key": "14837993877386314272"
}
```
  - view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - Observations
    + doctor has been verified
    + doctor has been added into the admins list

* Now, other than `eosaidchains`, other admins like `trpmdoc11111` can verify new doctors - `trpmdoc11112`, `trpmdoc11113`
  - to verify `trpmdoc11112`
```console
$ cleost push action trpm111stake verify '["trpmdoc11111", "trpmdoc11112"]' -p trpmdoc11111@active
executed transaction: 150d88db2802b84f56350c342f79c48879cf4efa8fb29a59bba449a3214474f3  112 bytes  185 us
#  trpm111stake <= trpm111stake::verify         {"verified_doctor":"trpmdoc11111","new_doctor":"trpmdoc11112"}
#  trpm111stake <= trpm111stake::addadmin       {"doctor":"trpmdoc11112"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - to verify `trpmdoc11113`
```console
$ cleost push action trpm111stake verify '["trpmdoc11111", "trpmdoc11113"]' -p trpmdoc11111@active
executed transaction: c4b812e4288f102390ee9f08e28d0919ffbb5a72cf3dfadfafddbe1d250ee909  112 bytes  157 us
#  trpm111stake <= trpm111stake::verify         {"verified_doctor":"trpmdoc11111","new_doctor":"trpmdoc11113"}
#  trpm111stake <= trpm111stake::addadmin       {"doctor":"trpmdoc11113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* Now, new admins like `trpmdoc11112` can verify new doctors - `trpmdoc11114`, `trpmdoc11115`, `trpmdoc11121`
  - to verify `trpmdoc11114`
```console
$ cleost push action trpm111stake verify '["trpmdoc11112", "trpmdoc11114"]' -p trpmdoc11112@active
executed transaction: 7dd121d56508798eaa8613454159e1782cf2b1f2f3b699c987f6d9aed1e0003f  112 bytes  183 us
#  trpm111stake <= trpm111stake::verify         {"verified_doctor":"trpmdoc11112","new_doctor":"trpmdoc11114"}
#  trpm111stake <= trpm111stake::addadmin       {"doctor":"trpmdoc11114"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113",
          "trpmdoc11114"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - to verify `trpmdoc11115`
```console
$ cleost push action trpm111stake verify '["trpmdoc11112", "trpmdoc11115"]' -p trpmdoc11112@active
executed transaction: 79f75979a1774ce0070aa774b8b5e9776d89ef264e86af2ac156a4f16e3da882  112 bytes  194 us
#  trpm111stake <= trpm111stake::verify         {"verified_doctor":"trpmdoc11112","new_doctor":"trpmdoc11115"}
#  trpm111stake <= trpm111stake::addadmin       {"doctor":"trpmdoc11115"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113",
          "trpmdoc11114",
          "trpmdoc11115"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - to verify `trpmdoc11121`
```console
$ cleost push action trpm111stake verify '["trpmdoc11112", "trpmdoc11121"]' -p trpmdoc11112@active
executed transaction: f8e12c53e8eaebf88e43bb289f1175cf34849391bb0f742ecf9e11abdcf65577  112 bytes  214 us
#  trpm111stake <= trpm111stake::verify         {"verified_doctor":"trpmdoc11112","new_doctor":"trpmdoc11121"}
#  trpm111stake <= trpm111stake::addadmin       {"doctor":"trpmdoc11121"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113",
          "trpmdoc11114",
          "trpmdoc11115",
          "trpmdoc11121"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```

### Action - `blacklist`
* Now, let's blacklist a doctor `trpmdoc11113` from the admins list. This requires 5 doctors to blacklist the doctor.
  1. blacklist by `eosaidchains`
```console
$ cleost push action trpm111stake blacklist '["eosaidchains", "trpmdoc11113"]' -p eosaidchains@active
executed transaction: 86a91bda549ed9ba6df723d9b9fdbf604f6e580415aae634f4d12880214879eb  112 bytes  336 us
#  trpm111stake <= trpm111stake::blacklist      {"verified_doctor":"eosaidchains","doctor":"trpmdoc11113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113",
          "trpmdoc11114",
          "trpmdoc11115",
          "trpmdoc11121"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
    + view the table `auth`
```console
$ cleost get table trpm111stake trpm111stake auth --show-payer --lower trpmdoc11113 --limit 1
{
  "rows": [{
      "data": {
        "doctor": "trpmdoc11113",
        "profile_url": "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx",
        "user_status": "blacklisted",
        "add_timestamp": 1601401802,
        "verify_timestamp": 1601402117,
        "blist_timestamp": 1601402388,
        "validator_verify": "trpmdoc11111",
        "validators_blacklist": [
          "eosaidchains"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": true,
  "next_key": "14837993877386314304"
}
```

  2. blacklist by `trpmdoc11111`
```console
$ cleost push action trpm111stake blacklist '["trpmdoc11111", "trpmdoc11113"]' -p trpmdoc11111@active
executed transaction: 3c531e9905227bdc69f265d9dc4301fac4bf5d678dbb88ef7500f9b94598cb92  112 bytes  175 us
#  trpm111stake <= trpm111stake::blacklist      {"verified_doctor":"trpmdoc11111","doctor":"trpmdoc11113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113",
          "trpmdoc11114",
          "trpmdoc11115",
          "trpmdoc11121"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
    + view the table `auth`
```console
$ cleost get table trpm111stake trpm111stake auth --show-payer --lower trpmdoc11113 --limit 1
{
  "rows": [{
      "data": {
        "doctor": "trpmdoc11113",
        "profile_url": "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx",
        "user_status": "blacklisted",
        "add_timestamp": 1601401802,
        "verify_timestamp": 1601402117,
        "blist_timestamp": 1601402473,
        "validator_verify": "trpmdoc11111",
        "validators_blacklist": [
          "eosaidchains",
          "trpmdoc11111"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": true,
  "next_key": "14837993877386314304"
}
```
  3. blacklist by `trpmdoc11112`
```console
$ cleost push action trpm111stake blacklist '["trpmdoc11112", "trpmdoc11113"]' -p trpmdoc11112@active
executed transaction: f4c554cb8e37a75f69097a71f13295eba5e39725ad96f8490900935de65a88c0  112 bytes  158 us
#  trpm111stake <= trpm111stake::blacklist      {"verified_doctor":"trpmdoc11112","doctor":"trpmdoc11113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113",
          "trpmdoc11114",
          "trpmdoc11115",
          "trpmdoc11121"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
    + view the table `auth`
```console
$ cleost get table trpm111stake trpm111stake auth --show-payer --lower trpmdoc11113 --limit 1
{
  "rows": [{
      "data": {
        "doctor": "trpmdoc11113",
        "profile_url": "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx",
        "user_status": "blacklisted",
        "add_timestamp": 1601401802,
        "verify_timestamp": 1601402117,
        "blist_timestamp": 1601402536,
        "validator_verify": "trpmdoc11111",
        "validators_blacklist": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": true,
  "next_key": "14837993877386314304"
}
```
  4. blacklist by `trpmdoc11114`
```console
$ cleost push action trpm111stake blacklist '["trpmdoc11114", "trpmdoc11113"]' -p trpmdoc11114@active
executed transaction: daea8061d6cc123b7c293de8a7b871e1f7031d89953c01b459eaa3b00422e46e  112 bytes  137 us
#  trpm111stake <= trpm111stake::blacklist      {"verified_doctor":"trpmdoc11114","doctor":"trpmdoc11113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11113",
          "trpmdoc11114",
          "trpmdoc11115",
          "trpmdoc11121"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
    + view the table `auth`
```console
$ cleost get table trpm111stake trpm111stake auth --show-payer --lower trpmdoc11113 --limit 1
{
  "rows": [{
      "data": {
        "doctor": "trpmdoc11113",
        "profile_url": "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx",
        "user_status": "blacklisted",
        "add_timestamp": 1601401802,
        "verify_timestamp": 1601402117,
        "blist_timestamp": 1601402583,
        "validator_verify": "trpmdoc11111",
        "validators_blacklist": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11114"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": true,
  "next_key": "14837993877386314304"
}
```
  5. blacklist by `trpmdoc11115`
```console
$ cleost push action trpm111stake blacklist '["trpmdoc11115", "trpmdoc11113"]' -p trpmdoc11115@active
executed transaction: 0d88867886c9e0a3adcf3bb00695b2f62d5688c2f40ba991eb50c7cae5c5585e  112 bytes  186 us
#  trpm111stake <= trpm111stake::blacklist      {"verified_doctor":"trpmdoc11115","doctor":"trpmdoc11113"}
#  trpm111stake <= trpm111stake::remadmin       {"doctor":"trpmdoc11113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
    + view the table `admins`
```console
$ cleost get table trpm111stake trpm111stake admins --show-payer
{
  "rows": [{
      "data": {
        "type": "doctor",
        "vector_admin": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11114",
          "trpmdoc11115",
          "trpmdoc11121"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
    + view the table `auth`
```console
$ cleost get table trpm111stake trpm111stake auth --show-payer --lower trpmdoc11113 --limit 1
{
  "rows": [{
      "data": {
        "doctor": "trpmdoc11113",
        "profile_url": "https://www.hipaaspace.com/medical_billing/coding/national_provider_identifier/codes/npi_1467586115.aspx",
        "user_status": "blacklisted",
        "add_timestamp": 1601401802,
        "verify_timestamp": 1601402117,
        "blist_timestamp": 1601402617,
        "validator_verify": "trpmdoc11111",
        "validators_blacklist": [
          "eosaidchains",
          "trpmdoc11111",
          "trpmdoc11112",
          "trpmdoc11114",
          "trpmdoc11115"
        ]
      },
      "payer": "trpm111stake"
    }
  ],
  "more": true,
  "next_key": "14837993877386314304"
}
```
* Obeservations
  - After 5 other doctors (as admins) when votes for blacklisting the doctor `trpmdoc11113`, then the doctor is removed from the admin list

### Action - `stake` [Payable action]
* patient `trpmuser1112` transfers "500 TRPM" for stake before admitting into rehab
  - view the token accounts table
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
  - transfers token - 500 TRPM, otherwise not transferred
```console
$ cleost push action trpm111token transfer '["trpmuser1112", "trpm111stake", "500.0000 TRPM", "transfer 500 TRPM tokens for Rehab admission"]' -p trpmuser1112@active
executed transaction: a67dda52e57dbcff0b16407e5073d0643f4d56bd371f0dfb2da5ac6aa60eeede  176 bytes  325 us
#  trpm111token <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm111stake","quantity":"500.0000 TRPM","memo":"transfer 500 TRPM toke...
#  trpmuser1112 <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm111stake","quantity":"500.0000 TRPM","memo":"transfer 500 TRPM toke...
#  trpm111stake <= trpm111token::transfer       {"from":"trpmuser1112","to":"trpm111stake","quantity":"500.0000 TRPM","memo":"transfer 500 TRPM toke...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - view the table `stakewallet`
```console
$ cleost get table trpm111stake trpmuser1112 stakewallet --show-payer
{
  "rows": [{
      "data": {
        "staked_qty": "500.0000 TRPM",
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
* `trpmdoc11111` sets lock period of 30 days for the patient `trpmuser1112`
```console
$ cleost push action trpm111stake startrehab '["trpmdoc11111", "trpmuser1112", "1603315246"]' -p trpmdoc11111@active
executed transaction: baef75039a2ec4e248c3039ee3c25104c81bf348b4bbcabe70cae6d54aa8e34d  112 bytes  277 us
#  trpm111stake <= trpm111stake::startrehab     {"verified_doctor":"trpmdoc11111","patient":"trpmuser1112","lock_timestamp":1603315246}
#  trpm111stake <= trpm111stake::sendalert      {"user":"trpmuser1112","message":"Your rehab period is started by the doctor: 'trpmdoc11111'."}
#  trpmuser1112 <= trpm111stake::sendalert      {"user":"trpmuser1112","message":"Your rehab period is started by the doctor: 'trpmdoc11111'."}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
  - View the table `stakewallet`
```console
$ cleost get table trpm111stake trpmuser1112 stakewallet --show-payer
{
  "rows": [{
      "data": {
        "staked_qty": "500.0000 TRPM",
        "lock_timestamp": 1603315246,
        "start_timestamp": 1600723257,
        "end_timestamp": 0,
        "doctor": "trpmdoc11111",
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
* Some other doctor tries to end the rehab & gets error
```console
$ cleost push action trpm111stake endrehab '["trpmdoc11115", "trpmuser1112", "caught"]' -p trpmdoc11115@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: parsed Doctor's name does not match with assigned Dr. name.
pending console output:
```

#### Case-1: Caught
* `trpmdoc11111` caught & ends rehab.
```console
$ cleost push action trpm111stake endrehab '["trpmdoc11111", "trpmuser1112", "caught"]' -p trpmdoc11111@active
executed transaction: 818efd04b014c6b6039ea81473788873e50e30b9ce62846ad7ee77ecbf473c36  120 bytes  201 us
#  trpm111stake <= trpm111stake::endrehab       {"doctor":"trpmdoc11111","patient":"trpmuser1112","patient_status":"caught"}
#  trpm111stake <= trpm111stake::sendalert      {"user":"trpmuser1112","message":"Your rehab period is ended as the doctor: 'trpmdoc11111' mentioned...
#  trpmuser1112 <= trpm111stake::sendalert      {"user":"trpmuser1112","message":"Your rehab period is ended as the doctor: 'trpmdoc11111' mentioned...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- View the table `stakewallet`
```console
$ cleost get table trpm111stake trpmuser1112 stakewallet --show-payer
{
  "rows": [{
      "data": {
        "staked_qty": "500.0000 TRPM",
        "lock_timestamp": 1603315246,
        "start_timestamp": 1600723257,
        "end_timestamp": 1600723416,
        "doctor": "trpmdoc11111",
        "patient_status": "caught"
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```

#### Case-2: Cured



### Action - `unstake`
* patient wants to unstake after rehab is ended with "caught" status
```console
$ cleost push action trpm111stake unstake '["trpmuser1112"]' -p trpmuser1112@active
executed transaction: 0b74a3a54cb2422e70716b817118fa450ab557d7616290a6290d42d54304af4c  104 bytes  331 us
#  trpm111stake <= trpm111stake::unstake        {"patient":"trpmuser1112"}
#  trpm111token <= trpm111token::transfer       {"from":"trpm111stake","to":"trpmdoc11111","quantity":"500.0000 TRPM","memo":"TROPIUM Stake contract...
#  trpm111stake <= trpm111stake::sendalert      {"user":"trpmuser1112","message":"You lose your staked tokens: '500.0000 TRPM' for rehab"}
#  trpm111stake <= trpm111token::transfer       {"from":"trpm111stake","to":"trpmdoc11111","quantity":"500.0000 TRPM","memo":"TROPIUM Stake contract...
>> Either money is not sent to the contract or contract itself is the buyer.
#  trpmdoc11111 <= trpm111token::transfer       {"from":"trpm111stake","to":"trpmdoc11111","quantity":"500.0000 TRPM","memo":"TROPIUM Stake contract...
#  trpmuser1112 <= trpm111stake::sendalert      {"user":"trpmuser1112","message":"You lose your staked tokens: '500.0000 TRPM' for rehab"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- Here, the patient loses all its staked tokens to Doctor.
  - view the doctor's TRPM balance
```console
$ cleost get table trpm111token trpmdoc11111 accounts --show-payer
{
  "rows": [{
      "data": {
        "balance": "500.0000 TRPM"
      },
      "payer": "trpm111stake"
    }
  ],
  "more": false,
  "next_key": ""
}
```
  - View the table `stakewallet`
```console
$ cleost get table trpm111stake trpmuser1112 stakewallet --show-payer
{
  "rows": [],
  "more": false,
  "next_key": ""
}
```


## Extra
* del `admins` table
```console
$ cleost push action trpm111stake testdeladmin '[]' -p trpm111stake@active
executed transaction: eae1544da3adf8f7872ce35fb50b4b4a4f2d82ff9f7ef06200182dba558794f2  96 bytes  278 us
#  trpm111stake <= trpm111stake::testdeladmin   ""
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* del `auth` table
```console
$ cleost push action trpm111stake testdelauth '["trpmdoc11111"]' -p trpm111stake@active
executed transaction: d5b27ec97138fd4c9604d08132ee202e969999bcb1da037510ca230bd9b07d34  104 bytes  176 us
#  trpm111stake <= trpm111stake::testdelauth    {"doctor":"trpmdoc11111"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* del `stakewallet` table
```console
$ cleost push action trpm111stake testdelrehab '["trpmuser1112"]' -p trpm111stake@active
executed transaction: d5b27ec97138fd4c9604d08132ee202e969999bcb1da037510ca230bd9b07d34  104 bytes  176 us
#  trpm111stake <= trpm111stake::testdelauth    {"doctor":"trpmdoc11111"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## TODO
* [ ] the auth table data ownership is transferred from new doctor to contract itself. as the verification is done.
  - transfer back to the new doctor by an ACTION owndata(), only then the doctor is added to the admins list.