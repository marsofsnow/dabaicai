#pragma once

#define USING_ACTION(contract_name,action_name) using action_name##_action = action_wrapper<#action_name##_n, &contract_name::action_name>
