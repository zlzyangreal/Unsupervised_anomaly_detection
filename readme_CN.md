# 简介
根据调查发现，在PCB异常检测领域，传统的人工检测方法不仅耗费大量人力资源，而且严重拖慢PCB的生产节奏，这些问题成为制约科技进步和可持续发展的瓶颈。因此，开发一种高效的PCB缺陷检测算法迫在眉睫。

我们团队计划采用数据驱动的方法，以PCB板为研究对象，并围绕提高PCB异常检测与定位的准确性进行研究。通过对算法进行剪枝、量化和蒸馏等优化措施，提高算法的执行效率，并将其部署至嵌入式开发板，以增强算法的实时处理能力。 

我们团队使用三种方式进行PCB异常检测，分别是1.机器视觉2.EfficientAD3.ADMM-RegAD（自研算法），除此之外我们还具备精美的UI人机交互界面。我们选择广和通公司的国产SC171开发板作为我们项目的核心技术平台，完成了这个项目。
# 产品效果展示
## 1.检测结果展示
### 1.1机器视觉
![alt text](<Result image display/MResult.png>)
### 1.2 EfficientAD
![alt text](<Result image display/EffentADResult.png>)
### 1.3 ADMM-RegAD
![alt text](<Result image display/ADMMResult.png>)

更多的关于ADMM-RegAD结果演示请查看[Result image display](<Result image display/ADMM_Multiple_results_presentation>)文件夹
## 2. UI设计展示
### 2.1 实时采集界面
![alt text](<Result image display/qt1.png>)
### 2.2 缺陷检测结果定位界面
![alt text](<Result image display/qt3.png>)
### 2.3 公开数据集选择界面
![alt text](<Result image display/qt4.png>)
![alt text](<Result image display/qt5.png>)
### 2.4 帮助手册界面
![alt text](<Result image display/qt2.png>)
# 文件目录简介
- 1.Embedded_side_important_code:项目的核心代码
- 2.Result image display:结果展示图片
- 3.train:项目的模型训练代码
- 4.parper:工程参考论文
