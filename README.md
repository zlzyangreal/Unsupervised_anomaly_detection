# Brief introduction
According to the survey, in the field of PCB anomaly detection, the traditional manual detection method not only consumes a lot of human resources, but also seriously slows down the pace of PCB production. These problems have become bottlenecks restricting scientific and technological progress and sustainable development. Therefore, it is urgent to develop an efficient PCB defect detection algorithm.     
Our team plans to adopt a data-driven approach, take PCB boards as the research object, and focus on improving the accuracy of PCB anomaly detection and localization. The algorithm was optimized by pruning, quantization and distillation to improve the execution efficiency of the algorithm, and was deployed to the embedded development board to enhance the real-time processing ability of the algorithm.     
Our team uses three ways to perform PCB anomaly detection, which are  1. Machine vision 2. EfficientAD 3. ADMM-RegAD (self-developed algorithm).   In addition, we also have a beautiful UI human-computer interaction interface.We chose the domestic SC171 development board of Chinese company Fibocom Wireless Inc. as the core technology platform of our project and completed this project.
# Product effect display
## 1. Display of detection results
### 1.1 Machine vision
![alt text](<Result image display/MResult.png>)
### 1.2 EfficientAD
![alt text](<Result image display/EffentADResult.png>)
### 1.3 ADMM-RegAD
![alt text](<Result image display/ADMMResult.png>)

For more demos of ADMM-RegAD results please see the [Result image display](<Result image display/ADMM_Multiple_results_presentation>) folder
## 2. UI Design Display
### 2.1 Real-time acquisition interface
![alt text](<Result image display/qt1.png>)
### 2.2 Defect detection result location interface
![alt text](<Result image display/qt3.png>)
### 2.3 Open dataset selection interface
![alt text](<Result image display/qt4.png>)
![alt text](<Result image display/qt5.png>)
### 2.4 Help manual interface
![alt text](<Result image display/qt2.png>)

# File Directory Introduction
- 1.Embedded_side_important_code:The main code of the embedded end, the core code of the whole project
- 2.Result image display:Image display of the running results of the project
- 3.train:The training part of the item model
- 4.parper:Engineering Reference Papers