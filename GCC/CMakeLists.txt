cmake_minimum_required(VERSION 3.10)

message(STATUS "Project root is: ${CMAKE_CURRENT_SOURCE_DIR}")

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)#这个不重要，arm也能行
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 定义 STM32F407ZGTx 作为目标设备
add_definitions(-DSTM32F407ZGTx)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DSTM32F407xx)
add_definitions(-DUSE_FULL_LL_DRIVER)
add_definitions(-DTX_ENABLE_FPU_SUPPORT)
add_definitions(-DTX_ENABLE_STACK_CHECKING)
add_definitions(-DTX_INCLUDE_USER_DEFINE_FILE)
add_definitions(-DNR_SHELL_USING_EXPORT_CMD)


project(Ls_Monitor_Lower_SW C CXX ASM)

# 设置默认构建类型为 Debug (用于调试器显示源码)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 设置 CMake 输出目录
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)


# 设置编译器和工具链路径（确保 arm-none-eabi 工具链已安装并正确配置）
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SZ arm-none-eabi-size)


# STM32 系列 MCU 配置
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb ")
set(FPU_FLAGS "-mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/stm32f407.ld")
set(LIBS "-lc -lm -lnosys")

# 添加需要编译的所有源码
set(C_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/FatFs/src/option/cc936.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/FatFs/src/diskio.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/FatFs/src/ff.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/FatFs/src/ff_gen_drv.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/FatFs/src/drivers/sd_diskio_dma.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/FatFs/src/option/syscall.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sram.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fsmc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/LogicalApp/gui_task/gui_task.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/LogicalApp/msg_task/msg_task.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/LogicalApp/statistic_task/statistic_task.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main/system_task_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/fonts/myFont20.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/fonts/myFont36.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_anim.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_anim_timeline.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/animimg/lv_animimg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_arc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_area.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_async.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_bar.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_bidi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_btn.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_btnmatrix.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/calendar/lv_calendar.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_canvas.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/chart/lv_chart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_checkbox.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_color.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/colorwheel/lv_colorwheel.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_disp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_arc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_img.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_label.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_layer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_line.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_mask.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_rect.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_arc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_blend.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_dither.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_gradient.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_img.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_layer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_letter.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_line.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_polygon.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_rect.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/sw/lv_draw_sw_transform.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_transform.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_draw_triangle.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_dropdown.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_event.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/lv_extra.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/layouts/flex/lv_flex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_dejavu_16_persian_hebrew.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_fmt_txt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_loader.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_montserrat_10.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_montserrat_12.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_montserrat_12_subpx.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_montserrat_14.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_montserrat_16.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_montserrat_24.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_montserrat_8.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_simsun_16_cjk.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_unscii_16.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/font/lv_font_unscii_8.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/fragment/lv_fragment.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/fragment/lv_fragment_manager.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_fs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_gc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/layouts/grid/lv_grid.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/gridnav/lv_gridnav.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_group.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/hal/lv_hal_disp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/hal/lv_hal_indev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/hal/lv_hal_tick.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/ime/lv_ime_pinyin.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_img.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_img_buf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_img_cache.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/draw/lv_img_decoder.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/imgbtn/lv_imgbtn.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/imgfont/lv_imgfont.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_indev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_indev_scroll.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/keyboard/lv_keyboard.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_label.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/led/lv_led.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_line.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/list/lv_list.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_ll.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_log.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_lru.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_math.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_mem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/menu/lv_menu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/meter/lv_meter.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/monkey/lv_monkey.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/msg/lv_msg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/msgbox/lv_msgbox.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj_class.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj_draw.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj_pos.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj_scroll.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj_style.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj_style_gen.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_obj_tree.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_objx_templ.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_printf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_refr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_roller.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_slider.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/others/snapshot/lv_snapshot.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/span/lv_span.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/spinbox/lv_spinbox.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/spinner/lv_spinner.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_style.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_style_gen.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_switch.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_table.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/tabview/lv_tabview.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_templ.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/widgets/lv_textarea.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/core/lv_theme.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/themes/basic/lv_theme_basic.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/themes/default/lv_theme_default.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/themes/mono/lv_theme_mono.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/tileview/lv_tileview.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_timer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_tlsf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_txt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_txt_ap.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc/lv_utils.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/widgets/win/lv_win.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/about_phone_ui.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/adjust_pedal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/battery_box.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/cool_control.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/motor_control_ui.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/sensor_ui.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/set_up.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui/vehicle_status.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/monitor.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/extra/libs/fsdrv/lv_fs_fatfs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/examples/porting/lv_port_disp_template.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/examples/porting/lv_port_indev_template.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main/stm32f4xx_hal_msp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main/stm32f4xx_it.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main/system_stm32f4xx.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Nr_Micro_Shell/src/ansi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Nr_Micro_Shell/src/ansi_port.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Nr_Micro_Shell/src/nr_micro_shell.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/can/bsp_can_bus.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/flash/bsp_cpu_flash.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/ticktim/bsp_dwt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/fonts/bsp_font.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/iic/bsp_iic_bus.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/ink_screen/bsp_inkscreen.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/key_fifo/bsp_key.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/lcd/bsp_lcd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/led/bsp_led.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/mlx90614/bsp_mlx90614.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/rotation/bsp_rotation.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/sd_card/bsp_sdio_sd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/spi/bsp_spi_bus.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/flash/bsp_spi_flash.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/flash/bsp_spi_flash_demo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/sram/bsp_sram.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/timer/bsp_tim_pwm.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/touch/bsp_touch.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/usart_fifo/bsp_usart_fifo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/ws2812b/bsp_ws2812b.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/Iwdg/iwdg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/sys/sys.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/ticktim/ticktim.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_allocate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_cleanup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_pool_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_block_release.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_allocate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_cleanup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_pool_search.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_byte_release.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_cleanup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_set.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_event_flags_set_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_initialize_high_level.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_initialize_kernel_enter.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_initialize_kernel_setup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_misra.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_cleanup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_priority_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_mutex_put.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_cleanup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_flush.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_front_send.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_receive.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_send.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_queue_send_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_ceiling_put.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_cleanup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_put.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_semaphore_put_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_entry_exit_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_identify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_preemption_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_priority_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_relinquish.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_reset.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_resume.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_shell_entry.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_sleep.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_stack_analyze.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_stack_error_handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_stack_error_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_suspend.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_system_preempt_check.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_system_resume.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_system_suspend.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_terminate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_time_slice.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_time_slice_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_timeout.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_thread_wait_abort.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_time_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_time_set.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_activate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_deactivate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_expiration_process.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_performance_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_performance_system_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_system_activate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_system_deactivate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_timer_thread_entry.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_buffer_full_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_disable.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_enable.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_event_filter.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_event_unfilter.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_initialize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_interrupt_control.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_isr_enter_insert.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_isr_exit_insert.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_object_register.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_object_unregister.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/tx_trace_user_event_insert.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_block_allocate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_block_pool_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_block_pool_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_block_pool_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_block_pool_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_block_release.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_byte_allocate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_byte_pool_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_byte_pool_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_byte_pool_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_byte_pool_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_byte_release.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_event_flags_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_event_flags_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_event_flags_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_event_flags_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_event_flags_set.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_event_flags_set_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_mutex_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_mutex_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_mutex_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_mutex_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_mutex_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_mutex_put.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_flush.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_front_send.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_receive.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_send.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_queue_send_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_ceiling_put.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_prioritize.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_put.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_semaphore_put_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_entry_exit_notify.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_info_get.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_preemption_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_priority_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_relinquish.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_reset.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_resume.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_suspend.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_terminate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_time_slice_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_thread_wait_abort.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_timer_activate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_timer_change.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_timer_create.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_timer_deactivate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_timer_delete.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/ThreadX/common/src/txe_timer_info_get.c"
    
    # 如有更多文件，请在此处添加
)

