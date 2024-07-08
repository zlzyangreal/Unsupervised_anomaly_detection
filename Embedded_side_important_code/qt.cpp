#pragma execution_character_set("utf-8") // 必须是tuf-8格式的文件

#include "WidgetMain.h"
#include "ui_WidgetMain.h"
#include "iconhelper.h"
#include "quihelper.h"

WidgetMain::WidgetMain(QWidget *parent) : 
    QWidget(parent), 
    ui(new Ui::WidgetMain)
{
    ui->setupUi(this);
    process = new QProcess(this);
    videoThread = new ControlCV(this, 0, cv::Size(640, 480), 30);
    this->initForm();
    this->initStyle();
    this->initLeftMain();
    this->initLeftConfig();
    // 程序进程
    this->ui->progressBar->setValue(0);
    this->ui->lab3->setStyleSheet("font-size: 20px; background-color: lightgreen; color: brown; border: 2px solid black;");
    this->ui->label_pathText->setStyleSheet("font-size: 30px; background-color: lightyellow; color: orange; border: 2px solid black;");
    this->ui->lab4->setStyleSheet("QLabel {"
                     "  color: #333;           /* 字体颜色 */"
                     "  background-color: #eee; /* 背景颜色 */"
                     "  font: bold 14px;      /* 字体样式、大小 */"
                     "  border: 1px solid #ccc; /* 边框 */"
                     "  padding: 5px;          /* 内边距 */"
                     "  border-radius: 10px;  /* 边框圆角 */"
                     "}");
    connect(this->ui->pushButton_select, &QPushButton::clicked, this, &WidgetMain::openFile);
    connect(process, &QProcess::started, [this]() {
        this->ui->progressBar->setValue(0); // 重置进度条
        this->ui->progressBar->show();      // 显示进度条
    });
    connect(process, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
        qDebug() << "Process error:" << error;
    });

    // 假设外部程序支持通过标准错误输出进度信息
    connect(process, &QProcess::readyReadStandardError, [this]() {
        while (this->process->canReadLine()) {
            const QString line = this->process->readLine();
            if (line.startsWith("Progress:")) { // 假设进度信息以 "Progress:" 开头
                int progressValue = line.mid(9).trimmed().toInt();
                this->ui->progressBar->setValue(progressValue);
            }
        }
    });
}

WidgetMain::~WidgetMain()
{
    delete ui;
}

bool WidgetMain::eventFilter(QObject *watched, QEvent *event)
{
    // 标题栏事件过滤器
    if (watched == ui->widgetTitle) { 
        if (event->type() == QEvent::MouseButtonDblClick) {
            // 双击标题栏放大
            on_btnMenu_Max_clicked();
        }
    }
    return QWidget::eventFilter(watched, event); // 返回父类的事件过滤器
}

void WidgetMain::callExternalProgram(const QString &program, const QStringList &arguments)
{
    this->ui->progressBar->setValue(0);
    process->start(program, arguments);
    if (!process->waitForStarted()) {
        qDebug() << "程序启动失败:" << process->errorString();
        return;
    }
    QByteArray output = process->readAllStandardOutput();
    QByteArray errorOutput = process->readAllStandardError();

    // 程序运行中，可以定期检查输出或等待程序结束
    while (!process->waitForFinished(100)) { // 每100ms检查一次
        // 读取并打印输出
        QByteArray outputAvailable = process->readAllStandardOutput();
        if (!outputAvailable.isEmpty()) {
            qDebug() << "程序输出:" << outputAvailable<<endl;
        }
        // 读取并打印错误信息
        QByteArray errorAvailable = process->readAllStandardError();
        if (!errorAvailable.isEmpty()) {
            qDebug() << "程序错误输出:" << errorAvailable<<endl;
        }
    }
    // 程序结束后，获取最终的输出和错误信息
    QByteArray finalOutput = process->readAllStandardOutput();
    QByteArray finalErrorOutput = process->readAllStandardError();
    // 打印最终输出和错误信息
    if (!finalOutput.isEmpty()) {
        qDebug() << "最终程序输出:" << finalOutput<<endl;
    }
    if (!finalErrorOutput.isEmpty()) {
        qDebug() << "最终程序错误输出:" << finalErrorOutput<<endl;
    }
    int exitCode = process->exitCode();
    qDebug() << "Process finished with exit code" << process->exitCode();
    ui->progressBar->setValue(100); // 完成进度
}

