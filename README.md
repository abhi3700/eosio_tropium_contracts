# TROPIUM
* A patient stakes 500 TRPM tokens to get enrolled into rehab.
* A verified doctor starts the rehab & sets the monitoring period as 30 days.
* The assigned doctor ends the rehab by stating __cured__/__caught__.
* The patient can then unstake the tokens in 2 cases:
	- before the 30 days lock period only if it is caught => loses 500 TRPM to the doctor
	- after the 30 days lock period, only if it is cured. => loses 95% of 500 TRPM to the doctor & gets back 5% of 500 TRPM
* Verification starter: `eosaidchains`

## Workflow
### Pre-requisite
* Create TRPM token with attributes:
```md
"max_supply": "15 B",
"total_supply": "5 B"
"issuer": "tropiumchain"
```
* Issue TRPM token to the issuer: `"tropiumchain"`
* Issuer transfer TRPM tokens to the __ICO contract__ for auto-disbursement TRPM tokens only after receiving EOS tokens from buyer.
* ICO rate has to be set by the `tropiumchain` respective phases - A, B, C.

### Real time
* Buyer send EOS tokens & receive TRPM tokens to/from contract respectively.

> NOTE: Anyone can be buyer, no authentication required from dApp side.


## Contracts
* Token contract - `tropiumtoken` | `trpm111token`
* ICO contract - `tropiumico` | `trpm11111ico`
* Stake contract - `tropiumstake` | `trpm111stake`