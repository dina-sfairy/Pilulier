import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from concept_gui2 import Ui_MainWindow
import numpy as np
import serial  # de la librairie pySerial (et non serial)
import time
import threading
from pygame import mixer  # Pour jouer des sons


class PilulePrescrite:
    """
    Ceci est une classe qui représente une pilule prescrite. Une pilules est simplement caractérisée par son nom et
    une matrice de distribution (7 lignes, 4 colonnes) indiquant combien de pilules doivent être placés dans le
    pilulier pour chaque moment de la journée.
    """
    def __init__(self, nom, matriceDistribution):
        self.nom = nom
        self.matriceDistribution = matriceDistribution


class ApplicationPilulier:
    """
    Cette classe est le coeur du programme. C'est cette classe qui s'occupe de gérer l'interface, la communication
    et le traitement de données.
    """
    def __init__(self, MainWindow):
        """
        Ceci est le constructeur de la classe ApplicationPilulier. Il est appelé au moment où la classe est créée.
        :param MainWindow: L'objet qui décrit la géométrie de l'interface, ainsi que son contenu (boutons,
        noms des boutons, cases, etc.)
        """
        self.ui = Ui_MainWindow()
        self.ui.setupUi(MainWindow)
        self.ui.messagesTextEdit.setReadOnly(True)
        self.ui.boutonArreter.setEnabled(False)
        self.ui.boutonRedemarrer.setEnabled(False)
        self.ui.boutonPause.setEnabled(False)
        self.ui.listePrescriptions.setCurrentRow(0)  # Par défaut, PyQt retourne 0 lorsqu'on appelle currentRow. Cette
        # ligne fait ensorte que l'item soit surligné au démarage.

        # Connecter les boutons aux méthodes qui leur sont associées
        self.ui.boutonDemarer.clicked.connect(self.onDemarerClicked)
        self.ui.boutonArreter.clicked.connect(self.onArreterClicked)
        self.ui.boutonRedemarrer.clicked.connect(self.onRedemarrerClicked)
        self.ui.boutonInfo.clicked.connect(self.onInfoClicked)

        # Créer un timer qui va refresh les instructions lorsque nécéssaire
        self.checkMessageTimer = QtCore.QTimer()
        self.checkMessageTimer.timeout.connect(self.updateMessage)
        self.checkMessageTimer.start(300)
        self.messageNeedsUpdate = False

        # Créer la liste de pillules prescrites (initialement vide)
        self.prescription = []

        # Créer une variable qui servira de contrôle pour l'arrêt du système
        self.systemControl = 0

        # Démarer la communication avec l'arduino (décommenter lorsqu'on travaille avec le prototype physique)
        # self.serPort = serial.Serial('COM16', 115200)
        # time.sleep(3)  # Il faut donner le temps au Arduino de reset

    def onDemarerClicked(self):
        print("Démarage")
        self.ui.boutonDemarer.setEnabled(False)
        self.ui.boutonArreter.setEnabled(True)
        self.ui.boutonPause.setEnabled(True)
        prescriptionSelectionnee = self.ui.listePrescriptions.currentItem().text()
        print(prescriptionSelectionnee)
        nomFichierPrescription = "prescription" + str(self.ui.listePrescriptions.currentRow() + 1) + ".txt"
        self.lirePrescription(nomFichierPrescription)  # Lire la prescription
        self.afficherPrescription()

        self.prescriptionEnCoursIndex = 0  # sert à suivre où nous sommes rendu dans la distribution
        self.ui.messagesTextEdit.setText("Verser au moins "
                                         + str(self.getNombrePilulesRequis(self.prescriptionEnCoursIndex))
                                         + " pillules du type "
                                         + self.prescription[self.prescriptionEnCoursIndex].nom
                                         + " dans le système.")

        # Fonction pour afficher les messages lorsqu'on n'a pas accès au prototype physique
        accesAuPrototype = False
        if accesAuPrototype is False:
            self.afficherMessage(6)
            return

        # self.threadCommunication()
        t = threading.Thread(target=self.threadCommunication)
        t.start()  # Démmarre la fonction threadCommunication dans un nouveau thread

    def onRedemarrerClicked(self):
        self.systemControl = 2
        self.ui.boutonArreter.setEnabled(True)
        self.ui.boutonRedemarrer.setEnabled(False)
        self.ui.boutonPause.setEnabled(True)

    def onArreterClicked(self):
        print("Arrêt")
        self.systemControl = 1

    def onPauseClicked(self):
        print("Pause")
        self.systemControl = 3
        self.ui.boutonArreter.setEnabled(True)
        self.ui.boutonRedemarrer.setEnabled(True)
        self.ui.boutonPause.setEnabled(False)

    def onInfoClicked(self):
        """
        Cette fonction lit le fichier .txt de la prescription sélectionnée et l'affiche dans une fenêtre pop-up
        """
        messageContenu = "Contenu de la prescription : \t \t  \n \n"
        nomFichierPrescription = "prescription" + str(self.ui.listePrescriptions.currentRow() + 1) + ".txt"
        fichierPrescription = open(nomFichierPrescription, 'r')
        contenuFichier = fichierPrescription.read()
        messageContenu = messageContenu + contenuFichier
        fichierPrescription.close()

        fenetreInfo = QtWidgets.QMessageBox()
        fenetreInfo.setWindowTitle("Informations sur la prescription")
        fenetreInfo.setText(messageContenu)
        fenetreInfo.exec()

    def threadCommunication(self):
        """
        Cette fonction effectue la communication entre l'interface et le microcontrôleur.
        Note: puisque les interfaces pyQt ne sont pas thread-safe, il n'est pas possible de modifier les messages sur
        l'interface directement à partir de ce thread. C'est pour celà que la fonction updateMessage est appelée
        à partir du main thread toutes les 0.3 secondes.
        """
        # Envoyer le signal de départ
        self.serPort.write(bytes([1]))
        self.envoyerVecteursAuUC(self.prescriptionEnCoursIndex)

        while(self.prescriptionEnCoursIndex < self.prescription.__len__()):
            # Attendre de recevoir une réponse du microcontroleur
            while not (self.serPort.in_waiting):
                if self.systemControl == 3:  # Si l'utilisateur a appuyé sur le bouton de pause
                    self.serPort.write(bytes([5]))
                    self.attendreDeRedémarerOuArrêter()
                if self.systemControl == 1:  # Si l'utilisateur a appuyé sur le bouton d'arrêt
                    self.serPort.write(bytes([2]))
                    self.systemControl = 0
                time.sleep(0.200)

            # Lire le message du uC
            ligneLue = self.serPort.readline()
            try:
                ligneLue = str(ligneLue.decode('utf-8'))
            except Exception:
                ligneLue = "Il y eu une erreur, mais je l'ai catch."
            ligneLue = ligneLue.strip()
            print(ligneLue)

            if (ligneLue == "e1"):
                self.messageAAfficher = "Le pilulier est mal placé."
                self.messageNeedsUpdate = True
                mixer.music.load('messagePilulier.mp3')
                mixer.music.play()
                self.ui.boutonArreter.setEnabled(False)
                self.ui.boutonRedemarrer.setEnabled(True)
                self.ui.boutonPause.setEnabled(False)
                return
            elif (ligneLue == "e2"):
                self.messageAAfficher = "Le contenant de purge est mal placé."
                self.messageNeedsUpdate = True
                mixer.music.load('messagePurge.mp3')
                mixer.music.play()
                self.ui.boutonArreter.setEnabled(False)
                self.ui.boutonRedemarrer.setEnabled(True)
                self.ui.boutonPause.setEnabled(False)
                return
            elif (ligneLue == "e3"):
                self.messageAAfficher = "Veuillez ajouter plus de pillules du type " \
                                        + self.prescription[self.prescriptionEnCoursIndex].nom + " dans le système" \
                                        + " et appuyez sur le bouton 'redémarrer'."
                self.messageNeedsUpdate = True
                mixer.music.load('messageManquePilules.mp3')
                mixer.music.play()
                self.ui.boutonArreter.setEnabled(False)
                self.ui.boutonPause.setEnabled(False)
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
                self.ui.boutonPause.setEnabled(False)
                return
            elif (ligneLue == "f"):
                self.prescriptionEnCoursIndex = self.prescriptionEnCoursIndex + 1
                if self.prescriptionEnCoursIndex < self.prescription.__len__():  # Vérifier si c'était la dernière pill

                    # Attendre que l'usager appuie sur redémarer
                    self.ui.boutonArreter.setEnabled(False)
                    self.ui.boutonRedemarrer.setEnabled(True)
                    self.ui.boutonPause.setEnabled(False)
                    self.messageAAfficher = "Verser au moins " \
                                            + str(self.getNombrePilulesRequis(self.prescriptionEnCoursIndex)) \
                                            + " pillules du type " \
                                            + self.prescription[self.prescriptionEnCoursIndex].nom \
                                            + " dans le système."
                    self.messageNeedsUpdate = True
                    mixer.music.load('messageTypeComplet.mp3')
                    mixer.music.play()
                    while (self.systemControl is not 2):  # L'utilisateur doit appuyer sur le bouton redémarrer
                        time.sleep(0.2)
                    self.systemControl = 0
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
        self.ui.boutonPause.setEnabled(False)
        return

    def attendreDeRedémarerOuArrêter(self):
        while self.systemControl == 3:  # attendre que l'utilisateur appuie sur arrêter ou redémarer
            time.sleep(0.2)

        if self.systemControl == 2:  # Si l'utilisateur appuie sur redémarer
            self.serPort.write(bytes([3]))
            self.systemControl = 0
        # Si l'utilisateur a appuyé sur le bouton arrêter, le reste de la routine du thread de communication
        # devrait s'encharger
        return

    def getNombrePilulesRequis(self, prescriptionEnCoursIndex):
        """
        Cette fonction retourne le nombre minimum de pilules du type en cours qu'il faut verser dans le système
        :param prescriptionEnCoursIndex: index qui indique à quel type de pilules on est rendu
        :type prescriptionEnCoursIndex: int
        :return: le nombre minimum de pilules du type en cours qu'il faut verser dans le système.
        :rtype: int
        """
        matriceDePrescription = self.prescription[prescriptionEnCoursIndex].matriceDistribution
        return matriceDePrescription.sum()

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
        """
        Old. N'est plus utilisé.
        """
        for i in range(7):
            for j in range(4):
                self.serPort.write(bytes([matricePrescription[i, j]]))

    def afficherMessage(self, numeroMessage):
        if numeroMessage is 1:
            self.messageAAfficher = "Le pilulier est mal placé."
            self.messageNeedsUpdate = True
            mixer.music.load('messagePilulier.mp3')
            mixer.music.play()
            self.ui.boutonArreter.setEnabled(False)
            self.ui.boutonRedemarrer.setEnabled(True)
            self.ui.boutonPause.setEnabled(False)
        elif numeroMessage is 2:
            self.messageAAfficher = "Le contenant de purge est mal placé."
            self.messageNeedsUpdate = True
            mixer.music.load('messagePurge.mp3')
            mixer.music.play()
            self.ui.boutonArreter.setEnabled(False)
            self.ui.boutonRedemarrer.setEnabled(True)
            self.ui.boutonPause.setEnabled(False)
        elif numeroMessage is 3:
            self.messageAAfficher = "Veuillez ajouter plus de pillules du type " \
                                    + self.prescription[self.prescriptionEnCoursIndex].nom + " dans le système" \
                                    + " et appuyez sur le bouton 'redémarrer'."
            self.messageNeedsUpdate = True
            mixer.music.load('messageManquePilules.mp3')
            mixer.music.play()
            self.ui.boutonArreter.setEnabled(False)
            self.ui.boutonPause.setEnabled(False)
            self.ui.boutonRedemarrer.setEnabled(True)
        elif numeroMessage is 4:
            self.messageAAfficher = "Arrêt du système."
            self.messageNeedsUpdate = True
            self.ui.boutonArreter.setEnabled(False)
            self.ui.boutonDemarer.setEnabled(True)
            self.ui.boutonPause.setEnabled(False)
        elif numeroMessage is 5:
            self.ui.boutonArreter.setEnabled(False)
            self.ui.boutonRedemarrer.setEnabled(True)
            self.ui.boutonPause.setEnabled(False)
            self.messageAAfficher = " Vider la purge et verser au moins " \
                                    + str(self.getNombrePilulesRequis(self.prescriptionEnCoursIndex)) \
                                    + " pillules du type " \
                                    + self.prescription[self.prescriptionEnCoursIndex].nom \
                                    + " dans le système."
            self.messageNeedsUpdate = True
            mixer.music.load('messageTypeComplet.mp3')
            mixer.music.play()
        elif numeroMessage is 6:
            self.messageAAfficher = "La prescription est terminée."
            self.messageNeedsUpdate = True
            mixer.music.load('messagePrescriptionTerminee.mp3')
            mixer.music.play()
            self.ui.boutonArreter.setEnabled(False)
            self.ui.boutonDemarer.setEnabled(True)
            self.ui.boutonPause.setEnabled(False)

    def updateMessage(self):
        """
        Cette méthode est appelée à chaque 0.3 secondes à partir du main thread pour vérifier s'il faut mettre
        les instructions à jour. Ceci doit être fait à partir du main thread puisque les interfaces pyQt ne sont pas
        thread safe.
        """
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
