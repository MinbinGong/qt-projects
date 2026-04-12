/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *imageLayout;
    QVBoxLayout *leftLayout;
    QPushButton *loadBaseBtn;
    QLabel *baseImageLabel;
    QVBoxLayout *rightLayout;
    QComboBox *cameraComboBox;
    QPushButton *cameraBtn;
    QLabel *cameraImageLabel;
    QPushButton *compareBtn;
    QLabel *resultLabel;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 593);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setObjectName("mainLayout");
        imageLayout = new QHBoxLayout();
        imageLayout->setObjectName("imageLayout");
        leftLayout = new QVBoxLayout();
        leftLayout->setObjectName("leftLayout");
        loadBaseBtn = new QPushButton(centralWidget);
        loadBaseBtn->setObjectName("loadBaseBtn");

        leftLayout->addWidget(loadBaseBtn);

        baseImageLabel = new QLabel(centralWidget);
        baseImageLabel->setObjectName("baseImageLabel");
        baseImageLabel->setStyleSheet(QString::fromUtf8("border: 1px solid gray;"));
        baseImageLabel->setAlignment(Qt::AlignCenter);
        baseImageLabel->setProperty("fixedSize", QVariant(QSize(300, 300)));

        leftLayout->addWidget(baseImageLabel);


        imageLayout->addLayout(leftLayout);

        rightLayout = new QVBoxLayout();
        rightLayout->setObjectName("rightLayout");
        cameraComboBox = new QComboBox(centralWidget);
        cameraComboBox->setObjectName("cameraComboBox");

        rightLayout->addWidget(cameraComboBox);

        cameraBtn = new QPushButton(centralWidget);
        cameraBtn->setObjectName("cameraBtn");

        rightLayout->addWidget(cameraBtn);

        cameraImageLabel = new QLabel(centralWidget);
        cameraImageLabel->setObjectName("cameraImageLabel");
        cameraImageLabel->setStyleSheet(QString::fromUtf8("border: 1px solid gray;"));
        cameraImageLabel->setAlignment(Qt::AlignCenter);
        cameraImageLabel->setProperty("fixedSize", QVariant(QSize(300, 300)));

        rightLayout->addWidget(cameraImageLabel);


        imageLayout->addLayout(rightLayout);


        mainLayout->addLayout(imageLayout);

        compareBtn = new QPushButton(centralWidget);
        compareBtn->setObjectName("compareBtn");

        mainLayout->addWidget(compareBtn);

        resultLabel = new QLabel(centralWidget);
        resultLabel->setObjectName("resultLabel");
        resultLabel->setAlignment(Qt::AlignCenter);

        mainLayout->addWidget(resultLabel);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 800, 24));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "ProductChecker", nullptr));
        loadBaseBtn->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\345\237\272\345\207\206\345\233\276\347\211\207", nullptr));
        baseImageLabel->setText(QCoreApplication::translate("MainWindow", "\345\237\272\345\207\206\345\233\276\347\211\207", nullptr));
#if QT_CONFIG(tooltip)
        cameraComboBox->setToolTip(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\347\233\270\346\234\272", nullptr));
#endif // QT_CONFIG(tooltip)
        cameraBtn->setText(QCoreApplication::translate("MainWindow", "\346\213\215\346\221\204\345\233\276\347\211\207", nullptr));
        cameraImageLabel->setText(QCoreApplication::translate("MainWindow", "\347\233\270\346\234\272\345\233\276\347\211\207", nullptr));
        compareBtn->setText(QCoreApplication::translate("MainWindow", "\346\257\224\350\276\203\345\233\276\347\211\207", nullptr));
        resultLabel->setText(QCoreApplication::translate("MainWindow", "\346\257\224\350\276\203\347\273\223\346\236\234", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
