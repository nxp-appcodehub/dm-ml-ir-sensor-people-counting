# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## Leveraging deep learning to count the number of people in a room using a low-resolution 8x8 infrared array sensor

This demo shows how to create a deep learning model to count the number of people in a room using a low-resolution 8x8 infrared array sensor.

#### Boards: LPCXpresso55S69, LPCXpresso55S28
#### Categories: AI/ML, Vision
#### Peripherals: I2C, SENSOR, UART, TIMER, CLOCKS
#### Toolchains: MCUXpresso IDE

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [FAQs](#step5)
6. [Support](#step6)
7. [Release Notes](#step7)

## 1. Software<a name="step1"></a>
* [MCUXpresso v11.8.0](https://nxp.com/mcuxpresso)
* LPCXpresso55S69 SDK from the [MCUXpresso SDK Builder](https://mcuxpresso.nxp.com/en/select) or from the [MCUXpresso SDK repository](https://github.com/nxp-mcuxpresso/mcux-sdk).
* [Python 3.11+](https://www.python.org/downloads/)
* [Jupyter notebook](https://jupyter.org/install) or [VS Code](https://code.visualstudio.com/docs/datascience/jupyter-notebooks)

## 2. Hardware<a name="step2"></a>
* [LPCXpresso55s69](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/lpcxpresso-boards/lpcxpresso55s69-development-board:LPC55S69-EVK)
* [LPCXpresso55s28](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/lpcxpresso-boards/lpcxpresso55s28-development-board:LPC55S28-EVK)
* [Grid-EYE CLICK](https://www.mikroe.com/grid-eye-click)
* Micro USB cable
## 3. Setup<a name="step3"></a>
### 3.1 MCU Application Setup
1. Add the LPCXpresso55S69 SDK to the MCUXpresso IDE by right clicking on `Installed SDKs` and selecting `Import archive...` for the SDK downloaded from the SDK Builder or `Import remote SDK Git repository...` for the MCUXpresso SDK repository.  
![add_sdk](images/add_sdk.png)
2. Clone this repository anywhere in your drive.
3. Open MCUXpresso and select `File` -> `Open Projects from File System...`.  
![opne_project](images/open_project.png)
4. Under Import Source, select `Directory...`, navigate to `<repo_location>/mcu_app/<board>` and click on `Select Folder`.
5. Make Sure that the project is selected in the window and click `Finish`.  
![project_importer](images/project_importer.png)
6. Right click on the project in the `Project Explorer` then navigate to `Build Configurations` -> `Set Active` -> `Release`.  
![active_configuration](images/active_config.png)
7. Connect the Grid-EYE CLICK board to `P23` and `P24` as shown in the image below.  
![Board](images/board.png)
8. Connect the board to your computer through the `Debug Link` (`P6`) connector on the board.
9. Build the application by clicking on `build` and then flash it to the board by either clicking on `Debug` or by selecting the gray rectangle on the tool bar.  
![build_and_flash](images/build_and_flash.png)
10. Mount the board to the ceiling for best results. Refer to Fig. 2 in [this research paper](https://arxiv.org/pdf/2304.06059.pdf).

### 3.2 Training Setup
1. We will use VS Code to open and run the Jupyter Notebook.
2. Open VS Code, click on `File` -> `Open Folder`, and navigate to `<repo_location>/training` and click `Select Folder`. Once opened, the Explorer should look like below.  
![training_folder](images/training_folder.png)
3. Open the `create_model.ipynb` file and then click on `Select Kernel` in the top right.  
![open_ipynb](images/open_ipynb.png)
4. Follow the instructions in the pop up to create a virtual environment.
5. After successfully creating a virtual environment, follow the instructions in the notebook to retrain or create a new model.
6. Once you have a new model, modify `mcu_app/common/model/model_config.h` and `mcu_app/common/model/model_ops.cpp` according to your new model.
7. Rebuild the MCU application, flash it to the board, and run it.

## 4. Results<a name="step4"></a>
* When running the model on the board, open a serial terminal and connect to the board to see the inference results.  
![terminal](images/terminal.png)
* When running the model on your computer using real-time data from the board, the animation on the Jupyter Notebook will look like below.  
![animation](images/animation.png)

## 5. FAQs<a name="step5"></a>
1. How do I generate the operations for model_ops.cpp?
    > You can use the [eIQ Toolkit](https://www.nxp.com/design/software/eiq-ml-development-environment/eiq-toolkit-for-end-to-end-model-development-and-deployment:EIQ-TOOLKIT)'s Model Tool to view all of the required operations for your model and manually add them to the ops resolver.

## 6. Support<a name="step6"></a>
Please submit any issues within this GitHub repository.

#### Project Metadata
<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-LPCXPRESSO55S69-blue)](https://github.com/search?q=org%3Anxp-appcodehub+LPCXpresso55S69+in%3Areadme&type=Repositories) [![Board badge](https://img.shields.io/badge/Board-LPCXPRESSO55S28-blue)](https://github.com/search?q=org%3Anxp-appcodehub+LPCXpresso55S28+in%3Areadme&type=Repositories)

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-AI/ML-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+aiml+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-VISION-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+vision+in%3Areadme&type=Repositories)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-I2C-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+i2c+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-SENSOR-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+sensor+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-UART-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+uart+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-TIMER-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+timer+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-CLOCKS-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+clocks+in%3Areadme&type=Repositories)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-MCUXPRESSO%20IDE-orange)](https://github.com/search?q=org%3Anxp-appcodehub+mcux+in%3Areadme&type=Repositories)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected funcionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/@NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/Twitter-Follow%20us%20on%20Twitter-white.svg)](https://twitter.com/NXP)

## 7. Release Notes<a name="step7"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | September 26<sup>nd</sup> 2023 |