void WidgetMain::getQssColor(const QString &qss, const QString &flag, QString &color)
{
    int index = qss.indexOf(flag);
    if (index >= 0) {
        color = qss.mid(index + flag.length(), 7);
    }
    //qDebug() << TIMEMS << flag << color;
}

void WidgetMain::getQssColor(const QString &qss, QString &textColor, QString &panelColor,
                          QString &borderColor, QString &normalColorStart, QString &normalColorEnd,
                          QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    getQssColor(qss, "TextColor:", textColor);
    getQssColor(qss, "PanelColor:", panelColor);
    getQssColor(qss, "BorderColor:", borderColor);
    getQssColor(qss, "NormalColorStart:", normalColorStart);
    getQssColor(qss, "NormalColorEnd:", normalColorEnd);
    getQssColor(qss, "DarkColorStart:", darkColorStart);
    getQssColor(qss, "DarkColorEnd:", darkColorEnd);
    getQssColor(qss, "HighColor:", highColor);
}

void WidgetMain::initForm()
{
    //设置无边框
    QUIHelper::setFramelessForm(this);
    //设置图标
    IconHelper::setIcon(ui->labIco, 0xf073, 30);
    IconHelper::setIcon(ui->btnMenu_Min, 0xf068);
    IconHelper::setIcon(ui->btnMenu_Max, 0xf067);
    IconHelper::setIcon(ui->btnMenu_Close, 0xf00d);

    //ui->widgetMenu->setVisible(false);
    ui->widgetTitle->setProperty("form", "title");
    //关联事件过滤器用于双击放大
    ui->widgetTitle->installEventFilter(this);
    ui->widgetTop->setProperty("nav", "top");

    QFont font;
    font.setPixelSize(25);
    ui->labTitle->setFont(font);
    ui->labTitle->setText("PCB异常检测系统");
    this->setWindowTitle(ui->labTitle->text());

    ui->stackedWidget->setStyleSheet("QLabel{font:60px;}");

    QSize icoSize(32, 32);
    int icoWidth = 85;

    //设置顶部导航按钮
    QList<QAbstractButton *> tbtns = ui->widgetTop->findChildren<QAbstractButton *>();
    foreach (QAbstractButton *btn, tbtns) {
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }

    ui->btnMain->click();

    ui->widgetLeftMain->setProperty("flag", "left");
    ui->widgetLeftConfig->setProperty("flag", "left");
    ui->page1->setStyleSheet(QString("QWidget[flag=\"left\"] QAbstractButton{min-height:%1px;max-height:%1px;}").arg(60));
    ui->page2->setStyleSheet(QString("QWidget[flag=\"left\"] QAbstractButton{min-height:%1px;max-height:%1px;}").arg(25));
}

void WidgetMain::initStyle()
{
    //加载样式表
    QString qss;
    QFile file(":/qss/blacksoft.css");
    if (file.open(QFile::ReadOnly)) {
        qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(paletteColor));
        qApp->setStyleSheet(qss);
        file.close();
    }

    //先从样式表中取出对应的颜色
    QString textColor, panelColor, borderColor, normalColorStart, normalColorEnd, darkColorStart, darkColorEnd, highColor;
    getQssColor(qss, textColor, panelColor, borderColor, normalColorStart, normalColorEnd, darkColorStart, darkColorEnd, highColor);

    //将对应颜色设置到控件
    this->borderColor = highColor;
    this->normalBgColor = normalColorStart;
    this->darkBgColor = panelColor;
    this->normalTextColor = textColor;
    this->darkTextColor = normalTextColor;
}

