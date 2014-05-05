
package require -exact qsys 13.1



set_module_property DESCRIPTION ""
set_module_property NAME mysram
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME mysram
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ANALYZE_HDL AUTO
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false


add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL mysram
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file mysram.v VERILOG PATH mysram.v TOP_LEVEL_FILE





add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true
set_interface_property clock EXPORT_OF ""
set_interface_property clock PORT_NAME_MAP ""
set_interface_property clock SVD_ADDRESS_GROUP ""

add_interface_port clock clk clk Input 1


add_interface reset reset end
set_interface_property reset associatedClock clock
set_interface_property reset synchronousEdges DEASSERT
set_interface_property reset ENABLED true
set_interface_property reset EXPORT_OF ""
set_interface_property reset PORT_NAME_MAP ""
set_interface_property reset SVD_ADDRESS_GROUP ""

add_interface_port reset reset_n reset_n Input 1



add_interface s avalon end
set_interface_property s addressUnits WORDS
set_interface_property s associatedClock clock
set_interface_property s associatedReset ""
set_interface_property s bitsPerSymbol 8
set_interface_property s burstOnBurstBoundariesOnly false
set_interface_property s burstcountUnits WORDS
set_interface_property s explicitAddressSpan 0
set_interface_property s holdTime 0
set_interface_property s linewrapBursts false
set_interface_property s maximumPendingReadTransactions 0
set_interface_property s readLatency 0
set_interface_property s readWaitTime 1
set_interface_property s setupTime 0
set_interface_property s timingUnits Cycles
set_interface_property s writeWaitTime 0
set_interface_property s ENABLED true
set_interface_property s EXPORT_OF ""
set_interface_property s PORT_NAME_MAP ""
set_interface_property s SVD_ADDRESS_GROUP ""

add_interface_port s s_chipselect_n chipselect_n Input 1
add_interface_port s s_byteenable_n byteenable_n Input 16
add_interface_port s s_write_n write_n Input 1
add_interface_port s s_read_n read_n Input 1
add_interface_port s s_address address Input 16
add_interface_port s s_writedata writedata Input 16
add_interface_port s s_readdata readdata Output 1
set_interface_assignment s embeddedsw.configuration.isFlash 0
set_interface_assignment s embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment s embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment s embeddedsw.configuration.isPrintableDevice 0



add_interface avalon_slave_0 avalon end
set_interface_property avalon_slave_0 addressUnits WORDS
set_interface_property avalon_slave_0 associatedClock clock
set_interface_property avalon_slave_0 associatedReset ""
set_interface_property avalon_slave_0 bitsPerSymbol 8
set_interface_property avalon_slave_0 burstOnBurstBoundariesOnly false
set_interface_property avalon_slave_0 burstcountUnits WORDS
set_interface_property avalon_slave_0 explicitAddressSpan 0
set_interface_property avalon_slave_0 holdTime 0
set_interface_property avalon_slave_0 linewrapBursts false
set_interface_property avalon_slave_0 maximumPendingReadTransactions 0
set_interface_property avalon_slave_0 readLatency 0
set_interface_property avalon_slave_0 readWaitTime 1
set_interface_property avalon_slave_0 setupTime 0
set_interface_property avalon_slave_0 timingUnits Cycles
set_interface_property avalon_slave_0 writeWaitTime 0
set_interface_property avalon_slave_0 ENABLED true
set_interface_property avalon_slave_0 EXPORT_OF ""
set_interface_property avalon_slave_0 PORT_NAME_MAP ""
set_interface_property avalon_slave_0 SVD_ADDRESS_GROUP ""
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isFlash 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isPrintableDevice 0



add_interface conduit_end conduit end
set_interface_property conduit_end associatedClock clock
set_interface_property conduit_end associatedReset ""
set_interface_property conduit_end ENABLED true
set_interface_property conduit_end EXPORT_OF ""
set_interface_property conduit_end PORT_NAME_MAP ""
set_interface_property conduit_end SVD_ADDRESS_GROUP ""

add_interface_port conduit_end SRAM_DQ export Bidir 1
add_interface_port conduit_end SRAM_ADDR export Output 1
add_interface_port conduit_end SRAM_UB_n export Output 1
add_interface_port conduit_end SRAM_LB_n export Output 1
add_interface_port conduit_end SRAM_WE_n export Output 1
add_interface_port conduit_end SRAM_CE_n export Output 1
add_interface_port conduit_end SRAM_OE_n export Output 1



add_interface conduit_end_1 conduit end
set_interface_property conduit_end_1 associatedClock clock
set_interface_property conduit_end_1 associatedReset ""
set_interface_property conduit_end_1 ENABLED true
set_interface_property conduit_end_1 EXPORT_OF ""
set_interface_property conduit_end_1 PORT_NAME_MAP ""
set_interface_property conduit_end_1 SVD_ADDRESS_GROUP ""

