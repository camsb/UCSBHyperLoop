# Extra Info: Setting Up a New Project
## Making a New Project for the LPC4088
1. Open LPCXpresso and select your workspace. 
2. Select File -> New Project.
3. Select LPCXpresso C Project under the C/C++ tab.
4. Under the LPC4x section, select LPC407x_8x, then select LPCOpen -> C Project.
5. Name your project (e.g. "ucsb_hyperloop").
6. Select LPC4088 again.
7. Under LPCOpen Chip Library Project, select Browse... and find "lpc_chip_40xx" and click OK.
8. Under LPCOpen Board Library Project, select Browse... and find "lpc_board_ea_devkit_4088" and click OK.
9. Click Next on the CMSIS DSP window. 
10. Click next on the Floating Point Unit window (Enabled SoftABI should be selected by default).
11. Click next and keep all default option on the "Other options" screen.
12. Click next and keep both options unchecked on the Printf options screen.
13. Click Finish.

## Setting Up LPCXpresso with LPCOpen
1. Follow this [link](http://www.nxp.com/products/microcontrollers-and-processors/arm-processors/lpc-cortex-m-mcus/lpc-cortex-m3/lpc1800-cortex-m3/lpcopen-software-development-platform-lpc40xx:LPCOPEN-SOFTWARE-FOR-LPC40XX).
2. Under the latest available table, click the v2.10 download link for LPCXpresso v7.0.2_102. 
3. Move the "lpcopen_2_10_lpcxpresso_ea_devkit_4088" zip folder to somewhere other than your Downloads folder.
4. Open LPCXpresso and select your workspace.
5. Click File -> Import... -> General -> Existing Projects into Workspace -> Next -> Select archive file: -> Browse.
6. Navigate to and select the "lpcopen_2_10_lpcxpresso_ea_devkit_4088" zip folder you downloaded.
7. Deselect any projects you don't want to import (you only need "lpc_board_ea_devkit_4088" and "lpc_chip_40xx" but the other projects contain helpful example code).
8. Click Finish.
