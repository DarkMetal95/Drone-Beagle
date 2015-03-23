# Todo


## Urgent:

* Mode debug
* Réception bluetooth code erreur : acknowledge


## Améliorations possibles:

* Ajouter diode de dysfonctionnement sur sortie GPIO BBB

# Liste des commandes
##Toutes les commandes:

	't': Altitude +
	'g': Altitude -
	'z': Déplacement avant
	'q': Déplacement latéral gauche
	's': Déplacement arrière
	'd': Déplacement latéral droit
	'k': Déconnection bluetooth
	'l': Extinction de la BBB
	'a': Activation du mode débug

##Pour paramétrer les coefficients:

Envoyer d'abord la valeur 0x01.

Puis envoyer la valeur sous forme d'un tableau de 5 char, dont le 5ème correspond au coefficient à modifier:
	0x00 : Kp
	0x01 : Ki
	0x02 : Kd

	[0x01]	['1']['0']['0']['0'][0x00] : Kp = 1000
	[0x01]	['3']['0']['0'][ 0 ][0x01] : Ki = 300
	[0x01]	['5']['0'][ 0 ][ 0 ][0x02] : Kd = 50

La valeur max est donc 9999.