import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from concept_gui2 import Ui_MainWindow
import numpy as np
import serial  # de la librairie pySerial (et non serial)
import time
import threading
from pygame import mixer  # Pour jouer des sons


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
        self.ui.boutonRedemarrer.setEnabled(False)
        self.ui.listePrescriptions.setCurrentRow(0)  # Par défaut, PyQt retourne 0 lorsqu'on appelle currentRow. Cette
        # ligne fait ensorte que l'item soit surligné au démarage.

        # Connecter les boutons aux méthodes qui leur sont associées
        self.ui.boutonDemarer.clicked.connect(self.onDemarerClicked)
        self.ui.boutonArreter.clicked.connect(self.onArreterClicked)
        self.ui.boutonRedemarrer.clicked.connect(self.onRedemarrerClicked)

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
        self.serPort = serial.Serial('COM16', 115200)
        time.sleep(3)  # Il faut donner le temps au Arduino de reset

    def onDemarerClicked(self):
        print("Démarage")
        self.ui.boutonDemarer.setEnabled(False)
        self.ui.boutonArreter.setEnabled(True)
        prescriptionSelectionnee = self.ui.listePrescriptions.currentItem().text()
        print(prescriptionSelectionnee)
        nomFichierPrescription = "prescription" + str(self.ui.listePrescriptions.currentRow() + 1) + ".txt"
        self.lirePrescription(nomFichierPrescription)
        self.afficherPrescription()

        self.prescriptionEnCoursIndex = 0  # sert à suivre où nous sommes rendu dans la distribution
        self.ui.messagesTextEdit.setText("Verser les pilules de type "
                                         + self.prescription[self.prescriptionEnCoursIndex].nom
                                         + " dans le système.")

        # self.threadCommunication()
        t = threading.Thread(target=self.threadCommunication)
        t.start()

    def onRedemarrerClicked(self):
        self.systemControl = 2
        self.ui.boutonArreter.setEnabled(True)
        self.ui.boutonRedemarrer.setEnabled(False)

    def onArreterClicked(self):
        print("Arrêt")
        self.systemControl = 1

    def threadCommunication(self):
        # Envoyer le signal de départ
        self.serPort.write(bytes([1]))
        self.envoyerVecteursAuUC(self.prescriptionEnCoursIndex)

        while(self.prescriptionEnCoursIndex < self.prescription.__len__()):
            # Attendre de recevoir une réponse du microcontroleur
            while not (self.serPort.in_waiting):
                if self.systemControl == 1:  # L'utilisateur a appuyé sur le bouton d'arrêt
                    self.serPort.write(bytes([2]))
                    self.systemControl = 0
                time.sleep(0.200)

            # Lire le message du uC
            ligneLue = self.serPort.readline()
            ligneLue = str(ligneLue.decode('utf-8'))
            ligneLue = ligneLue.strip()
            print(ligneLue)
            if (ligneLue == "e1"):
                self.messageAAfficher = "Le pilulier est mal placé."
                self.messageNeedsUpdate = True
                mixer.music.load('messagePilulier.mp3')
                mixer.music.play()
                self.ui.boutonArreter.setEnabled(False)
                self.ui.boutonDemarer.setEnabled(True)
                return
            elif (ligneLue == "e2"):
                self.messageAAfficher = "Le contenant de purge est mal placé."
                self.messageNeedsUpdate = True
                mixer.music.load('messagePurge.mp3')
                mixer.music.play()
                self.ui.boutonArreter.setEnabled(False)
                self.ui.boutonDemarer.setEnabled(True)
                return
            elif (ligneLue == "e3"):
                self.messageAAfficher = "Veuillez ajouter plus de pillules du type " \
                                        + self.prescription[self.prescriptionEnCoursIndex].nom + " dans le système" \
                                        + " et appuyez sur le bouton 'redémarrer'."
                self.messageNeedsUpdate = True
                mixer.music.load('messageManquePilules.mp3')
                mixer.music.play()
                self.ui.boutonArreter.setEnabled(False)
                self.ui.boutonRedemarrer.setEnabled(True)
                while(self.systemControl is not 2): # L'utilisateur doit appuyer sur le bouton redémarrer
                    time.sleep(0.2)
                self.systemControl = 0
                self.serPort.write(bytes([3]))

            elif (ligneLue == "ok"):
                self.messageAAfficher = "Arrêt du système."
                self.messageNeedsUpdate = True
                self.ui.boutonArreter.setEnabled(False)
                self.ui.boutonDemarer.setEnabled(True)
                return
            elif (ligneLue == "f"):
                self.prescriptionEnCoursIndex = self.prescriptionEnCoursIndex + 1
                if self.prescriptionEnCoursIndex < self.prescription.__len__():  # Vérifier si c'était la dernière pill
                    self.messageAAfficher = "Verser les pilules de type " \
                                            + self.prescription[self.prescriptionEnCoursIndex].nom \
                                            + " dans le système."
                    self.messageNeedsUpdate = True
                    mixer.music.load('messageTypeComplet.mp3')
                    mixer.music.play()
                    time.sleep(0.3)
                    self.serPort.write(bytes([1]))
                    self.envoyerVecteursAuUC(self.prescriptionEnCoursIndex)
            else:
                print("Commande non-reconnue")

        self.serPort.write(bytes([4]))
        self.messageAAfficher = "La prescription est terminée."
        self.messageNeedsUpdate = True
        mixer.music.load('messagePrescriptionTerminee.mp3')
        mixer.music.play()
        self.ui.boutonArreter.setEnabled(False)
        self.ui.boutonDemarer.setEnabled(True)
        return

    def envoyerVecteursAuUC(self, prescriptionEnCoursIndex):
        # Initialiser les vecteurs à envoyer
        vecteurTailles = np.array([], dtype=np.uint8)
        vecteursDistributionListe = []

        matricePrescriptionEnCours = self.prescription[prescriptionEnCoursIndex].matriceDistribution

        for i in range(4):
            lignePrescription = matricePrescriptionEnCours[:, i]
            vecteurDistribution = self.genererVecteurDeDistribution(lignePrescription)
            vecteursDistributionListe.append(vecteurDistribution)
            vecteurTailles = np.append(vecteurTailles, vecteurDistribution.size)

        # Envoyer le vecteur de tailles
        vecteurTailles = vecteurTailles.astype(dtype=np.uint8)
        for i in range(vecteurTailles.size):
            self.serPort.write(bytes([vecteurTailles[i]]))

        # Envoyer les vecteurs de distribution
        for i in range(vecteursDistributionListe.__len__()):
            vecteurAEnvoyer = vecteursDistributionListe[i]
            for j in range(vecteurAEnvoyer.size):
                self.serPort.write(bytes([vecteurAEnvoyer[j]]))

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
        while (nomPillule != "fin"):
            matricePrescription = np.zeros([7, 4], dtype=np.uint8)
            for i in range(7):
                ligneLue = fichierPrescription.readline().split()
                for j in range(4):
                    matricePrescription[i, j] = int(ligneLue[j])
            # Ajouter la prescription de la pilule lue à la liste de prescription
            self.prescription.append(PilulePrescrite(nomPillule, matricePrescription))
            nomPillule = fichierPrescription.readline().split()[0]
        fichierPrescription.close()


    def genererVecteurDeDistribution(self, ligneDePrescription):
        """
        Cette méthode génère un vecteur de distribution
        :param ligneDePrescription: Vecteur représentant la prescription de la pilule pour un moment de la journée 
        :type ligneDePrescription: NumPy array 1D de taille 7. 
        :return Le vecteur de distribution décrivant les déplacements que le tapis de compartimentation doit faire.
        :rtype NumPy array 1D avec une taille de entre 0 et 21
        """

        # Creer le vecteur de distribution
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
        if vecteurDeDistribution.shape[0] != 0:
            vecteurDeDistribution = np.append(vecteurDeDistribution, 8)

        return vecteurDeDistribution

    def afficherPrescription(self):
        print("Voici la prescription sélectionnée :")
        for pilules in self.prescription:
            print(pilules.nom)
            print(pilules.matriceDistribution)


if __name__ == "__main__":
    mixer.init()
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    appliPilulier = ApplicationPilulier(MainWindow)
    ui = appliPilulier.ui
    MainWindow.show()
    sys.exit(app.exec_())
