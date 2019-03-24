==============================================================
Unofficial MAMBA/PRX Autoloader v3.1.4 by (c) 2016 Ps3ita Team 
Original author (c) 2015 NzV

Load MAMBA and/or VSH plugins (with MAMBA or PRX Loader) at system boot using New Core.

The generated "sys_init_osd.self" should replace sys_init_osd.self in /dev_flash/sys/internal/
who need to be previously renamed as sys_init_osd_orig.self
 
===============================
			[FLAGS]
===============================

Lv2 kernel DEX is slower to mount /dev_usb than the CEX one, is take 13 seconds
but you can't wait 13 seconds after than a lv2 soft reboot is done, because it loaded the xmb faster and
you can't intercept the hash of sprxs and patch it.
For this the new_core look first if the flag "/dev_hdd/tmp/core_flags/nousb" exist and only if it is not found load the flags from USB.
NOTE: the file "/dev_hdd/tmp/core_flags/nousb" is automatically created from MAMBA_PRX_Loader and Funny Mamba Autoloader Installer.

Flags can be placed in /dev_usb000/core_flags/ or /dev_usb001/core_flags/ or /dev_hdd0/tmp/core_flags/

"nousb"			Don't load flags from USB (obviously this only work if placed in /dev_hdd0/tmp/core_flags/)
"failsafe"  	Start in normal mode (MAMBA and VSH plugins not loaded)
"mamba_off"   	Don't load  MAMBA (PRX Loader will be used instead of MAMBA to load VSH plugins)
"noplugins"   	Don't load  VSH plugins at boot
"verbose"   	Enable log and write it in /dev_usb000 or /dev_usb001 or /dev_hdd0

===============================
		[VSH PLUGINS]
===============================

If flag "mamba_off" is not set VSH Plugins will be loaded from file /dev_hdd0/mamba_plugins.txt with MAMBA
else they will be loaded from file /dev_hdd0/prx_plugins.txt with prx loader

==============================================================
==============================================================

(c) 2013 MiralaTijera <www.elotrolado.net> (Original Core)
(c) 2013 Estwald <www.elotrolado.net> (New Core)

"New Core" is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

"New Core" is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with "New Core". If not, see <http://www.gnu.org/licenses/>.

==============================================================
