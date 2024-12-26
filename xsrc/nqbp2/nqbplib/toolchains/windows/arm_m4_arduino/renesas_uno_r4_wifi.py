#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   GCC ARM-R\M
#   Target:     Renesas RA4M1 \ Ardunio
#   Output:     .BIN file
#------------------------------------------------------------------------------

import sys, os
from nqbplib import base
from nqbplib import utils
from nqbplib import my_globals

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, arduino_root, env_bsp_ver, default_variant='arduino', env_error=None, override_freertos_config=False ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname   = 'GCC Arm-Cortex M4 (no eabi) Compiler'
        self._cc       = 'arm-none-eabi-gcc'
        self._asm      = 'arm-none-eabi-gcc'
        self._ld       = 'arm-none-eabi-gcc'
        self._ar       = 'arm-none-eabi-ar'
        self._objcpy   = 'arm-none-eabi-objcopy'
        self._printsz  = 'arm-none-eabi-size'

        self._asm_ext  = 'asm'    
        self._asm_ext2 = 'S'   

        self._shell      = r'cmd.exe /C'
        self._rm         = r'del /f /q'

        self._os_sep     = '/' # Force unix directory separator (for using a response file with gcc on Windoze Host)

        # Cache potential error for environment variables not set
        self._env_error = env_error;

        self._clean_pkg_dirs.extend( ['arduino', '_arduino'] )

        # set the name of the linker output (not the final output)
        self._link_output = '-o'

        # Define paths
        uno_src_path = os.path.join( arduino_root, 'arduino', 'hardware', 'renesas_uno', env_bsp_ver )
        self._base_release.inc = self._base_release.inc + \
                ' -iprefix'+uno_src_path + \
                r' -iwithprefix\cores\arduino\tinyusb' + \
                r' -iwithprefix\cores\arduino\api\deprecated' + \
                r' -iwithprefix\cores\arduino' + \
                r' -iwithprefix\variants\UNOWIFIR4' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\inc' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\inc\api' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\inc\instances' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\arm\CMSIS_5\CMSIS\Core\Include' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra_gen' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra_cfg\fsp_cfg\bsp' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra_cfg\fsp_cfg' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\src\r_usb_basic\src\driver\inc' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\src\r_sce\crypto_procedures\src\sce5\plainkey\private\inc' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\src\r_sce\crypto_procedures\src\sce5\plainkey\public\inc' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\src\r_sce\common' + \
                r' -iwithprefix\variants\UNOWIFIR4\includes\ra\fsp\src\r_sce'  

                # r' -iwithprefix\cores\arduino\api' + \
                # r' -iwithprefix\cores\arduino\USB' + \
                # r' -iwithprefix\variants\UNOWIFIR4\includes' + \

        #if ( not override_freertos_config ):
        #    self._base_release.inc = self._base_release.inc + ' -I' + freertos_src_path + r'\config'


        # 
        #common_flags                    = ' -Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 --specs=nano.specs --specs=nosys.specs -u _printf_float '
        common_flags                    = ' -g3 -Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16'
        asm_and_compile_flags           = ' -D_RA_CORE=CM4 -D_RENESAS_RA_ -DARDUINO_UNOWIFIR4 -DBACKTRACE_SUPPORT -DARDUINO_ARCH_RENESAS_UNO -DARDUINO_ARCH_RENESAS -DARDUINO_FSP -D_XOPEN_SOURCE=700 -DCFG_TUSB_MCU=OPT_MCU_RAXXX "-DARDUINO_BSP_VERSION=\\"' + env_bsp_ver + '\\""' 
        cpp_and_c_flags                 = ' -nostdlib -ffunction-sections -fdata-sections -fmessage-length=0 -fsigned-char -fno-builtin'
        self._base_release.cflags       = self._base_release.cflags + common_flags + cpp_and_c_flags + asm_and_compile_flags
        self._base_release.c_only_flags = self._base_release.c_only_flags + ' -Wno-discarded-qualifiers -std=gnu17'
        self._base_release.cppflags     = self._base_release.cppflags + ' -fno-threadsafe-statics -fno-rtti -fno-exceptions -x c++ -std=gnu++17 -fno-use-cxa-atexit'  
        self._base_release.cppflags    += ' -Wno-restrict -Wno-address-of-packed-member -Wno-class-memaccess'
        self._base_release.asmflags     = asm_and_compile_flags + ' -c -x assembler-with-cpp'

        linker_search_path1          = os.path.join(uno_src_path, 'variants', 'UNOWIFIR4' )
        core_libs                    = f'-Wl,--whole-archive -Wl,--start-group {uno_src_path}\\variants\\UNOWIFIR4\\libs\\libfsp.a -Wl,--end-group -Wl,--no-whole-archive'
        self._base_release.linklibs  = f' {core_libs} -Wl,--start-group --specs=nano.specs -lm -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys -Wl,--end-group'
        self._base_release.linkflags = f' -Wl,--gc-sections --specs=nosys.specs {common_flags} -L{linker_search_path1} -Wl,-Map,{exename}.map '

    #--------------------------------------------------------------------------
    def link( self, arguments, inf, local_external_setting, variant ):
        # Run the linker
        base.ToolChain.link(self, arguments, inf, local_external_setting, variant, outname=self._final_output_name + ".elf" )

        # Generate the .BIN file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".bin",
               rule       = 'objcpy_rule',
               inputs     = self._final_output_name + ".elf" ,
               variables  = {"objcpy_opts":'-O binary -j .text -j .data '} )
        self._ninja_writer.newline()

        # Run the 'size' command
        self._ninja_writer.rule( 
            name = 'print_size', 
            command = f'$shell {self._printsz} --format=berkeley {self._final_output_name+".elf"}', 
            description = "Generic Command: $cmd" )
        self._create_always_build_statments( "print_size", "dummy_printsize", impilicit_list=self._final_output_name+ ".bin" )
 
    def finalize( self, arguments, builtlibs, objfiles, local_external_setting, linkout=None ):
        self._ninja_writer.default( [self._final_output_name + ".bin", "dummy_printsize_final"] )

        

    #--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext, self._asm_ext2 ]
        return extlist

    #--------------------------------------------------------------------------
    def validate_cc( self ):
        if ( self._env_error != None ):
            exit( "ERROR: The {} environment variable is not set.".format( self._env_error) )
        
        return base.ToolChain.validate_cc(self)


    #--------------------------------------------------------------------------
    # Because Windoze is pain!
    #def _build_ar_rule( self ):
    #    self._ninja_writer.rule( 
    #        name = 'ar', 
    #        command = 'cmd.exe \\C "$rm $out 1>nul 2>nul && $ar ${aropts} ${arout}${out} $in"', 
    #        description = "Archiving Directory: $out" )
    #    self._ninja_writer.newline()
        
        
        
#--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext, self._asm_ext2 ]
        return extlist


    # Because Windoze is pain!
    def _build_ar_rule( self ):
        self._win32_withrspfile_build_ar_rule()

    def _build_compile_rule( self ):
        self._build_withrspfile_compile_rule()

    def _build_assembly_rule( self ):
        self._build_withrspfile_assembly_rule()

    def _build_link_rule( self ):
        self._build_withrspfile_link_rule()        