#启动文件
set(ASM_SOURCES
    "${CMAKE_SOURCE_DIR}/startup_stm32f407xx.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src/tx_thread_context_restore.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src/tx_thread_interrupt_control.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src/tx_thread_context_save.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src/tx_thread_schedule.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src/tx_thread_stack_build.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src/tx_thread_system_return.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src/tx_timer_interrupt.S"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/example_build/tx_initialize_low_level.S"
)

# 指定头文件目录
include_directories(
    "${CMAKE_SOURCE_DIR}/../Core"
    "${CMAKE_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src"
    "${CMAKE_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Src/Legacy"
    "${CMAKE_SOURCE_DIR}/../Hal_Libraries/CMSIS/Include"
    "${CMAKE_SOURCE_DIR}/../Hal_Libraries/CMSIS/Device/ST/STM32F4xx/Include"
    "${CMAKE_SOURCE_DIR}/../Hal_Libraries/STM32F4xx_HAL_Driver/Inc"
    "${CMAKE_SOURCE_DIR}/../System/ads1256"
    "${CMAKE_SOURCE_DIR}/../System/can"
    "${CMAKE_SOURCE_DIR}/../System/flash"
    "${CMAKE_SOURCE_DIR}/../System/fonts"
    "${CMAKE_SOURCE_DIR}/../System/iic"
    "${CMAKE_SOURCE_DIR}/../System/ink_screen"
    "${CMAKE_SOURCE_DIR}/../System/Iwdg"
    "${CMAKE_SOURCE_DIR}/../System/key_fifo"
    "${CMAKE_SOURCE_DIR}/../System/lcd"
    "${CMAKE_SOURCE_DIR}/../System/led"
    "${CMAKE_SOURCE_DIR}/../System/mlx90614"
    "${CMAKE_SOURCE_DIR}/../System/rotation"
    "${CMAKE_SOURCE_DIR}/../System/sd_card"
    "${CMAKE_SOURCE_DIR}/../System/spi"
    "${CMAKE_SOURCE_DIR}/../System/sram"
    "${CMAKE_SOURCE_DIR}/../System/sys"
    "${CMAKE_SOURCE_DIR}/../System/ticktim"
    "${CMAKE_SOURCE_DIR}/../System/timer"
    "${CMAKE_SOURCE_DIR}/../System/touch"
    "${CMAKE_SOURCE_DIR}/../System/usart_fifo"
    "${CMAKE_SOURCE_DIR}/../System/ws2812b"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/example_build"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/common/inc"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/examples/porting"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP/widght_ui"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI_APP"
    "${CMAKE_SOURCE_DIR}/../FatFs/src"
    "${CMAKE_SOURCE_DIR}/../FatFs/src/drivers"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/Lvgl/GUI/lvgl/src/misc"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/src"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/ThreadX/ports/cortex_m4/gnu/inc"
    "${CMAKE_SOURCE_DIR}/../stm32f4_main"
    "${CMAKE_SOURCE_DIR}/../System/canfestival/inc"
    "${CMAKE_SOURCE_DIR}/../System/canfestival/dictionary"
    "${CMAKE_SOURCE_DIR}/../Main"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/LogicalApp/gui_task"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/LogicalApp/msg_task"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/LogicalApp/statistic_task"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/Nr_Micro_Shell/inc"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/FatFs"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/FatFs/src"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/FatFs/src/option"
    "${CMAKE_SOURCE_DIR}/../Third_Libraries/FatFs/src/drivers"
)

