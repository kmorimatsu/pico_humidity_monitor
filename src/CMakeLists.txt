cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(Pico_ePaper_Code)
pico_sdk_init()

# add a compilation subdirectory
add_subdirectory(lib/Config)
add_subdirectory(lib/e-Paper)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)

# add a header directory
include_directories(./lib/Config)
include_directories(./lib/GUI)

# generate an executable file
add_executable(humidity 
	main.c
	EPD_2in13_V4.c
	flash_counter.c
	io.c
)

# enable usb output, disable uart output
#pico_enable_stdio_usb(humidity 1)
pico_enable_stdio_usb(humidity 0)
pico_enable_stdio_uart(humidity 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(humidity)

target_link_libraries(humidity 
	ePaper 
	GUI 
	Fonts 
	Config 
	pico_stdlib 
	hardware_spi
	hardware_flash
	hardware_adc
	hardware_i2c
)