void WidgetMain::buttonClick()
{
    QAbstractButton *b = (QAbstractButton *)sender();
    QString name = b->text();

    QList<QAbstractButton *> tbtns = ui->widgetTop->findChildren<QAbstractButton *>();
    foreach (QAbstractButton *btn, tbtns) {
        btn->setChecked(btn == b);
    }

    if (name == "主界面") {
        ui->stackedWidget->setCurrentIndex(0); // 切换到主界面
    } else if (name == "系统设置") {
        ui->stackedWidget->setCurrentIndex(1); // 切换到系统设置
    } else if (name == "数据选择") {
        ui->stackedWidget->setCurrentIndex(2); // 切换到警情查询
    } else if (name == "调试帮助") {
        ui->stackedWidget->setCurrentIndex(3); // 切换到调试帮助
    } else if (name == "用户退出") {
        exit(0);
    }
}

void WidgetMain::initLeftMain()
{
    iconsMain << 0xf030 << 0xf03e << 0xf247;
    btnsMain << ui->tbtnMain1 << ui->tbtnMain2 << ui->tbtnMain3;
    
    int count = btnsMain.count();
    for (int i = 0; i < count; ++i) {
        QToolButton *btn = (QToolButton *)btnsMain.at(i);
        btn->setCheckable(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(leftMainClick()));
    }
    // 连接按钮的clicked信号到显示/隐藏视频显示控件的槽函数
    connect(
        ui->button_start, &QPushButton::clicked, 
        this, [this]() {
            this->videoThread->_switch = false;
            this->videoThread->start(); // 启动线程
        }
    );
    connect(
        ui->button_capture, &QPushButton::clicked, 
        
    );
    connect(videoThread, &ControlCV::frame_ready, this, &WidgetMain::update_frame);
    connect(videoThread, &ControlCV::frame_capture, this, &WidgetMain::update_frame);

    IconHelper::StyleColor styleColor;
    styleColor.position = "left";
    styleColor.iconSize = 18;
    styleColor.iconWidth = 35;
    styleColor.iconHeight = 25;
    styleColor.borderWidth = 4;
    styleColor.borderColor = borderColor;
    styleColor.setColor(normalBgColor, normalTextColor, darkBgColor, darkTextColor);
    IconHelper::setStyle(ui->widgetLeftMain, btnsMain, iconsMain, styleColor);
    ui->tbtnMain1->click();
}

void WidgetMain::initLeftConfig()
{
    iconsConfig << 0xf031 << 0xf036 << 0xf249 << 0xf055 << 0xf05a << 0xf249;
    btnsConfig << ui->tbtnConfig1 << ui->tbtnConfig2 << ui->tbtnConfig3 << ui->tbtnConfig4 << ui->tbtnConfig5 << ui->tbtnConfig6;

    int count = btnsConfig.count();
    for (int i = 0; i < count; ++i) {
        QToolButton *btn = (QToolButton *)btnsConfig.at(i);
        btn->setCheckable(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(leftConfigClick()));
    }

    IconHelper::StyleColor styleColor;
    styleColor.position = "left";
    styleColor.iconSize = 16;
    styleColor.iconWidth = 20;
    styleColor.iconHeight = 20;
    styleColor.borderWidth = 3;
    styleColor.borderColor = borderColor;
    styleColor.setColor(normalBgColor, normalTextColor, darkBgColor, darkTextColor);
    IconHelper::setStyle(ui->widgetLeftConfig, btnsConfig, iconsConfig, styleColor);
    ui->tbtnConfig1->click();
}

void WidgetMain::leftMainClick()
{
    QAbstractButton *b = (QAbstractButton *)sender();
    QString name = b->text();

    int count = btnsMain.count();
    for (int i = 0; i < count; ++i) {
        QAbstractButton *btn = btnsMain.at(i);
        btn->setChecked(btn == b); // 设置选中状态白条
        if (btn == b) {
            ui->stackedWidget_Face->setCurrentIndex(i); // 切换到对应界面
        }
    }
}

