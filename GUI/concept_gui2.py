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
        MainWindow.resize(400, 294)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtWidgets.QLabel(self.centralwidget)
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.listePrescriptions = QtWidgets.QListWidget(self.centralwidget)
        self.listePrescriptions.setResizeMode(QtWidgets.QListView.Adjust)
        self.listePrescriptions.setViewMode(QtWidgets.QListView.IconMode)
        self.listePrescriptions.setObjectName("listePrescriptions")
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
        item = QtWidgets.QListWidgetItem()
        self.listePrescriptions.addItem(item)
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
        font.setBold(True)
        font.setWeight(75)
        self.label_2.setFont(font)
        self.label_2.setObjectName("label_2")
        self.verticalLayout.addWidget(self.label_2)
        self.messagesTextEdit = QtWidgets.QTextEdit(self.centralwidget)
        self.messagesTextEdit.setObjectName("messagesTextEdit")
        self.verticalLayout.addWidget(self.messagesTextEdit)
        self.boutonDemarer = QtWidgets.QPushButton(self.centralwidget)
        self.boutonDemarer.setObjectName("boutonDemarer")
        self.verticalLayout.addWidget(self.boutonDemarer)
        self.boutonRedemarrer = QtWidgets.QPushButton(self.centralwidget)
        self.boutonRedemarrer.setObjectName("boutonRedemarrer")
        self.verticalLayout.addWidget(self.boutonRedemarrer)
        self.boutonArreter = QtWidgets.QPushButton(self.centralwidget)
        self.boutonArreter.setObjectName("boutonArreter")
        self.verticalLayout.addWidget(self.boutonArreter)
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.label.setText(_translate("MainWindow", "Choisir la prescription"))
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
        item = self.listePrescriptions.item(4)
        item.setText(_translate("MainWindow", "test_5"))
        item = self.listePrescriptions.item(5)
        item.setText(_translate("MainWindow", "test_6"))
        item = self.listePrescriptions.item(6)
        item.setText(_translate("MainWindow", "test_7"))
        item = self.listePrescriptions.item(7)
        item.setText(_translate("MainWindow", "demo"))
        self.listePrescriptions.setSortingEnabled(__sortingEnabled)
        self.label_2.setText(_translate("MainWindow", "Instructions"))
        self.boutonDemarer.setText(_translate("MainWindow", "Démarrer"))
        self.boutonRedemarrer.setText(_translate("MainWindow", "Redémarrer"))
        self.boutonArreter.setText(_translate("MainWindow", "Arrêter"))


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
