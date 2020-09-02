project(maker)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)


add_contract( maker maker maker.cpp )



target_include_directories( maker PUBLIC ${CMAKE_SOURCE_DIR}/../include  )

target_ricardian_directory( maker ${CMAKE_SOURCE_DIR}/../ricardian )