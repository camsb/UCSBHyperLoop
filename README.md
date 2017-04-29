# UCSB Hyperloop
![pod_gif_here](http://www.ucsbhyperloop.com/assets/ucsbhyperloop-d2207c2e7ad8ddb19f23f307bd09f1a862927b595cb68cd3478cf50093f5d581.gif)

## Who are We?
We are an interdisiplinary team of 32 students and professors competing in SpaceX's Hyperloop Pod Competition. The competition involves designing and building a pod to be tested on a track constructed at SpaceX's Hawthorne headquarters in August 2017. Our main website can be found at http://www.ucsbhyperloop.com/

The eight Electrical and Computer Engineering students currently responsible for this repository include:
 
- [Celeste Bean](https://github.com/celestebean)  
- [Yang Ren](https://github.com/yangr13)  
- [Tristan Seroff](https://github.com/t-seroff)  
- [Asitha Kaduwela](https://github.com/akaduwela)  
- [Jesus Diera](https://github.com/jdiera)  
- [Kevin Kha](https://github.com/killer225whale)  
- [Brian Canty](https://github.com/ece-god)  
- [Ricky Castro](https://github.com/ricky-27)  

Previous students who managed this repository include:
- [Benjamin Hartl](https://github.com/benjaminhartl)
- [Cameron McCarthy](https://github.com/camsb)
- [Connor Buckland](https://github.com/cbuckland)
- [Connor Mulcahey](https://github.com/connormulcahey)

## What is the Hyperloop?
The Hyperloop is a proposed advanced method of high speed transportation between cities that are less than about 900 miles apart (e.g. Los Angeles and San Francisco). The system uses a capsule that is propelled through a low pressure steel tube as it rests on frictionless air bearings or magnets.

## What is this Repository?
Our design involves two LPC4088 microcontroller chips for all sensor, communications, and controls relavent to our pod. We have designed a circuit board from the ground up to handle our specific requirements for these measurements. The code present here is what drives this functionality of our design.

**Important Note:** This repo now contains an entire workspace, and not just a single project!

## Download Instructions and Activation
1. Download LPCXpresso V. 7.3 (Mac: https://www.lpcware.com/lpcxpresso/downloads/macosx, Windows: https://www.lpcware.com/lpcxpresso/downloads/windows).
2. Follow all the license and download information. 
3. Activate your free license by following this link: https://www.lpcware.com/lpcxpresso/activate. You will have to create an account to continue.
4. In LPCXpresso, click Help -> Activate -> Create serial number and register (Free Edition)...
5. Paste the serial number into the the website listed in step 3, and an LPCXpresso Activation Key will be given to you.
6. Click Help -> Activate -> Activate (Free Edition)... and paste the code into there. Ensure that license files are created and you are prompted with a successful activation message. You can find where the license files are at the following link (https://www.lpcware.com/content/faq/lpcxpresso/where-license-stored).

## Downloading the Github Application (Required for Windows Users)
1. Download the Github application (https://desktop.github.com/) and make sure to that the Git Shell application is installed as well.
2. Windows Users: You'll need the Git Shell since Windows does not have git command line tools natively installed.

## Cloning the Git Repositiory 
### Method 1: Via the GitHub Application
1. Go to the git respository online (https://github.com/YangR13/UCSBHyperloop) and click the green "Clone or download" button.
2. Click the "Open in Desktop" button. This should open the GitHub application and a new window for selecting a cloning directory.
3. Navigate to the directory where you want to save the your new UCSBHyperloop **workspace**. For me, it was /Users/myname/LPCXpresso/.
4. Click "Clone".

### Method 2: Via the Git Shell / Terminal
1. Go to the git respository online (https://github.com/YangR13/UCSBHyperloop) and click the green "Clone or download" button.
2. Click the "Copy to clipboard" icon to the right of the web URL. Currently this shows (https://github.com/YangR13/UCSBHyperloop.git).
3. In the Git Shell or Terminal for Mac, navigate to the directory where you want to save the your new UCSBHyperloop **workspace**. For me, it was /Users/myname/LPCXpresso/.
4. In the directory you navigated to, perform the following command: "git clone https://github.com/YangR13/UCSBHyperloop.git".

## Setting up LPCXpresso
### Switching Workspaces
1. Open LPCXpresso.
2. Select "File" -> "Switch Workspace" -> "Other..." from the menu bar.
3. Click "Browse..."
4. Navigate to the directory where you saved the "UCSBHyperloop" workspace folder and select it (the "UCSBHyperloop" folder).
5. Click "Open" -> "Ok".
6. LPCXpresso will restart.

### Importing Projects
1. Open LPCXpresso.
2. Select "File" -> "Import..." from the menu bar.
3. Expand the "General" option and select "Existing Projects into Workspace".
4. Click "Next >".
5. Click "Browse..." next to the field for "Select root directory:".
6. Navigate to the directory where you saved the "UCSBHyperloop" workspace folder and select it (the "UCSBHyperloop" folder).
7. Click "Open" -> "Finish".
8. The projects should show up in the "Project Explorer" window.