void WidgetMain::leftConfigClick()
{
    QToolButton *b = (QToolButton *)sender();
    QString name = b->text();

    int count = btnsConfig.count();
    for (int i = 0; i < count; ++i) {
        QAbstractButton *btn = btnsConfig.at(i);
        btn->setChecked(btn == b);
    }

    // ui->lab2->setText(name);
}

void WidgetMain::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Select Yours Needed File", 
        QDir::currentPath(), "Video Files (*.jpg *.png *.txt)");
    if (!fileName.isEmpty()) {
        // 检测是否选择文件
        this->ui->label_pathText->setText(fileName);
        selectFilePath = fileName.toStdString();
        // opencv 读取filePath
        int width = 1562;
        int height = 960;
        cv::Mat resizedFrame;
        cv::Mat selectImage = cv::imread(selectFilePath, cv::IMREAD_COLOR);
        std::string filePath = "/home/aidlux/Desktop/CODE/plan2/python/Efficient-ADLKlk/Start/000.png";
        cv::resize(selectImage, resizedFrame, cv::Size(width, height));
        cv::imwrite(filePath, resizedFrame);
        if(!selectImage.empty()) {
            int labelWidth = 850;
            int labelHeight = 450;
            cv::resize(selectImage, selectImage, cv::Size(labelWidth, labelHeight));
            cv::cvtColor(selectImage, selectImage, cv::COLOR_BGR2RGB);
            QImage qimage = QImage(selectImage.data, selectImage.cols, 
                                    selectImage.rows, selectImage.step, 
                                    QImage::Format_RGB888);
            qimage = qimage.scaled(labelWidth, labelHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            this->ui->label_selectImage->setPixmap(QPixmap::fromImage(qimage));
        } else {
            qDebug() << "Could not read the image";
        }
    }
}

void WidgetMain::update_frame(std::shared_ptr<cv::Mat> frame) {
    // 将OpenCV图像转换为Qt图像格式，并更新到QLabel上
    if (frame.get()) {
        cv::cvtColor(*frame, *frame, cv::COLOR_BGR2RGB);
        QImage qimage = QImage(frame->data, frame->cols, frame->rows, frame->step, QImage::Format_RGB888);
        ui->label_video->setPixmap(QPixmap::fromImage(qimage));
        // frame->release(); // 释放会泄漏
        // frame = nullptr;
    } else {
        // nullptr
        qDebug() << "frame is nullptr";
    }
}

void WidgetMain::on_btnMenu_Min_clicked()
{
    showMinimized();
}

void WidgetMain::on_btnMenu_Max_clicked()
{
    static bool max = false;
    static QRect location = this->geometry();

    if (max) {
        this->setGeometry(location);
    } else {
        location = this->geometry();
        this->setGeometry(QUIHelper::getScreenRect());
    }

    this->setProperty("canMove", max);
    max = !max;
}

void WidgetMain::on_btnMenu_Close_clicked()
{
    close();
}

void WidgetMain::on_progressBar_valueChanged(int value)
{
    if (value == 100) {
        qDebug() << "bar ok get result";
        // 指定图像的路径
        std::string resultImagePath = "/home/aidlux/Desktop/CODE/plan2/Result/Result.png"; // 替换为你的图像路径
        // 使用cv::imread读取图像
        cv::Mat resultImage = cv::imread(resultImagePath, cv::IMREAD_COLOR);
        // 检查图像是否正确加载
        if(!resultImage.empty()) {
            int labelWidth = 800;
            int labelHeight = 500;
            cv::resize(resultImage, resultImage, cv::Size(labelWidth, labelHeight));
            cv::cvtColor(resultImage, resultImage, cv::COLOR_BGR2RGB);
            QImage qimage = QImage(resultImage.data, resultImage.cols, 
                                   resultImage.rows, resultImage.step, 
                                   QImage::Format_RGB888);
            qimage = qimage.scaled(labelWidth, labelHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->label_result->setPixmap(QPixmap::fromImage(qimage));
        } else {
            qDebug() << "Could not read the image";
        }
    }
}
