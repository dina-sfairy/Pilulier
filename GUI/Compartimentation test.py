import numpy as np

# # Pour des fins de tests voici un vecteur de prescription pour faire tes tests
# ligneDePrescription = np.array([1, 1, 1, 1, 1, 1, 1], dtype=np.uint8)
#
# # TODO: Creer le vecteur de distribution
# vecteurDeDistribution = np.array([], dtype=np.uint8)
#
# # Initialiser le compteur d'incréments
# compteur = 0
# for i in range(7):
#     if ligneDePrescription[i] != 0:
#         compteur = compteur + 1
#         vecteurDeDistribution = np.append(vecteurDeDistribution, compteur)
#         if ligneDePrescription[i] > 1:
#             for j in range(ligneDePrescription[i]-1):
#                 vecteurDeDistribution = np.append(vecteurDeDistribution, 0)
#         compteur = 0
#     else:
#         compteur = compteur + 1
# if compteur != 0:
#     vecteurDeDistribution = np.append(vecteurDeDistribution, compteur)
#
# print(ligneDePrescription)
# print(vecteurDeDistribution)

vecteurTailles = np.array([], dtype=np.uint8)
momentCourant = 0



def genererVecteurDeDistribution(ligneDePrescription):
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

    # Pour des fins de tests voici un vecteur de prescription pour faire tes tests

    # TODO: Creer le vecteur de distribution
    vecteurDeDistribution = np.array([], dtype=np.uint8)

    # Utilisation d'une variable globale pour construire vecteur de tailles
    global vecteurTailles

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

    vecteurTailles = np.append(vecteurTailles, vecteurDeDistribution.shape[0])

    return vecteurDeDistribution, vecteurTailles

ligneDePrescription = np.array([3, 0, 2, 0, 1, 1, 1], dtype=np.uint8)
vecteurDist, vecteurT = genererVecteurDeDistribution(ligneDePrescription)
print(vecteurDist)

ligneDePrescription = np.array([1, 0, 1, 0, 1, 0, 1], dtype=np.uint8)
vecteurDist, vecteurT = genererVecteurDeDistribution(ligneDePrescription)
print(vecteurDist)

ligneDePrescription = np.array([0, 0, 0, 0, 0, 0, 0], dtype=np.uint8)
vecteurDist, vecteurT = genererVecteurDeDistribution(ligneDePrescription)
print(vecteurDist)

ligneDePrescription = np.array([1, 1, 1, 1, 2, 1, 0], dtype=np.uint8)
vecteurDist, vecteurT = genererVecteurDeDistribution(ligneDePrescription)
print(vecteurDist)
print(vecteurT)