# 设置编译标志
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	# Debug 模式：包含完整调试信息 使用 -Og 优化以保持可调试性
	set(CMAKE_C_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -Wall -fdata-sections -ffunction-sections -Og -g3 -gdwarf-2")
	set(CMAKE_CXX_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -Wall -fdata-sections -ffunction-sections -Og -g3 -gdwarf-2")
	set(CMAKE_ASM_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -x assembler-with-cpp -Wall -fdata-sections -g3 -gdwarf-2")
else()
	# Release 模式：使用 -Os 优化代码大小 类似 IAR 的平衡优化
	set(CMAKE_C_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -Wall -fdata-sections -ffunction-sections -Os -g")
	set(CMAKE_CXX_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -Wall -fdata-sections -ffunction-sections -Os -g")
	set(CMAKE_ASM_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -x assembler-with-cpp -Wall -fdata-sections -ffunction-sections")
endif()

# 创建目标文件 - 对象文件
add_executable(${PROJECT_NAME}.elf
		${C_SOURCES}
		${ASM_SOURCES}
)

# Ensure changes to the linker script trigger a relink.
set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

# NR Micro Shell uses 'char' for argv/offset storage. GCC defaults to signed char
# on many targets, which can cause negative indices when values exceed 127.
# Apply unsigned-char behavior only to the shell sources to match common IDE defaults.
set_source_files_properties(
	"${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Nr_Micro_Shell/src/ansi.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Nr_Micro_Shell/src/ansi_port.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Third_Libraries/Nr_Micro_Shell/src/nr_micro_shell.c"
	PROPERTIES COMPILE_OPTIONS "-funsigned-char"
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
	-mcpu=cortex-m4
	-mthumb
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard
	-T${LINKER_SCRIPT}
	--specs=nosys.specs
	--specs=nano.specs
	-Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map,--cref
	-Wl,--gc-sections
	-Wl,--start-group -lc -lm -lnosys -Wl,--end-group
)

# 添加生成 .hex 和 .bin 文件的自定义命令
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
)

# 设置清理命令
add_custom_target(clean-all
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/*
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/*.elf
    COMMENT "Cleaning build files..."
)

