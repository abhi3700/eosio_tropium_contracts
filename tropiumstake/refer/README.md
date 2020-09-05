# tropium -- Stake contract and ICO contract

## Stake Contract

this is the eosio smart contract that will be in charge of handling the staking.
It contains four methods:
#### Staker methods:
* __regstaker__ : This method is in charge of registering staker
* __stake__: Once a user has been registered as a staker, he can transfer an amount to the account holding
this contract. This will trigger the "on_notify" attached to stake method that will handle the staking 
* __unstake__: cancel the staked amount, return the funds

#### admin methods:
* __banstaker__: To ban a user and prevent him from staking in the future. If banned, the staked amount is not
reimbursed
* __letinstaker__: To reinstate a banned staker. After being reinstated, the user can re-register and participate 
in the staking operation

#### contract method:
* __regadmin__: register a user as an admin. Only executable by the owner of this smart contract

## ICO contract

## Notice
*Before deployment, change all TODO accordingly*
