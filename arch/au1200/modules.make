#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#   设置与平台相关的bsp 环境.
#
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


CMN_INCLUDES=$(__AMBO_OS_HOME)/include $(__AMBO_OS_HOME)/mipsel-linux/mipsel-linux/include $(__AMBO_OS_HOME)/linux/include/linux $(__AMBO_OS_HOME)/arch/au1200/include $(__AMBO_OS_HOME)/drivers/include $(__AMBO_OS_HOME)/kingmos_private/include $(__AMBO_OS_HOME)/mipsel-linux/lib/gcc-lib/mipsel-linux/3.3.5/include $(__AMBO_OS_HOME)/mipsel-linux/mipsel-linux/sys-include $(__AMBO_OS_HOME)/thirdpart_prj/freetype-2.1.10/include $(__AMBO_OS_HOME)/thirdpart_prj/Mesa-6.2.1_app/include


ifeq ($(PLAT_BOOTLOADER), 1)
CMN_ADEFINES += --defsym _BOOTLOADER=1
else
CMN_ADEFINES += --defsym _BOOTLOADER=0
endif


#CMN_ADEFINES += --defsym DEBUG_UART=3

CMN_CDEFINES += -DMIPS_CPU -DAU1200 -DKINGMOS -DLINUX_KERNEL -DUSE_PLATFORM_DEFINE -DCANON_DIGCMP

