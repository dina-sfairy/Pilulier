import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from concept_gui2 import Ui_MainWindow
import numpy as np
import serial
import time
import threading

class PilulePrescrite:
    def __init__(self, nom, matriceDistribution):
        self.nom = nom
        self.matriceDistribution = matriceDistribution

class ApplicationPilulier:
    def __init__(self, MainWindow):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(MainWindow)
        self.ui.messagesTextEdit.setReadOnly(True)
        self.ui.boutonArreter.setEnabled(False)
        self.ui.listePrescriptions.setCurrentRow(0) # Par défaut, PyQt retourne 0 lorsqu'on appelle currentRow. Cette
                                                    # ligne fait ensorte que l'item soit surligné au démarage.

        # Connecter les boutons aux méthodes qui leur sont associées
        self.ui.boutonDemarer.clicked.connect(self.onDemarerClicked)
        self.ui.boutonArreter.clicked.connect(self.onArreterClicked)

        # Créer un timer qui va refresh les instructions lorsque nécéssaire
        self.checkMessageTimer = QtCore.QTimer()
        self.checkMessageTimer.timeout.connect(self.updateMessage)
        self.checkMessageTimer.start(300)
        self.messageNeedsUpdate = False

        # Créer la liste de pillules prescrites (initialement vide)
        self.prescription = []

        # Créer une variable qui servira de contrôle pour l'arrêt du système
        self.systemControl = 0

        # Démarer la communication avec l'arduino
        self.serPort = serial.Serial('COM6', 9600)
        time.sleep(3) # Il faut donner le temps au Arduino de reset

    def onDemarerClicked(self):
        print("Démarage")
        self.ui.boutonDemarer.setEnabled(False)
        self.ui.boutonArreter.setEnabled(True)
        prescriptionSelectionnee = self.ui.listePrescriptions.currentItem().text()
        print(prescriptionSelectionnee)
        nomFichierPrescription = "prescription" + str(self.ui.listePrescriptions.currentRow()+1) + ".txt"
        self.lirePrescription(nomFichierPrescription)
        self.afficherPrescription()

        self.prescriptionEnCoursIndex = 0 # sert à suivre où nous sommes rendu dans la distribution
        self.ui.messagesTextEdit.setText("Verser les pilules de type "
                                         + self.prescription[self.prescriptionEnCoursIndex].nom
                                         +" dans le système.")

        # self.threadCommunication()
        t = threading.Thread(target=self.threadCommunication)
        t.start()

    def onArreterClicked(self):
        print("Arrêt")
        self.systemControl = 1

    def threadCommunication(self):
        self.serPort.write(bytes([10]))
        while not (self.serPort.in_waiting):
            if self.systemControl == 1:
                self.serPort.write(bytes([11]))
                self.systemControl = 0
            time.sleep(0.200)
        ligneLue = self.serPort.readline()
        print(ligneLue)
        ligneLue = str(ligneLue.decode('utf-8'))
        self.messageAAfficher = ligneLue
        self.messageNeedsUpdate = True
        self.ui.boutonArreter.setEnabled(False)
        self.ui.boutonDemarer.setEnabled(True)

    def envoyerMatrice(self, matricePrescription):
        for i in range(7):
            for j in range(4):
                self.serPort.write(bytes([matricePrescription[i, j]]))

    def updateMessage(self):
        if self.messageNeedsUpdate is True:
            self.ui.messagesTextEdit.setText(self.messageAAfficher)
            self.messageNeedsUpdate = False

    def lirePrescription(self, nomFichierPrescription):
        # Réinitialiser la liste de pilules prescrites
        self.prescription = []

        # Lire le fichier de prescription
        fichierPrescription = open(nomFichierPrescription, "r")
        nomPillule = fichierPrescription.readline().split()[0]
        while(nomPillule != "fin"):
            matricePrescription = np.zeros([7, 4], dtype=np.uint8)
            for i in range(7):
                ligneLue = fichierPrescription.readline().split()
                for j in range(4):
                    matricePrescription[i, j] = int(ligneLue[j])
            # Ajouter la prescription de la pilule lue à la liste de prescription
            self.prescription.append(PilulePrescrite(nomPillule, matricePrescription))
            nomPillule = fichierPrescription.readline().split()[0]
        fichierPrescription.close()

    def genererVecteurDeDistribution(self, matriceDeDistribution):
        """
        Cette méthode génère un vecteur de distribution
        :param matriceDeDistribution: matrice represantant, pour une pilule donnée, la quantité à placer dans le pilulier
         à chaque jour de la semaine selon le moment de la journée
        :type matriceDeDistribution: NumPy array de taille 7x4 (7 lignes, 4 colonnes)
        :return vecteurTaille: Vecteur contenant le nombre d'éléments à envoyer pour chaque moment de la journée
        :rtype NumPy array 1D avec une taille de 4
        :return matriceDeDeplacement: Matrice décrivant combien de cases il faut déplacer le tapis de compartimentation à chaque étape de
        celui-ci pour la pilule donnée.
        :rtype matriceDeDeplacement: NumPy array de taille 21x4
        """
        # TODO: Creer le vecteur de taille et la matrice de distribution
        vecteurTaille = np.zeros(4, dtype=np.uint8)
        matriceDeDeplacement = np.zeros([21, 4], dtype=np.uint8)

        vecteurDeDistribution = np.array([], dtype=np.uint8)

        # Initialiser le compteur d'incréments
        compteur = 0
        for i in range(7):
            if ligneDePrescription[i] != 0:
                compteur = compteur + 1
                vecteurDeDistribution = np.append(vecteurDeDistribution, compteur)
                if ligneDePrescription[i] > 1:
                    for j in range(ligneDePrescription[i] - 1):
                        vecteurDeDistribution = np.append(vecteurDeDistribution, 0)
                compteur = 0
            else:
                compteur = compteur + 1

        if vecteurDeDistribution is not None:
            vecteurDeDistribution = np.append(vecteurDeDistribution, 8)

        # Pour des fins de tests voici un vecteur de prescription pour faire tes tests
        # vecteurPrescriptionPourTester = np.array([1, 0, 3, 0, 2, 1, 2], dtype=np.uint8)

        return vecteurTaille, vecteurDeDistribution

    def afficherPrescription(self):
        print("Voici la prescription sélectionnée :")
        for pilules in self.prescription:
            print(pilules.nom)
            print(pilules.matriceDistribution)
            

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    appliPilulier = ApplicationPilulier(MainWindow)
    ui = appliPilulier.ui
    MainWindow.show()
    sys.exit(app.exec_())
    
