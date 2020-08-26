project(learn)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( learn learn learn.cpp )
target_include_directories( learn PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( learn ${CMAKE_SOURCE_DIR}/../ricardian )