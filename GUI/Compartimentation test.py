import numpy as np

# Pour des fins de tests voici un vecteur de prescription pour faire tes tests
ligneDePrescription = np.array([1, 1, 1, 1, 1, 1, 1], dtype=np.uint8)

# TODO: Creer le vecteur de distribution
vecteurDeDistribution = np.array([], dtype=np.uint8)

# Initialiser le compteur d'incréments
compteur = 0
for i in range(7):
    if ligneDePrescription[i] != 0:
        compteur = compteur + 1
        vecteurDeDistribution = np.append(vecteurDeDistribution, compteur)
        if ligneDePrescription[i] > 1:
            for j in range(ligneDePrescription[i]-1):
                vecteurDeDistribution = np.append(vecteurDeDistribution, 0)
        compteur = 0
    else:
        compteur = compteur + 1
if compteur != 0:
    vecteurDeDistribution = np.append(vecteurDeDistribution, compteur)

print(ligneDePrescription)
print(vecteurDeDistribution)


