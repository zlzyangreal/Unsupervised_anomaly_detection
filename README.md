# Brief introduction
According to the survey, in the field of PCB anomaly detection, the traditional manual detection method not only consumes a lot of human resources, but also seriously slows down the pace of PCB production. These problems have become bottlenecks restricting scientific and technological progress and sustainable development. Therefore, it is urgent to develop an efficient PCB defect detection algorithm.     
Our team plans to adopt a data-driven approach, take PCB boards as the research object, and focus on improving the accuracy of PCB anomaly detection and localization. The algorithm was optimized by pruning, quantization and distillation to improve the execution efficiency of the algorithm, and was deployed to the embedded development board to enhance the real-time processing ability of the algorithm.     
Our team uses three ways to perform PCB anomaly detection, which are  1. Machine vision 2. EfficientAD 3. ADMM-RegAD (self-developed algorithm).   In addition, we also have a beautiful UI human-computer interaction interface.We chose the domestic AGC6490 development board of Chinese company Fibocom Wireless Inc. as the core technology platform of our project and completed this project.
# Product effect display
## 1. Machine vision
![alt text](<Result image display/MResult.png>)
## 2. EfficientAD
![alt text](<Result image display/EffentADResult.png>)
## 3. ADMM-RegAD
![alt text](<Result image display/ADMMResult.png>)
## 4. UI

# file structure
- 1.Embedded_side_important_code:The main code of the embedded end, the core code of the whole project
- 2.Result image display:Image display of the running results of the project
- 3.train:The training part of the item model