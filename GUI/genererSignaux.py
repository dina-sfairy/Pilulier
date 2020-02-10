from gtts import gTTS

listeMessages =[
    ("messagePilulier.mp3", "Le pilulier est mal placé."),
    ("messageManquePilules.mp3", "Veuillez ajouter plus de pilules."),
    ("messageTypeComplet.mp3", "La distribution du type de pilule en cours est terminée."),
    ("messagePrescriptionTerminee.mp3", "Le pilulier est rempli."),
    ("messagePurge.mp3", "Le contenant de purge est mal placé.")
]

for message in listeMessages:
    tts = gTTS(text=message[1], lang="fr", slow=False)
    tts.save(message[0])

print("Messages générés.")
