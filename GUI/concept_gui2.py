# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'concept_gui2.ui'
#
# Created by: PyQt5 UI code generator 5.13.0
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(476, 306)
        MainWindow.setMinimumSize(QtCore.QSize(0, 0))
        MainWindow.setMaximumSize(QtCore.QSize(800, 600))
        MainWindow.setToolTip("")
        MainWindow.setStyleSheet("*{background-color: rgb(50, 50, 50);\n"
"}\n"
"\n"
"QPushButton{\n"
"    color: rgb(255, 255, 255);\n"
"    background-color: rgb(85,85,85);\n"
"    border: 1px solid #6a6eb0;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgb(110, 110, 110);\n"
"}")
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout_5 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        self.label = QtWidgets.QLabel(self.centralwidget)
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setStyleSheet("color: rgb(255, 255, 255);")
        self.label.setObjectName("label")
        self.horizontalLayout_5.addWidget(self.label)
        spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem)
        self.boutonInfo = QtWidgets.QPushButton(self.centralwidget)
        self.boutonInfo.setMinimumSize(QtCore.QSize(30, 30))
        self.boutonInfo.setMaximumSize(QtCore.QSize(30, 30))
        font = QtGui.QFont()
        font.setFamily("Script MT Bold")
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        self.boutonInfo.setFont(font)
        self.boutonInfo.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.boutonInfo.setStyleSheet("color: rgb(200, 200, 200);\n"
"background-color: rgba(255, 255, 255, 0);\n"
"border-color: rgba(255, 255, 255, 0);\n"
"")
        self.boutonInfo.setObjectName("boutonInfo")
        self.horizontalLayout_5.addWidget(self.boutonInfo)
        self.verticalLayout.addLayout(self.horizontalLayout_5)
        self.listePrescriptions = QtWidgets.QListWidget(self.centralwidget)
        font = QtGui.QFont()
        font.setFamily("Arial")
        self.listePrescriptions.setFont(font)
        self.listePrescriptions.setStyleSheet("color: rgb(255, 255, 255);\n"
"background-color: rgb(80, 80, 80);\n"
"border: 2px solid #6a6eb0;\n"
"border-radius: 5px;")
        self.listePrescriptions.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.listePrescriptions.setResizeMode(QtWidgets.QListView.Adjust)
        self.listePrescriptions.setLayoutMode(QtWidgets.QListView.SinglePass)
        self.listePrescriptions.setViewMode(QtWidgets.QListView.IconMode)
        self.listePrescriptions.setUniformItemSizes(False)
        self.listePrescriptions.setWordWrap(False)
        self.listePrescriptions.setObjectName("listePrescriptions")
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
        self.verticalLayout.addWidget(self.listePrescriptions)
        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(True)
        font.setWeight(75)
        self.label_2.setFont(font)
        self.label_2.setAutoFillBackground(False)
        self.label_2.setStyleSheet("color: rgb(255, 255, 255);")
        self.label_2.setObjectName("label_2")
        self.verticalLayout.addWidget(self.label_2)
        self.messagesTextEdit = QtWidgets.QTextEdit(self.centralwidget)
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(9)
        self.messagesTextEdit.setFont(font)
        self.messagesTextEdit.setStyleSheet("background-color: rgb(80, 80, 80);\n"
"color: rgb(255, 255, 255);\n"
"border: 2px solid #6a6eb0;\n"
"border-radius: 5px;")
        self.messagesTextEdit.setObjectName("messagesTextEdit")
        self.verticalLayout.addWidget(self.messagesTextEdit)
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.boutonDemarer = QtWidgets.QPushButton(self.centralwidget)
        self.boutonDemarer.setMinimumSize(QtCore.QSize(225, 30))
        self.boutonDemarer.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("icon_start.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.boutonDemarer.setIcon(icon)
        self.boutonDemarer.setObjectName("boutonDemarer")
        self.horizontalLayout.addWidget(self.boutonDemarer)
        self.boutonPause = QtWidgets.QPushButton(self.centralwidget)
        self.boutonPause.setMinimumSize(QtCore.QSize(225, 30))
        self.boutonPause.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.boutonPause.setStyleSheet("")
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap("icon_pause.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.boutonPause.setIcon(icon1)
        self.boutonPause.setObjectName("boutonPause")
        self.horizontalLayout.addWidget(self.boutonPause)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.boutonArreter = QtWidgets.QPushButton(self.centralwidget)
        self.boutonArreter.setMinimumSize(QtCore.QSize(225, 30))
        self.boutonArreter.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.boutonArreter.setLayoutDirection(QtCore.Qt.LeftToRight)
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap("icon_stop.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.boutonArreter.setIcon(icon2)
        self.boutonArreter.setAutoDefault(False)
        self.boutonArreter.setObjectName("boutonArreter")
        self.horizontalLayout_2.addWidget(self.boutonArreter)
        self.boutonRedemarrer = QtWidgets.QPushButton(self.centralwidget)
        self.boutonRedemarrer.setMinimumSize(QtCore.QSize(225, 30))
        self.boutonRedemarrer.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap("icon_redemarrer.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.boutonRedemarrer.setIcon(icon3)
        self.boutonRedemarrer.setObjectName("boutonRedemarrer")
        self.horizontalLayout_2.addWidget(self.boutonRedemarrer)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.label.setText(_translate("MainWindow", "Choisir la prescription"))
        self.boutonInfo.setToolTip(_translate("MainWindow", "<html><head/><body><p><span style=\" color:#000000;\">Informations sur la prescription sélectionnée.</span></p></body></html>"))
        self.boutonInfo.setText(_translate("MainWindow", "i"))
        self.listePrescriptions.setSortingEnabled(False)
        __sortingEnabled = self.listePrescriptions.isSortingEnabled()
        self.listePrescriptions.setSortingEnabled(False)
        item = self.listePrescriptions.item(0)
        item.setText(_translate("MainWindow", "test_1"))
        item = self.listePrescriptions.item(1)
        item.setText(_translate("MainWindow", "test_2"))
        item = self.listePrescriptions.item(2)
        item.setText(_translate("MainWindow", "test_3"))
        item = self.listePrescriptions.item(3)
        item.setText(_translate("MainWindow", "test_4"))
        self.listePrescriptions.setSortingEnabled(__sortingEnabled)
        self.label_2.setText(_translate("MainWindow", "Messages"))
        self.boutonDemarer.setText(_translate("MainWindow", "Démarrer la distribution"))
        self.boutonPause.setText(_translate("MainWindow", "Pause"))
        self.boutonArreter.setText(_translate("MainWindow", "Arrêter"))
        self.boutonRedemarrer.setText(_translate("MainWindow", "Continuer la distribution"))


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
