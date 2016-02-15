# UCSB Hyperloop
![pod_gif_here](http://www.ucsbhyperloop.com/assets/ucsbhyperloop-d2207c2e7ad8ddb19f23f307bd09f1a862927b595cb68cd3478cf50093f5d581.gif)

## Who We Are
We are an interdisiplinary team of 30 students and professors competing in SpaceX's Hyperloop Pod Competition. The competition involves designing and building a pod to be tested on a track constructed at SpaceX's Hawthorne headquarters in June 2016.

## What is the Hyperloop?
The Hyperloop is a proposed advanced method of high speed transportation between cities that are less than about 900 miles apart (e.g. Los Angeles and San Francisco). The system uses a capsule that is propelled through a low pressure steel tube as it rests on frictionless air bearings or magnets.

## What is this repository?
Our design involves two LPC 4088 microcontroller boards for all sensor, communications, and controls relavent to our pod. The code present here is what drives this functionality of our design.

## Download Instructions and Activation
1. Download LPCXpresso V. 7.3 (Mac: https://www.lpcware.com/lpcxpresso/downloads/macosx, Windows: https://www.lpcware.com/lpcxpresso/downloads/windows).
2. Follow all the License and Download information. 
3. Activate your free license by following this link: https://www.lpcware.com/lpcxpresso/activate. You will have to create an account to continue.
4. In LPCXpresso, click Help -> Activate -> Create serial number and register (Free Edition)...
5. Paste the serial number into the the website listed in step 3, and an LPCXpresso Activation Key will be given to you.
6. Click Help -> Activate -> Activate (Free Edition)... and paste the code into there. Ensure that license files are created and you are prompted with a successful activation message. You can find where the license files are at the following link (https://www.lpcware.com/content/faq/lpcxpresso/where-license-stored).

## For Windows Users (Downloading Github Application)
1. Download the Github application (https://desktop.github.com/) and make sure to that the Git Shell application is installed as well.
2. You'll need the Git Shell since Windows does not have git command line tools natively installed.

## Setting Up LPCXpresso with LPCOpen and Making a Project.
1. Open LPCXpresso and select your workplace. 
2. Select File -> New Project.
3. Select LPCXpresso C Project under the C/C++ tab.
4. Under the LPC4x section, select LPC407x_8x, then select LPCOpen - C Project.
5. Name your project "ucsb_hyperloop".
6. Select LPC4088 again.
7. Now you will have to download LPCOpen. Click the link on the bottom of the window (http:/www.lpcware.com/lpcopen).
8. Click "Click here to go to the LPCOpen product family download page".
9. LPCOpen v2.xx for LPC40xx family devices.
10. Under the latest available table, click the v2.10 download link for LPCXpresso v7.0.2_102. 
11. Move "lpcopen_2_10_lpcxpresso_ea_devkit_4088" zip folder to somewhere other than your Downloads folder.
12. Back to LPCXpresso, under LPCOpen Chip Library Priject, click Import.
13. Browse for the zip archive and select wherever you moved it to and click Finish.
14. Now all the example code and chip/board code should be in your workspace.
15. Under LPCOpen Chip Library Project, select Browse... and find "lpc_chip_40xx" and click OK.
16. Under LPCOpen Board Library Project, select Browse... and find "lpc_board_ea_devkit_4088" and click OK.
17. Click Next on the CMSIS DSP window. 
18. Click next on the Floating Point Unit window (Enabled SoftABI should be selected by default).
19. Click next and keep all default option on the "Other options" screen.
20. Click next and keep both options unchecked on the Printf options screen.
21. Click Finish.

## Cloning the Git Repositiory 
1. First, remove the src folder created when the project was created.
2. In the Git Powershell (or Terminal for Windows), navigate to the project directory you just made. For me, it was /Users/myname/LPCXpresso/ucsb_hyperloop.
3. Go to the git respository online (https://github.com/camsb/UCSBHyperLoop) and copy the link by the "HTTPS" button. Currently this shows (https://github.com/camsb/UCSBHyperLoop.git).
4. In the directory you navigated to, perform the following command: "git clone https://github.com/camsb/UCSBHyperLoop.git"
5. Rename this directory back to "src".
6. You now have the latest source code and a working project.
