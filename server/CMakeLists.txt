# This is the main binary. List your C files here.
add_executable(lab11_server
  picow_bt_example_freertos.c
  gatt_counter.c
  temp_sense.c
)

target_compile_definitions(lab11_server PRIVATE
  CYW43_TASK_STACK_SIZE=1024
  NO_SYS=0
)

# Add the libraries to the build
target_link_libraries(lab11_server PRIVATE
  pico_stdlib
  pico_async_context_freertos
  pico_cyw43_arch_lwip_sys_freertos
  FreeRTOS-Kernel-Heap4
  pico_btstack_cyw43
  pico_btstack_ble
  hardware_adc
)

pico_btstack_make_gatt_header(lab11_server PUBLIC ${CMAKE_SOURCE_DIR}/include/gatt_counter.gatt)

pico_set_program_name(lab11_server "server")
pico_set_program_version(lab11_server "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(lab11_server 0)
pico_enable_stdio_usb(lab11_server 1)

pico_add_extra_outputs(lab11_server)


add_custom_target(flash_server picotool load -f $<TARGET_FILE:lab11_server> DEPENDS lab11_server)
