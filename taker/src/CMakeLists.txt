project(taker)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( taker taker taker.cpp )
target_include_directories( taker PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( taker ${CMAKE_SOURCE_DIR}/../ricardian )